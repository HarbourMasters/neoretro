#include "TextureBuilder.h"

#include "otr/Exporters.h"
#include "utils/Paths.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <spdlog/spdlog.h>

namespace TextureBuilder {
namespace {
std::optional<std::vector<char>> ProcessBands(const TextureGroup& group, const std::string& textureName) {
    const TextureManifestEntry& entry = group.front().second;
    if (!entry.strips) {
        return std::nullopt;
    }
    const auto& strips = *entry.strips;

    std::map<int, Image> supplied;
    for (const auto& [file, pairEntry] : group) {
        if (!pairEntry.stripIndex || *pairEntry.stripIndex >= (int)strips.size()) {
            continue;
        }
        const int index = *pairEntry.stripIndex;
        const auto bytes = Paths::ReadFile(file);
        auto image = bytes ? ImageIO::DecodePng(*bytes) : std::nullopt;
        if (!image) {
            SPDLOG_WARN("Failed to decode band {} of {}", index, textureName);
            continue;
        }
        if (image->hasPalette) {
            image = ImageOps::Convert(*image, 4);
        }
        if (supplied.count(index)) {
            SPDLOG_WARN("Band {} of {} is supplied more than once; using {}", index, textureName, Paths::ToUtf8(file));
        }
        supplied[index] = std::move(*image);
    }
    if (supplied.empty()) {
        return std::nullopt;
    }

    int scale = 1;
    std::set<int> bandScales;
    for (const auto& [index, image] : supplied) {
        const int cols = strips[index].cols.value_or(entry.textureWidth);
        const int bandScale = (int)std::lround((double)image.width / cols);
        bandScales.insert(std::max(bandScale, 1));
        scale = std::max(scale, bandScale);
    }
    if (bandScales.size() > 1) {
        SPDLOG_WARN("Bands of {} are not all the same scale; using {}x", textureName, scale);
    }
    if (entry.fitWidth) {
        scale = *entry.fitWidth / entry.textureWidth;
    }

    Image canvas(entry.textureWidth * scale, entry.textureHeight * scale, 4);
    for (size_t i = 0; i < strips.size(); i++) {
        const auto found = supplied.find((int)i);
        if (found == supplied.end()) {
            continue;
        }
        Image image = found->second;
        const TextureStrip& strip = strips[i];
        const int width = strip.cols.value_or(entry.textureWidth) * scale;
        if (image.width != width || image.height != strip.hashRows * scale) {
            image = ImageOps::Resize(image, width, strip.hashRows * scale, ImageOps::Interpolation::Cubic);
        }
        if (strip.hashRows != strip.rows) {
            image = ImageOps::Crop(image, 0, (strip.y - strip.hashY) * scale, width, strip.rows * scale);
        }
        ImageOps::CompositeDirect(canvas, image, strip.x * scale, strip.y * scale);
    }

    TextureManifestEntry composed;
    composed.hash = entry.hash;
    composed.textureType = entry.textureType;
    composed.textureWidth = entry.textureWidth;
    composed.textureHeight = entry.textureHeight;
    composed.tileWidth = entry.tileWidth;
    composed.tileHeight = entry.tileHeight;
    composed.targetName = entry.targetName;
    return BuildFromImage(canvas, composed, textureName);
}

std::optional<std::vector<char>> ProcessJpeg(const TexturePair& pair, const std::string& textureName) {
    const auto bytes = Paths::ReadFile(pair.first);
    const auto image = bytes ? ImageIO::DecodeJpeg(*bytes) : std::nullopt;
    if (!image) {
        SPDLOG_WARN("Failed to decode image data for JPEG: {}", textureName);
        return std::nullopt;
    }

    Otr::Texture texture;
    texture.type = Otr::TextureType::RGBA32bpp;
    texture.SetFlags(Otr::kTextureFlagLoadAsRaw);
    const double hByteScale = ((double)image->width / pair.second.textureWidth) *
                              (Otr::PixelMultiplier(texture.type) / Otr::PixelMultiplier(Otr::TextureType::RGBA16bpp));
    const double vPixelScale = (double)image->height / pair.second.textureHeight;
    texture.SetScale((float)hByteScale, (float)vPixelScale);
    if (!Otr::TextureCodec::FromImage(texture, *image)) {
        return std::nullopt;
    }
    return Otr::ExportTexture(texture, textureName);
}

std::optional<std::vector<char>> ProcessPng(const TexturePair& pair, const std::string& textureName) {
    const auto bytes = Paths::ReadFile(pair.first);
    auto image = bytes ? ImageIO::Decode(*bytes) : std::nullopt;
    if (!image) {
        SPDLOG_WARN("Failed to decode image data for PNG: {}", textureName);
        return std::nullopt;
    }

    const std::string path = Paths::ToPosix(pair.first);
    const std::string rgbSuffix = "_rgb.png";
    if (path.size() > rgbSuffix.size() &&
        path.compare(path.size() - rgbSuffix.size(), rgbSuffix.size(), rgbSuffix) == 0) {
        image = ImageOps::Convert(*image, 3);
        const fs::path alphaFile = Paths::FromUtf8(path.substr(0, path.size() - rgbSuffix.size()) + "_a.png");
        if (fs::exists(alphaFile)) {
            const auto alphaBytes = Paths::ReadFile(alphaFile);
            const auto alpha = alphaBytes ? ImageIO::DecodePng(*alphaBytes) : std::nullopt;
            if (alpha && alpha->width == image->width && alpha->height == image->height) {
                image = WithAlphaFrom(*image, *alpha);
            } else {
                SPDLOG_WARN("Ignoring {}: not the size of {}", Paths::ToUtf8(alphaFile), textureName);
            }
        }
    }

    return BuildFromImage(*image, pair.second, textureName);
}
} // namespace

BuiltTexture Process(const std::string& baseDir, const TextureGroup& group, bool prependAlt) {
    const TexturePair& pair = group.front();
    const auto& targetPath = pair.second.targetName;
    const std::string textureName =
        targetPath ? Paths::Basename(*targetPath) : Paths::BasenameWithoutExtension(Paths::ToUtf8(pair.first));
    const std::string fileName = targetPath ? *targetPath : Paths::Join(baseDir, textureName);

    BuiltTexture result;
    result.archivePath = Paths::Normalize(prependAlt ? Paths::Join("alt", fileName) : fileName);
    try {
        if (pair.second.stripIndex) {
            result.data = ProcessBands(group, textureName);
        } else if (pair.second.textureType == Otr::TextureType::JPEG32bpp) {
            result.data = ProcessJpeg(pair, textureName);
        } else {
            result.data = ProcessPng(pair, textureName);
        }
    } catch (const std::exception& e) { SPDLOG_ERROR("Failed to process {}: {}", textureName, e.what()); }
    return result;
}

std::optional<std::vector<char>> BuildFromImage(const Image& source, const TextureManifestEntry& entry,
                                                const std::string& textureName) {
    Otr::Texture texture;
    Image image = source;

    if (entry.kind == TextureEntryKind::AdditiveFontGlyph) {
        auto k = ExactMultiple(image, entry.textureWidth, entry.textureHeight);
        if (!k && entry.tileWidth && entry.tileHeight) {
            k = ExactMultiple(image, *entry.tileWidth, *entry.tileHeight);
        }
        if (!k) {
            SPDLOG_WARN("Skipping {}: {}x{} is not an integer multiple of its {}x{} mask chunk or of its drawn tile",
                        textureName, image.width, image.height, entry.textureWidth, entry.textureHeight);
            return std::nullopt;
        }
        const int alignedWidth = (entry.textureWidth + 3) & ~3;
        image = ImageOps::PadCanvas(image, *k * alignedWidth, *k * entry.textureHeight);
        texture.type = Otr::TextureType::RGBA32bpp;
        texture.SetFlags(Otr::kTextureFlagLoadAsRaw);
        texture.SetScale((float)*k, (float)*k);
        if (!Otr::TextureCodec::FromImage(texture, image)) {
            return std::nullopt;
        }
        return Otr::ExportTexture(texture, textureName);
    }

    if (entry.tileWidth && entry.tileHeight && !ExactMultiple(image, entry.textureWidth, entry.textureHeight)) {
        if (const auto k = ExactMultiple(image, *entry.tileWidth, *entry.tileHeight)) {
            image = ImageOps::PadCanvas(image, *k * entry.textureWidth, *k * entry.textureHeight);
        }
    }

    texture.type = entry.textureType;
    bool isPalette = image.hasPalette && FitsPaletteFormat(image, texture.type);

    const bool isNotOriginalSize = entry.textureWidth != image.width || entry.textureHeight != image.height;
    const bool isAdditive = entry.kind == TextureEntryKind::Additive;
    if (isAdditive) {
        isPalette = false;
    }
    const bool isCi = Otr::IsPalette(entry.textureType);
    const bool isIntensity =
        entry.textureType == Otr::TextureType::Grayscale4bpp || entry.textureType == Otr::TextureType::Grayscale8bpp;
    if (isIntensity && (image.channels % 2) == 1) {
        image = WithAlphaFrom(image, image);
    }
    const bool isRaw =
        isNotOriginalSize || isAdditive || (isCi && !isPalette) || !FitsIntensityFormat(image, entry.textureType);
    if (isRaw) {
        image = FitIntegerScale(image, entry, textureName);
        texture.SetFlags(Otr::kTextureFlagLoadAsRaw);
        if (!image.hasPalette || !isPalette) {
            texture.type = Otr::TextureType::RGBA32bpp;
        }
        const double hByteScale = ((double)image.width / entry.textureWidth) *
                                  (Otr::PixelMultiplier(texture.type) / Otr::PixelMultiplier(entry.textureType));
        const double vPixelScale = (double)image.height / entry.textureHeight;
        texture.SetScale((float)hByteScale, (float)vPixelScale);
    }

    if (!Otr::TextureCodec::FromImage(texture, image)) {
        return std::nullopt;
    }

    if (isCi) {
        if (isPalette) {
            texture.type = entry.textureType;
        }
    } else if (!isRaw) {
        texture.type = entry.textureType;
    }

    return Otr::ExportTexture(texture, textureName);
}

std::optional<std::pair<int, int>> PackedSize(const TextureGroup& group) {
    const TextureManifestEntry& entry = group.front().second;
    if (group.size() == 1) {
        const auto bytes = Paths::ReadFile(group.front().first);
        const auto size = bytes ? ImageIO::PngSize(*bytes) : std::nullopt;
        if (!size) {
            return std::nullopt;
        }
        return WholeMultiple(size->first, size->second, entry);
    }
    int scale = 1;
    for (const auto& [file, pairEntry] : group) {
        if (!entry.strips || !pairEntry.stripIndex || *pairEntry.stripIndex >= (int)entry.strips->size()) {
            continue;
        }
        const int cols = (*entry.strips)[*pairEntry.stripIndex].cols.value_or(entry.textureWidth);
        const auto bytes = Paths::ReadFile(file);
        if (const auto size = bytes ? ImageIO::PngSize(*bytes) : std::nullopt) {
            scale = std::max(scale, (int)std::lround((double)size->first / cols));
        }
    }
    return std::make_pair(entry.textureWidth * scale, entry.textureHeight * scale);
}

std::pair<int, int> WholeMultiple(int width, int height, const TextureManifestEntry& entry) {
    const int kx = std::max(1, (int)std::lround((double)width / entry.textureWidth));
    const int ky = std::max(1, (int)std::lround((double)height / entry.textureHeight));
    return { kx * entry.textureWidth, ky * entry.textureHeight };
}

Image WithAlphaFrom(const Image& image, const Image& alpha) {
    Image out = ImageOps::Convert(image, 4);
    for (int y = 0; y < out.height; y++) {
        for (int x = 0; x < out.width; x++) {
            out.Pixel(x, y)[3] = ImageOps::Luminance(alpha.Pixel(x, y));
        }
    }
    return out;
}

bool FitsPaletteFormat(const Image& image, Otr::TextureType type) {
    if (!Otr::IsPalette(type)) {
        return false;
    }
    const int limit = type == Otr::TextureType::Palette4bpp ? 16 : 256;
    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            if (image.Index(x, y) >= limit) {
                return false;
            }
        }
    }
    return true;
}

bool FitsIntensityFormat(const Image& image, Otr::TextureType type) {
    const bool intensity = type == Otr::TextureType::Grayscale4bpp || type == Otr::TextureType::Grayscale8bpp;
    const bool intensityAlpha = type == Otr::TextureType::GrayscaleAlpha4bpp ||
                                type == Otr::TextureType::GrayscaleAlpha8bpp ||
                                type == Otr::TextureType::GrayscaleAlpha16bpp;
    if (!intensity && !intensityAlpha) {
        return true;
    }
    const Image rgba = ImageOps::Convert(image, 4);
    for (size_t i = 0; i < rgba.rgba.size(); i += 4) {
        const uint8_t* p = &rgba.rgba[i];
        if (p[0] != p[1] || p[0] != p[2] || (intensity && p[3] != p[0])) {
            return false;
        }
    }
    return true;
}

Image FitIntegerScale(const Image& image, const TextureManifestEntry& entry, const std::string& textureName) {
    if (entry.textureWidth <= 0 || entry.textureHeight <= 0) {
        return image;
    }
    const auto multiple = WholeMultiple(image.width, image.height, entry);
    const int width = entry.fitWidth.value_or(multiple.first);
    const int height = entry.fitHeight.value_or(multiple.second);
    if (image.width == width && image.height == height) {
        return image;
    }
    SPDLOG_INFO("Resampling {} from {}x{} to {}x{}, a whole multiple of {}x{}", textureName, image.width, image.height,
                width, height, entry.textureWidth, entry.textureHeight);
    return ImageOps::Resize(image, width, height,
                            image.hasPalette ? ImageOps::Interpolation::Nearest : ImageOps::Interpolation::Cubic);
}

std::optional<int> ExactMultiple(const Image& image, int width, int height) {
    if (width <= 0 || height <= 0) {
        return std::nullopt;
    }
    if (image.width % width != 0 || image.height % height != 0) {
        return std::nullopt;
    }
    const int k = image.width / width;
    if (k > 0 && image.height / height == k) {
        return k;
    }
    return std::nullopt;
}
} // namespace TextureBuilder
