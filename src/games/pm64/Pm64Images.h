#pragma once

#include "image/Image.h"
#include "otr/Texture.h"

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace Otr {
class ArchiveReader;
}

namespace Pm64 {
enum class ImageKind { Sprites, MapTextures, Pictures };

struct ImageAsset {
    std::string path;
    std::string group;
    std::string name;
    int order = 0;
    Otr::Texture texture;
    std::string tlutPath;
    std::optional<Otr::Texture> tlut;
    bool sharedPalette = false;
    std::vector<std::pair<std::string, Otr::Texture>> mips;
};

struct EncodedImage {
    std::vector<std::pair<std::string, Otr::Texture>> textures;
    Image preview;
    bool reducedColors = false;
    bool mappedToPalette = false;
};

std::vector<ImageAsset> LoadImages(Otr::ArchiveReader& archive, ImageKind kind);

std::optional<Image> DecodeImage(const ImageAsset& asset);

std::optional<EncodedImage> EncodeImage(const ImageAsset& asset, const Image& image, std::string* error = nullptr);

std::string DescribeImage(const ImageAsset& asset);
} // namespace Pm64
