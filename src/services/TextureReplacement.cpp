#include "TextureReplacement.h"

#include "games/GameTextureConventions.h"
#include "image/Image.h"
#include "otr/Archive.h"
#include "otr/Resource.h"
#include "otr/Texture.h"
#include "utils/Hash.h"
#include "utils/Paths.h"
#include "utils/ThreadPool.h"

#include <algorithm>
#include <mutex>
#include <set>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace TextureReplacement {
namespace {
std::string ToLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return value;
}

std::optional<TextureManifestEntry> ProcessFile(const std::string& fileName, const std::vector<uint8_t>& data,
                                                const fs::path& outputPath) {
    const auto header = Otr::ReadHeader(data);
    if (!header) {
        return std::nullopt;
    }

    if (header->type == (uint32_t)Otr::ResourceType::Texture) {
        const auto texture = Otr::Texture::Read(data);
        const auto image = texture ? Otr::TextureCodec::ToImage(*texture) : std::nullopt;
        if (!image) {
            SPDLOG_WARN("Could not decode texture {}", fileName);
            return std::nullopt;
        }
        const auto png = ImageIO::EncodePng(*image);
        if (!Paths::WriteFile(fs::path(outputPath) += ".png", png)) {
            return std::nullopt;
        }
        TextureManifestEntry entry;
        entry.hash = Sha256Hex(png);
        entry.textureType = texture->type;
        entry.textureWidth = texture->width;
        entry.textureHeight = texture->height;
        return entry;
    } else if (header->type == (uint32_t)Otr::ResourceType::OoTBackground) {
        const auto background = Otr::Background::Read(data);
        const auto size = background ? ImageIO::JpegSize(background->data) : std::nullopt;
        if (!size) {
            SPDLOG_WARN("Could not decode background {}", fileName);
            return std::nullopt;
        }
        SPDLOG_INFO("Found JPEG background: {}!", fileName);
        if (!Paths::WriteFile(fs::path(outputPath) += ".jpg", background->data)) {
            return std::nullopt;
        }
        TextureManifestEntry entry;
        entry.hash = Sha256Hex(background->data);
        entry.textureType = Otr::TextureType::JPEG32bpp;
        entry.textureWidth = size->first;
        entry.textureHeight = size->second;
        return entry;
    }
    return std::nullopt;
}
} // namespace

std::string OutputFolderName(const std::string& otrPath) {
    const std::string name = Paths::Basename(otrPath);
    return name.substr(0, name.find('.'));
}

std::optional<int> BandIndexOf(const std::string& target) {
    const auto hash = target.find_last_of('#');
    if (hash == std::string::npos || hash == 0 || hash == target.size() - 1) {
        return std::nullopt;
    }
    const std::string digits = target.substr(hash + 1);
    if (!std::all_of(digits.begin(), digits.end(), ::isdigit)) {
        return std::nullopt;
    }
    try {
        return std::stoi(digits);
    } catch (...) { return std::nullopt; }
}

std::optional<TextureManifestEntry> ResolveTextureEntry(const nlohmann::json& manifest, const std::string& target) {
    if (manifest.contains(target)) {
        return TextureManifestEntry::FromJson(manifest[target]);
    }
    for (const auto& game : AllGameTextureConventions()) {
        if (auto entry = game->ResolveAdditiveEntry(manifest, target)) {
            return entry;
        }
    }
    return std::nullopt;
}

std::vector<fs::path> ListInGlidenOrder(const fs::path& folder) {
    std::vector<fs::directory_entry> entries;
    std::error_code ec;
    for (const auto& entry : fs::directory_iterator(folder, fs::directory_options::skip_permission_denied, ec)) {
        entries.push_back(entry);
    }
    auto sortKey = [](const fs::directory_entry& entry) {
        std::string name = Paths::ToUtf8(entry.path().filename());
        std::transform(name.begin(), name.end(), name.begin(), ::toupper);
        return name;
    };
    std::stable_sort(entries.begin(), entries.end(), [&](const fs::directory_entry& a, const fs::directory_entry& b) {
        return sortKey(a) < sortKey(b);
    });

    std::vector<fs::path> files;
    for (const auto& entry : entries) {
        const std::string name = Paths::ToUtf8(entry.path().filename());
        if (name.rfind(".", 0) == 0 || name.rfind("~!~", 0) == 0) {
            continue;
        }
        if (entry.is_directory(ec)) {
            auto nested = ListInGlidenOrder(entry.path());
            files.insert(files.end(), nested.begin(), nested.end());
        } else if (entry.is_regular_file(ec)) {
            files.push_back(entry.path());
        }
    }
    return files;
}

std::optional<ProcessedFolder> ProcessFolder(const fs::path& folder) {
    const auto manifestText = Paths::ReadText(folder / "manifest.json");
    if (!manifestText) {
        return std::nullopt;
    }
    nlohmann::json manifest;
    try {
        manifest = nlohmann::json::parse(*manifestText);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Invalid manifest.json: {}", e.what());
        return std::nullopt;
    }

    std::vector<std::pair<std::string, std::vector<std::string>>> aliasList;
    if (const auto aliasText = Paths::ReadText(folder / "aliases.json")) {
        try {
            const auto aliasJson = nlohmann::ordered_json::parse(*aliasText);
            for (const auto& [source, value] : aliasJson.items()) {
                std::vector<std::string> targets;
                if (value.is_array()) {
                    for (const auto& target : value) {
                        targets.push_back(target.get<std::string>());
                    }
                } else {
                    targets.push_back(value.get<std::string>());
                }
                for (auto& target : targets) {
                    target = Paths::Normalize(target.rfind("alt/", 0) == 0 ? target.substr(4) : target);
                }
                aliasList.emplace_back(Paths::Normalize(source), std::move(targets));
            }
        } catch (const std::exception& e) { SPDLOG_ERROR("Invalid aliases.json: {}", e.what()); }
    }
    std::map<std::string, std::vector<std::string>> aliases;
    for (const auto& [source, targets] : aliasList) {
        aliases[source] = targets;
    }
    if (!aliases.empty()) {
        SPDLOG_INFO("Loaded aliases.json with {} source images", aliases.size());
    }

    std::vector<fs::path> files;
    for (const auto& file : ListInGlidenOrder(folder)) {
        const std::string ext = ToLower(file.extension().string());
        if (ext == ".png" || ext == ".jpeg" || ext == ".jpg") {
            files.push_back(file);
        }
    }
    std::vector<std::string> relativePaths;
    relativePaths.reserve(files.size());
    for (const auto& file : files) {
        relativePaths.push_back(Paths::Relative(file, folder));
    }
    const std::set<std::string> present(relativePaths.begin(), relativePaths.end());
    std::map<std::string, std::vector<std::string>> movedAliases;
    for (const auto& [source, targets] : aliasList) {
        if (source.find('/') != std::string::npos && !present.count(source)) {
            movedAliases[Paths::Basename(source)] = targets;
        }
    }

    struct Outcome {
        std::string target;
        std::vector<std::pair<spdlog::level::level_enum, std::string>> logs;
        std::optional<TextureManifestEntry> entry;
    };
    std::vector<std::vector<Outcome>> outcomes(files.size());

    ThreadPool::Instance().ParallelFor(files.size(), [&](size_t fileIndex) {
        const auto& texFile = files[fileIndex];
        const std::string& relativePath = relativePaths[fileIndex];
        const std::string name = Paths::Basename(relativePath);

        std::vector<std::string> targets = { Paths::WithoutExtension(relativePath) };
        if (auto it = aliases.find(relativePath); it != aliases.end()) {
            targets = it->second;
        } else if (auto byName = aliases.find(name); byName != aliases.end()) {
            targets = byName->second;
        } else if (auto moved = movedAliases.find(name); moved != movedAliases.end()) {
            targets = moved->second;
        }

        for (const auto& target : targets) {
            Outcome& outcome = outcomes[fileIndex].emplace_back();
            outcome.target = target;
            auto log = [&](spdlog::level::level_enum level, std::string message) {
                outcome.logs.emplace_back(level, std::move(message));
            };

            std::string textureName = target;
            std::optional<int> stripIndex;
            auto manifestEntry = ResolveTextureEntry(manifest, target);

            const auto bandIndex = manifestEntry ? std::nullopt : BandIndexOf(target);
            if (bandIndex) {
                const std::string base = target.substr(0, target.find_last_of('#'));
                const auto baseEntry = ResolveTextureEntry(manifest, base);
                if (baseEntry && !baseEntry->strips) {
                    log(spdlog::level::warn, fmt::format("Skipping {}: {} declares no bands", target, base));
                    continue;
                }
                if (baseEntry && baseEntry->strips && *bandIndex >= (int)baseEntry->strips->size()) {
                    log(spdlog::level::warn,
                        fmt::format("Skipping {}: {} has only {} bands", target, base, baseEntry->strips->size()));
                    continue;
                }
                if (baseEntry && baseEntry->strips) {
                    manifestEntry = baseEntry;
                    textureName = base;
                    stripIndex = bandIndex;
                }
            }

            if (!manifestEntry) {
                log(spdlog::level::debug, fmt::format("Found file not present in manifest: {}", target));
                continue;
            }

            if (stripIndex) {
                manifestEntry->stripIndex = stripIndex;
                log(spdlog::level::info, fmt::format("Staging band {} of {}", *stripIndex, textureName));
            } else if (manifestEntry->kind == TextureEntryKind::Replacement) {
                const auto bytes = Paths::ReadFile(texFile);
                if (bytes && manifestEntry->hash == Sha256Hex(*bytes)) {
                    continue;
                }
                log(spdlog::level::info, fmt::format("Found file with changed hash: {}", textureName));
            } else {
                log(spdlog::level::info, fmt::format("Staging additive texture: {}", textureName));
            }

            manifestEntry->targetName = textureName;
            outcome.entry = std::move(manifestEntry);
        }
    });

    ProcessedFolder processed;
    std::map<std::string, std::string> suppliedBy;
    for (size_t fileIndex = 0; fileIndex < files.size(); fileIndex++) {
        const std::string& relativePath = relativePaths[fileIndex];
        for (auto& outcome : outcomes[fileIndex]) {
            if (const auto earlier = suppliedBy.find(outcome.target); earlier != suppliedBy.end()) {
                SPDLOG_DEBUG("Skipping {} for {}: {} comes first", relativePath, outcome.target, earlier->second);
                continue;
            }
            for (const auto& [level, message] : outcome.logs) {
                spdlog::log(level, message);
            }
            if (!outcome.entry) {
                continue;
            }
            suppliedBy[outcome.target] = relativePath;
            const std::string key = Paths::Dirname(*outcome.entry->targetName);
            processed[key].emplace_back(files[fileIndex], std::move(*outcome.entry));
        }
    }
    return processed;
}

std::optional<ExtractedTextures> ProcessOtr(const std::vector<std::string>& otrPaths, const fs::path& outputDir) {
    if (otrPaths.empty()) {
        return std::nullopt;
    }
    const fs::path dir = outputDir / OutputFolderName(otrPaths.front());
    std::error_code ec;
    if (fs::exists(dir, ec)) {
        SPDLOG_INFO("Deleting existing folder: {}", Paths::ToUtf8(dir));
        fs::remove_all(dir, ec);
    }

    ExtractedTextures processed;
    for (const auto& otrPath : otrPaths) {
        SPDLOG_INFO("Processing OTR: {}", otrPath);
        auto archive = Otr::ArchiveReader::Open(otrPath);
        if (!archive) {
            continue;
        }
        const auto names = archive->List();
        std::mutex processedMutex;
        ThreadPool::Instance().ParallelFor(names.size(), [&](size_t i) {
            const auto data = archive->Read(names[i]);
            if (!data) {
                return;
            }
            if (auto entry = ProcessFile(names[i], *data, dir / Paths::FromUtf8(names[i]))) {
                std::lock_guard lock(processedMutex);
                processed[names[i]] = std::move(*entry);
            }
        });
    }

    for (const auto& game : AllGameTextureConventions()) {
        game->RecordExtractionMetadata(otrPaths, processed);
    }
    if (processed.empty()) {
        return std::nullopt;
    }
    return processed;
}

std::optional<TextureManifestEntry> DumpFont(const fs::path& outputPath, const fs::path& assetsDir) {
    const auto fontData = Paths::ReadFile(assetsDir / "FontData");
    const auto font = fontData ? Otr::Texture::Read(*fontData) : std::nullopt;
    if (!font) {
        SPDLOG_ERROR("Missing or invalid {}", Paths::ToUtf8(assetsDir / "FontData"));
        return std::nullopt;
    }

    Image fontImage(16 * 4, 256, 4);
    for (int id = 0; id < 4; id++) {
        const auto tlutBytes = Paths::ReadFile(assetsDir / ("FontTLUT[" + std::to_string(id) + "].png"));
        const auto tlutImage = tlutBytes ? ImageIO::DecodePng(*tlutBytes) : std::nullopt;
        if (!tlutImage) {
            SPDLOG_ERROR("Missing font TLUT {}", id);
            return std::nullopt;
        }
        Otr::Texture tlut;
        tlut.type = Otr::TextureType::RGBA32bpp;
        Otr::TextureCodec::FromImage(tlut, *tlutImage);
        if (const auto glyphs = Otr::TextureCodec::ToImage(*font, &tlut)) {
            ImageOps::CompositeOver(fontImage, *glyphs, id * 16, 0);
        }
    }

    const auto png = ImageIO::EncodePng(fontImage);
    if (!Paths::WriteFile(outputPath / Paths::FromUtf8(std::string(kFontTextureName) + ".png"), png)) {
        return std::nullopt;
    }
    TextureManifestEntry entry;
    entry.hash = Sha256Hex(png);
    entry.textureType = font->type;
    entry.textureWidth = fontImage.width;
    entry.textureHeight = fontImage.height;
    return entry;
}

bool WriteManifest(const fs::path& path, const ExtractedTextures& entries) {
    nlohmann::ordered_json json = nlohmann::ordered_json::object();
    for (const auto& [name, entry] : entries) {
        json[name] = entry.ToJson();
    }
    return Paths::WriteText(path, json.dump(2));
}
} // namespace TextureReplacement
