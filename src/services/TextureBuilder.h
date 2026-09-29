#pragma once

#include "image/Image.h"
#include "models/StageEntry.h"

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace TextureBuilder {
using TextureGroup = std::vector<TexturePair>;

struct BuiltTexture {
    std::string archivePath;
    std::optional<std::vector<char>> data;
};

BuiltTexture Process(const std::string& baseDir, const TextureGroup& group, bool prependAlt);

std::optional<std::pair<int, int>> PackedSize(const TextureGroup& group);
std::pair<int, int> WholeMultiple(int width, int height, const TextureManifestEntry& entry);

std::optional<std::vector<char>> BuildFromImage(const Image& source, const TextureManifestEntry& entry,
                                                const std::string& textureName);

Image WithAlphaFrom(const Image& image, const Image& alpha);
bool FitsPaletteFormat(const Image& image, Otr::TextureType type);
bool FitsIntensityFormat(const Image& image, Otr::TextureType type);
Image FitIntegerScale(const Image& image, const TextureManifestEntry& entry, const std::string& textureName);
std::optional<int> ExactMultiple(const Image& image, int width, int height);
} // namespace TextureBuilder
