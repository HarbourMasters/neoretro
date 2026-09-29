#include "TextureReplacement.h"

#include "games/GameTextureConventions.h"
#include "image/Image.h"
#include "otr/Archive.h"
#include "otr/Resource.h"
#include "otr/Texture.h"
#include "utils/Hash.h"
#include "utils/Paths.h"

#include <algorithm>
#include <spdlog/spdlog.h>

namespace TextureReplacement {
namespace {
std::string ToLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return value;
}

void ProcessFile(const std::string& fileName, const std::vector<uint8_t>& data, const fs::path& outputPath,
                 ExtractedTextures& processed) {
    const auto header = Otr::ReadHeader(data);
    if (!header) {
        return;
    }

    if (header->type == (uint32_t)Otr::ResourceType::Texture) {
        const auto texture = Otr::Texture::Read(data);
        const auto image = texture ? Otr::TextureCodec::ToImage(*texture) : std::nullopt;
        if (!image) {
            SPDLOG_WARN("Could not decode texture {}", fileName);
            return;
        }
        const auto png = ImageIO::EncodePng(*image);
        if (!Paths::WriteFile(fs::path(outputPath) += ".png", png)) {
            return;
        }
        TextureManifestEntry entry;
        entry.hash = Sha256Hex(png);
        entry.textureType = texture->type;
        entry.textureWidth = texture->width;
        entry.textureHeight = texture->height;
        processed[fileName] = entry;
    } else if (header->type == (uint32_t)Otr::ResourceType::OoTBackground) {
        const auto background = Otr::Background::Read(data);
        const auto size = background ? ImageIO::JpegSize(background->data) : std::nullopt;
        if (!size) {
            SPDLOG_WARN("Could not decode background {}", fileName);
            return;
        }
        SPDLOG_INFO("Found JPEG background: {}!", fileName);
        if (!Paths::WriteFile(fs::path(outputPath) += ".jpg", background->data)) {
            return;
        }
        TextureManifestEntry entry;
        entry.hash = Sha256Hex(background->data);
        entry.textureType = Otr::TextureType::JPEG32bpp;
        entry.textureWidth = size->first;
        entry.textureHeight = size->second;
        processed[fileName] = entry;
    }
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

    std::map<std::string, std::vector<std::string>> aliases;
    if (const auto aliasText = Paths::ReadText(folder / "aliases.json")) {
        try {
            for (const auto& [source, value] : nlohmann::json::parse(*aliasText).items()) {
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
                aliases[Paths::Normalize(source)] = std::move(targets);
            }
            SPDLOG_INFO("Loaded aliases.json with {} source images", aliases.size());
        } catch (const std::exception& e) { SPDLOG_ERROR("Invalid aliases.json: {}", e.what()); }
    }

    ProcessedFolder processed;
    for (const auto& texFile : Paths::ListFilesRecursive(folder)) {
        const std::string ext = ToLower(texFile.extension().string());
        if (ext != ".png" && ext != ".jpeg" && ext != ".jpg") {
            continue;
        }
        const std::string relativePath = Paths::Relative(texFile, folder);
        const std::string texPathRelativeToFolder = Paths::WithoutExtension(relativePath);

        std::vector<std::string> targets = { texPathRelativeToFolder };
        if (auto it = aliases.find(relativePath); it != aliases.end()) {
            targets = it->second;
        } else if (auto base = aliases.find(Paths::Basename(relativePath)); base != aliases.end()) {
            targets = base->second;
        }

        for (const auto& target : targets) {
            std::string textureName = target;
            std::optional<int> stripIndex;
            auto manifestEntry = ResolveTextureEntry(manifest, target);

            const auto bandIndex = manifestEntry ? std::nullopt : BandIndexOf(target);
            if (bandIndex) {
                const std::string base = target.substr(0, target.find_last_of('#'));
                const auto baseEntry = ResolveTextureEntry(manifest, base);
                if (baseEntry && !baseEntry->strips) {
                    SPDLOG_WARN("Skipping {}: {} declares no bands", target, base);
                    continue;
                }
                if (baseEntry && baseEntry->strips && *bandIndex >= (int)baseEntry->strips->size()) {
                    SPDLOG_WARN("Skipping {}: {} has only {} bands", target, base, baseEntry->strips->size());
                    continue;
                }
                if (baseEntry && baseEntry->strips) {
                    manifestEntry = baseEntry;
                    textureName = base;
                    stripIndex = bandIndex;
                }
            }

            if (!manifestEntry) {
                SPDLOG_DEBUG("Found file not present in manifest: {}", target);
                continue;
            }

            if (stripIndex) {
                manifestEntry->stripIndex = stripIndex;
                SPDLOG_INFO("Staging band {} of {}", *stripIndex, textureName);
            } else if (manifestEntry->kind == TextureEntryKind::Replacement) {
                const auto bytes = Paths::ReadFile(texFile);
                if (bytes && manifestEntry->hash == Sha256Hex(*bytes)) {
                    continue;
                }
                SPDLOG_INFO("Found file with changed hash: {}", textureName);
            } else {
                SPDLOG_INFO("Staging additive texture: {}", textureName);
            }

            manifestEntry->targetName = textureName;
            processed[Paths::Dirname(textureName)].emplace_back(texFile, *manifestEntry);
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
        archive->ForEach([&](const std::string& fileName, const std::vector<uint8_t>& data) {
            ProcessFile(fileName, data, dir / Paths::FromUtf8(fileName), processed);
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
