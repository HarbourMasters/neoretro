#include "Pm64Images.h"
#include "Pm64SpriteNames.h"

#include "otr/Archive.h"
#include "otr/Resource.h"
#include "utils/ThreadPool.h"

#include <algorithm>
#include <cstring>
#include <map>
#include <regex>
#include <set>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace Pm64 {
namespace {
bool StartsWith(std::string_view text, std::string_view prefix) {
    return text.size() >= prefix.size() && text.compare(0, prefix.size(), prefix) == 0;
}

bool EndsWith(std::string_view text, std::string_view suffix) {
    return text.size() >= suffix.size() && text.compare(text.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::optional<Otr::Texture> ReadTexture(Otr::ArchiveReader& archive, const std::string& name) {
    const auto data = archive.Read(name);
    return data ? Otr::Texture::Read(*data) : std::nullopt;
}

std::optional<std::vector<uint8_t>> ReadBlob(Otr::ArchiveReader& archive, const std::string& name) {
    const auto data = archive.Read(name);
    const auto header = data ? Otr::ReadHeader(*data) : std::nullopt;
    if (!header || header->type != (uint32_t)Otr::ResourceType::Blob) {
        return std::nullopt;
    }
    try {
        Otr::ResourceReader reader(*data, header->bigEndian);
        return reader.ReadBytes(reader.ReadU32());
    } catch (const std::out_of_range&) { return std::nullopt; }
}

uint32_t LE32(const std::vector<uint8_t>& data, size_t at) {
    return at + 4 > data.size() ? 0xFFFFFFFF
                                : (uint32_t)data[at] | (uint32_t)data[at + 1] << 8 | (uint32_t)data[at + 2] << 16 |
                                      (uint32_t)data[at + 3] << 24;
}

std::vector<int> SpriteRasterPalettes(const std::vector<uint8_t>& blob) {
    std::vector<int> palettes;
    const uint32_t rasters = LE32(blob, 0);
    for (size_t at = rasters; at + 4 <= blob.size(); at += 4) {
        const uint32_t entry = LE32(blob, at);
        if (entry == 0xFFFFFFFF || entry + 8 > blob.size()) {
            break;
        }
        palettes.push_back((int8_t)blob[entry + 6]);
    }
    return palettes;
}

struct Candidate {
    std::string path;
    std::string tlutPath;
    bool sharedPalette = false;
    std::vector<std::string> mipPaths;
    std::string group;
    std::string name;
    int order = 0;
};

std::string Basename(const std::string& path) {
    return path.substr(path.rfind('/') + 1);
}

std::string Folder(const std::string& path) {
    const auto first = path.find('/');
    const auto last = path.rfind('/');
    return first == last ? path.substr(0, first) : path.substr(first + 1, last - first - 1);
}

const SpriteSheetName* SheetName(bool player, int index) {
    const SpriteSheetName* table = player ? kPlayerSpriteNames : kNpcSpriteNames;
    const int count = player ? kPlayerSpriteNameCount : kNpcSpriteNameCount;
    return index >= 0 && index < count && table[index].group != nullptr ? &table[index] : nullptr;
}

void NameSprite(Candidate& candidate, bool player, int index, int raster) {
    const auto* sheet = SheetName(player, index);
    const std::string fallback = fmt::format("{}_sprite_{:03}", player ? "player" : "npc", index);
    const char* rasterName = sheet && raster < sheet->rasterCount ? sheet->rasters[raster] : nullptr;
    candidate.group = sheet ? sheet->group : fallback;
    candidate.name = fmt::format("{} / {}", sheet ? sheet->sheet : fallback,
                                 rasterName ? std::string(rasterName) : fmt::format("Raster {}", raster));
    candidate.order = ((player ? 0 : 1) << 24) | index << 12 | raster;
}

std::vector<Candidate> SpriteCandidates(Otr::ArchiveReader& archive, const std::set<std::string>& names) {
    static const std::regex kRaster(R"(sprites/((npc|player)_sprite_\d+)_raster_(\d+))");
    std::map<std::string, std::vector<int>> palettes;
    std::vector<Candidate> candidates;
    for (const auto& name : names) {
        std::smatch match;
        if (!std::regex_match(name, match, kRaster)) {
            continue;
        }
        const std::string base = "sprites/" + match[1].str();
        auto it = palettes.find(base);
        if (it == palettes.end()) {
            const auto blob = ReadBlob(archive, base);
            it = palettes.emplace(base, blob ? SpriteRasterPalettes(*blob) : std::vector<int>{}).first;
        }
        const size_t raster = std::stoul(match[3].str());
        const int palette = raster < it->second.size() ? std::max(it->second[raster], 0) : 0;
        std::string tlut = fmt::format("{}_pal_{}", base, palette);
        if (!names.count(tlut)) {
            tlut = base + "_pal_0";
        }
        Candidate candidate{ name, names.count(tlut) ? tlut : "", true };
        NameSprite(candidate, match[2] == "player", std::stoi(match[1].str().substr(match[1].str().rfind('_') + 1)),
                   (int)raster);
        candidates.push_back(std::move(candidate));
    }
    return candidates;
}

std::string AreaName(const std::string& folder) {
    static const std::map<std::string, std::string> kAreas = {
        { "kmr", "Goomba Village" },
        { "mac", "Toad Town" },
        { "tik", "Toad Town Tunnels" },
        { "kgr", "Inside the Whale" },
        { "kkj", "Peach's Castle" },
        { "hos", "Shooting Star Summit" },
        { "nok", "Koopa Village" },
        { "trd", "Koopa Bros. Fortress" },
        { "iwa", "Mt. Rugged" },
        { "dro", "Dry Dry Outpost" },
        { "sbk", "Dry Dry Desert" },
        { "isk", "Dry Dry Ruins" },
        { "mim", "Forever Forest" },
        { "obk", "Boo's Mansion" },
        { "arn", "Gusty Gulch" },
        { "dgb", "Tubba Blubba's Castle" },
        { "omo", "Shy Guy's Toy Box" },
        { "jan", "Jade Jungle" },
        { "kzn", "Mt. Lavalava" },
        { "flo", "Flower Fields" },
        { "sam", "Shiver City" },
        { "pra", "Crystal Palace" },
        { "kpa", "Bowser's Castle" },
        { "osr", "Outside Peach's Castle" },
        { "end", "Ending" },
        { "mgm", "Minigames" },
        { "gv_", "Game Over" },
        { "tst", "Test Map" },
    };
    const auto it = kAreas.find(folder.substr(0, 3));
    return it == kAreas.end() ? folder : it->second;
}

std::vector<Candidate> MapTextureCandidates(const std::set<std::string>& names) {
    static const std::regex kMip(R"(.*_mm\d+)");
    std::vector<Candidate> candidates;
    for (const auto& name : names) {
        if (!StartsWith(name, "textures/") || EndsWith(name, "_tlut") || std::regex_match(name, kMip)) {
            continue;
        }
        Candidate candidate{ name, names.count(name + "_tlut") ? name + "_tlut" : "" };
        for (int level = 1; names.count(fmt::format("{}_mm{}", name, level)); level++) {
            candidate.mipPaths.push_back(fmt::format("{}_mm{}", name, level));
        }
        const bool sharesWithAux = names.count(name + "_aux") && !names.count(name + "_aux_tlut");
        const bool sharedAux = EndsWith(name, "_aux") && candidate.tlutPath.empty();
        if (sharedAux) {
            const std::string main = name.substr(0, name.size() - 4) + "_tlut";
            candidate.tlutPath = names.count(main) ? main : "";
        }
        candidate.sharedPalette = !candidate.mipPaths.empty() || sharesWithAux || sharedAux;
        candidate.group = AreaName(Folder(name));
        candidate.name = Basename(name);
        candidates.push_back(std::move(candidate));
    }
    return candidates;
}

std::string PictureGroup(const std::string& path) {
    static const std::map<std::string, std::string> kGroups = {
        { "backgrounds", "Backgrounds" },
        { "title_screen", "Title Screen" },
        { "story_images", "Intro Story" },
        { "charset", "Letters" },
    };
    const auto it = kGroups.find(Folder(path));
    return it == kGroups.end() ? Folder(path) : it->second;
}

std::vector<Candidate> PictureCandidates(const std::set<std::string>& names) {
    std::vector<Candidate> candidates;
    for (const auto& name : names) {
        if (StartsWith(name, "backgrounds/") && EndsWith(name, "_bg")) {
            candidates.push_back({ name,
                                   names.count(name + "_pal0") ? name + "_pal0" : "",
                                   false,
                                   {},
                                   PictureGroup(name),
                                   Basename(name) });
        } else if ((StartsWith(name, "title_screen/") || StartsWith(name, "story_images/") ||
                    StartsWith(name, "charset/")) &&
                   EndsWith(name, "_img")) {
            const std::string stem = name.substr(0, name.size() - 4);
            std::string tlut;
            for (const auto& option : { name + "_tlut", stem + "_tlut" }) {
                if (names.count(option)) {
                    tlut = option;
                    break;
                }
            }
            candidates.push_back({ name, tlut, false, {}, PictureGroup(name), Basename(name) });
        }
    }
    return candidates;
}

int PaletteLimit(Otr::TextureType type) {
    return type == Otr::TextureType::Palette4bpp ? 16 : 256;
}

Image ExactPalette(const Image& image) {
    std::vector<uint8_t> palette;
    const auto has = [&](const uint8_t* color) {
        for (size_t i = 0; i < palette.size(); i += 4) {
            if (std::memcmp(&palette[i], color, 4) == 0) {
                return true;
            }
        }
        return false;
    };
    for (size_t i = 0; i < (size_t)image.width * image.height; i++) {
        const uint8_t* color = &image.rgba[i * 4];
        if (!has(color)) {
            palette.insert(palette.end(), color, color + 4);
        }
    }
    return ImageOps::MapToPalette(image, palette.data(), (int)palette.size() / 4);
}
} // namespace

std::vector<ImageAsset> LoadImages(Otr::ArchiveReader& archive, ImageKind kind) {
    const auto list = archive.List();
    const std::set<std::string> names(list.begin(), list.end());
    std::vector<Candidate> candidates;
    switch (kind) {
        case ImageKind::Sprites:
            candidates = SpriteCandidates(archive, names);
            break;
        case ImageKind::MapTextures:
            candidates = MapTextureCandidates(names);
            break;
        case ImageKind::Pictures:
            candidates = PictureCandidates(names);
            break;
    }

    std::vector<std::optional<ImageAsset>> loaded(candidates.size());
    ThreadPool::Instance().ParallelFor(candidates.size(), [&](size_t i) {
        const auto& candidate = candidates[i];
        auto texture = ReadTexture(archive, candidate.path);
        if (!texture) {
            return;
        }
        ImageAsset asset;
        asset.path = candidate.path;
        asset.group = candidate.group;
        asset.name = candidate.name;
        asset.order = candidate.order;
        asset.texture = std::move(*texture);
        if (Otr::IsPalette(asset.texture.type) && !candidate.tlutPath.empty()) {
            asset.tlutPath = candidate.tlutPath;
            asset.tlut = ReadTexture(archive, candidate.tlutPath);
        }
        asset.sharedPalette = candidate.sharedPalette;
        for (const auto& mipPath : candidate.mipPaths) {
            if (auto mip = ReadTexture(archive, mipPath)) {
                asset.mips.emplace_back(mipPath, std::move(*mip));
            }
        }
        loaded[i] = std::move(asset);
    });

    std::vector<ImageAsset> assets;
    for (auto& asset : loaded) {
        if (asset) {
            assets.push_back(std::move(*asset));
        }
    }
    std::sort(assets.begin(), assets.end(), [](const ImageAsset& a, const ImageAsset& b) {
        if ((a.order >> 24) != (b.order >> 24)) {
            return (a.order >> 24) < (b.order >> 24);
        }
        if (a.group != b.group) {
            return a.group < b.group;
        }
        return a.order != b.order ? a.order < b.order : a.path < b.path;
    });
    SPDLOG_INFO("Loaded {} Paper Mario images", assets.size());
    return assets;
}

std::optional<Image> DecodeImage(const ImageAsset& asset) {
    return Otr::TextureCodec::ToImage(asset.texture, asset.tlut ? &*asset.tlut : nullptr);
}

std::optional<EncodedImage> EncodeImage(const ImageAsset& asset, const Image& image, std::string* error) {
    const auto fail = [&](std::string message) -> std::optional<EncodedImage> {
        if (error) {
            *error = std::move(message);
        }
        return std::nullopt;
    };
    if (image.width != asset.texture.width || image.height != asset.texture.height) {
        return fail(fmt::format("The image must be {}x{}", asset.texture.width, asset.texture.height));
    }

    EncodedImage encoded;
    Otr::Texture texture = asset.texture;
    if (!Otr::IsPalette(texture.type)) {
        if (!Otr::TextureCodec::FromImage(texture, image)) {
            return fail("Could not encode the image");
        }
        encoded.textures.emplace_back(asset.path, texture);
        encoded.preview = *Otr::TextureCodec::ToImage(texture);
        return encoded;
    }
    if (!asset.tlut) {
        return fail("The image has no palette");
    }

    const int limit = PaletteLimit(texture.type);
    Otr::Texture tlut = *asset.tlut;
    Image indexed;
    if (asset.sharedPalette) {
        const auto palette = Otr::TextureCodec::ToImage(tlut);
        if (!palette) {
            return fail("The palette could not be read");
        }
        indexed =
            ImageOps::MapToPalette(image, palette->rgba.data(), std::min(palette->width * palette->height, limit));
        encoded.mappedToPalette = indexed.rgba != image.rgba;
    } else {
        if (ImageOps::CountColors(image, limit) <= limit) {
            indexed = ExactPalette(image);
        } else {
            indexed = ImageOps::Quantize(image, limit);
            encoded.reducedColors = true;
        }
        Image palette(tlut.width, tlut.height, 4);
        std::memcpy(palette.rgba.data(), indexed.palette.data(),
                    std::min(palette.rgba.size(), (size_t)indexed.paletteSize * 4));
        if (!Otr::TextureCodec::FromImage(tlut, palette)) {
            return fail("Could not encode the palette");
        }
        encoded.textures.emplace_back(asset.tlutPath, tlut);
    }
    if (!Otr::TextureCodec::FromImage(texture, indexed)) {
        return fail("Could not encode the image");
    }
    encoded.textures.emplace(encoded.textures.begin(), asset.path, texture);

    const auto palette = Otr::TextureCodec::ToImage(tlut);
    for (const auto& [mipPath, original] : asset.mips) {
        Otr::Texture mip = original;
        Image scaled = ImageOps::Resize(image, mip.width, mip.height, ImageOps::Interpolation::Cubic);
        if (Otr::IsPalette(mip.type) && palette) {
            scaled = ImageOps::MapToPalette(scaled, palette->rgba.data(),
                                            std::min(palette->width * palette->height, PaletteLimit(mip.type)));
        }
        if (!Otr::TextureCodec::FromImage(mip, scaled)) {
            return fail("Could not encode a mipmap level");
        }
        encoded.textures.emplace_back(mipPath, mip);
    }
    encoded.preview = *Otr::TextureCodec::ToImage(texture, &tlut);
    return encoded;
}

std::string DescribeImage(const ImageAsset& asset) {
    std::string text =
        fmt::format("{} {}x{}", Otr::TextureTypeName(asset.texture.type), asset.texture.width, asset.texture.height);
    if (!asset.tlutPath.empty()) {
        text += fmt::format("  palette {}", asset.tlutPath.substr(asset.tlutPath.rfind('/') + 1));
    }
    if (!asset.mips.empty()) {
        text += fmt::format("  {} mipmaps", asset.mips.size());
    }
    return text;
}
} // namespace Pm64
