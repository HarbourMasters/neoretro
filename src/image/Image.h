#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <vector>

struct Image {
    int width = 0;
    int height = 0;
    int channels = 4;
    std::vector<uint8_t> rgba;

    bool hasPalette = false;
    std::vector<uint8_t> indices;
    std::array<uint8_t, 256 * 4> palette = {};
    int paletteSize = 0;

    Image() = default;
    Image(int w, int h, int numChannels = 4);

    bool Empty() const {
        return width <= 0 || height <= 0;
    }
    uint8_t* Pixel(int x, int y) {
        return &rgba[((size_t)y * width + x) * 4];
    }
    const uint8_t* Pixel(int x, int y) const {
        return &rgba[((size_t)y * width + x) * 4];
    }
    uint8_t Index(int x, int y) const;

    void SetPaletteColor(int index, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void ResolvePalette();
};

namespace ImageIO {
std::optional<Image> DecodePng(const std::vector<uint8_t>& bytes);
std::optional<Image> DecodeJpeg(const std::vector<uint8_t>& bytes);
std::optional<Image> Decode(const std::vector<uint8_t>& bytes);
std::optional<std::pair<int, int>> PngSize(const std::vector<uint8_t>& bytes);
std::optional<std::pair<int, int>> JpegSize(const std::vector<uint8_t>& bytes);
std::vector<uint8_t> EncodePng(const Image& image);
} // namespace ImageIO

namespace ImageOps {
enum class Interpolation { Nearest, Cubic };

Image Convert(const Image& image, int numChannels);
Image Resize(const Image& image, int width, int height, Interpolation interpolation);
Image Crop(const Image& image, int x, int y, int width, int height);
void CompositeDirect(Image& dst, const Image& src, int dstX, int dstY);
void CompositeOver(Image& dst, const Image& src, int dstX, int dstY);
Image PadCanvas(const Image& image, int width, int height);
uint8_t Luminance(const uint8_t* rgba);
} // namespace ImageOps
