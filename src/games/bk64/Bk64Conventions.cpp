#include "Bk64Conventions.h"

#include "otr/Archive.h"

#include <regex>
#include <spdlog/spdlog.h>
#include <tuple>

namespace {
const std::regex kColorVariantPattern(R"(^(.+_\d+_\d+)_(BLUE|GREEN|ORANGE|PURPLE|YELLOW)$)");
const std::regex kBoldFontPattern(R"(^(.*)boldfont/(.+_\d+_\d+)_[0-9A-Fa-f]+$)");
const std::regex kLangScopedPattern(R"(^(.*)lang/[^/]+/(.+)$)");
const std::regex kAnimFramePattern(R"(^(.+_tex_\d+)_frame_\d+$)");
const std::regex kSpriteChunkPattern(R"(^(.+)_(\d+)_(\d+)$)");

std::optional<TextureManifestEntry> Lookup(const nlohmann::json& manifest, const std::string& key) {
    if (!manifest.contains(key)) {
        return std::nullopt;
    }
    return TextureManifestEntry::FromJson(manifest[key]);
}

TextureManifestEntry AdditiveFrom(const TextureManifestEntry& tmpl, TextureEntryKind kind) {
    TextureManifestEntry entry;
    entry.textureType = tmpl.textureType;
    entry.textureWidth = tmpl.textureWidth;
    entry.textureHeight = tmpl.textureHeight;
    entry.tileWidth = tmpl.tileWidth;
    entry.tileHeight = tmpl.tileHeight;
    entry.kind = kind;
    return entry;
}

uint16_t ReadU16LE(const std::vector<uint8_t>& data, size_t offset) {
    return (uint16_t)(data[offset] | data[offset + 1] << 8);
}

uint32_t ReadU32LE(const std::vector<uint8_t>& data, size_t offset) {
    return (uint32_t)data[offset] | (uint32_t)data[offset + 1] << 8 | (uint32_t)data[offset + 2] << 16 |
           (uint32_t)data[offset + 3] << 24;
}
} // namespace

std::optional<TextureManifestEntry> Bk64TextureConventions::ResolveAdditiveEntry(const nlohmann::json& manifest,
                                                                                 const std::string& target) const {
    std::smatch match;
    if (std::regex_match(target, match, kColorVariantPattern)) {
        if (auto tmpl = Lookup(manifest, match[1].str())) {
            return AdditiveFrom(*tmpl, TextureEntryKind::Additive);
        }
    }
    if (std::regex_match(target, match, kBoldFontPattern)) {
        if (auto tmpl = Lookup(manifest, match[1].str() + "sprite/" + match[2].str())) {
            return AdditiveFrom(*tmpl, TextureEntryKind::AdditiveFontGlyph);
        }
    }
    if (std::regex_match(target, match, kLangScopedPattern)) {
        if (auto tmpl = Lookup(manifest, match[1].str() + match[2].str())) {
            return AdditiveFrom(*tmpl, TextureEntryKind::Additive);
        }
    }
    if (std::regex_match(target, match, kAnimFramePattern)) {
        if (auto tmpl = Lookup(manifest, match[1].str())) {
            return AdditiveFrom(*tmpl, TextureEntryKind::Additive);
        }
    }
    return std::nullopt;
}

void Bk64TextureConventions::RecordExtractionMetadata(const std::vector<std::string>& otrPaths,
                                                      std::map<std::string, TextureManifestEntry>& entries) const {
    std::map<std::string, std::vector<std::tuple<std::string, int, int>>> wanted;
    for (const auto& [key, entry] : entries) {
        std::smatch match;
        if (!std::regex_match(key, match, kSpriteChunkPattern)) {
            continue;
        }
        wanted[match[1].str()].emplace_back(key, std::stoi(match[2].str()), std::stoi(match[3].str()));
    }
    if (wanted.empty()) {
        return;
    }

    for (const auto& otrPath : otrPaths) {
        auto archive = Otr::ArchiveReader::Open(otrPath);
        if (!archive) {
            continue;
        }
        for (const auto& fileName : archive->List()) {
            const auto chunks = wanted.find(fileName);
            if (chunks == wanted.end()) {
                continue;
            }
            const auto data = archive->Read(fileName);
            if (!data) {
                continue;
            }
            const auto tiles = ParseSpriteTilePositions(*data);
            if (!tiles) {
                continue;
            }
            for (const auto& [key, frame, chunk] : chunks->second) {
                const auto tile = tiles->find({ frame, chunk });
                const auto entry = entries.find(key);
                if (tile == tiles->end() || entry == entries.end()) {
                    continue;
                }
                const auto [tw, th] = tile->second;
                auto& e = entry->second;
                const bool valid = tw > 0 && th > 0 && tw <= e.textureWidth && th <= e.textureHeight &&
                                   (tw != e.textureWidth || th != e.textureHeight);
                if (valid) {
                    e.tileWidth = tw;
                    e.tileHeight = th;
                }
            }
        }
    }
}

std::optional<std::map<std::pair<int, int>, std::pair<int, int>>>
ParseSpriteTilePositions(const std::vector<uint8_t>& data) {
    size_t offset = 0x40 + 10 + 4;
    if (data.size() < offset + 4) {
        return std::nullopt;
    }
    const uint32_t positionCount = ReadU32LE(data, offset);
    offset += 4;
    if (positionCount == 0 || positionCount > 0x4000 || data.size() < offset + positionCount * 4 + 4) {
        return std::nullopt;
    }
    std::vector<std::pair<int, int>> positions;
    for (uint32_t i = 0; i < positionCount; i++) {
        positions.emplace_back((int16_t)ReadU16LE(data, offset), (int16_t)ReadU16LE(data, offset + 2));
        offset += 4;
    }
    const uint32_t frameCount = ReadU32LE(data, offset);
    offset += 4;
    if (frameCount == 0 || frameCount > 0x1000 || data.size() < offset + frameCount * 2) {
        return std::nullopt;
    }
    std::map<std::pair<int, int>, std::pair<int, int>> tiles;
    size_t index = 0;
    for (uint32_t frame = 0; frame < frameCount; frame++) {
        const uint16_t count = ReadU16LE(data, offset + frame * 2);
        for (uint16_t chunk = 0; chunk < count; chunk++) {
            if (index < positions.size()) {
                tiles[{ (int)frame, (int)chunk }] = positions[index];
            }
            index++;
        }
    }
    return tiles;
}

const std::vector<std::unique_ptr<GameTextureConventions>>& AllGameTextureConventions() {
    static const auto sConventions = [] {
        std::vector<std::unique_ptr<GameTextureConventions>> conventions;
        conventions.push_back(std::make_unique<Bk64TextureConventions>());
        return conventions;
    }();
    return sConventions;
}
