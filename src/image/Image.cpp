#include "Image.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <lodepng.h>
#include <n64graphics/stb_image.h>
#include <spdlog/spdlog.h>

#define STB_IMAGE_RESIZE_STATIC
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize2.h>

Image::Image(int w, int h, int numChannels) : width(w), height(h), channels(numChannels) {
    rgba.assign((size_t)w * h * 4, 0);
}

uint8_t Image::Index(int x, int y) const {
    if (hasPalette) {
        return indices[(size_t)y * width + x];
    }
    return Pixel(x, y)[0];
}

void Image::SetPaletteColor(int index, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (index < 0 || index >= 256) {
        return;
    }
    palette[index * 4 + 0] = r;
    palette[index * 4 + 1] = g;
    palette[index * 4 + 2] = b;
    palette[index * 4 + 3] = a;
    paletteSize = std::max(paletteSize, index + 1);
}

void Image::ResolvePalette() {
    if (!hasPalette) {
        return;
    }
    rgba.resize((size_t)width * height * 4);
    for (size_t i = 0; i < indices.size(); i++) {
        std::memcpy(&rgba[i * 4], &palette[indices[i] * 4], 4);
    }
}

namespace ImageIO {
std::optional<Image> DecodePng(const std::vector<uint8_t>& bytes) {
    lodepng::State state;
    unsigned width = 0;
    unsigned height = 0;
    if (lodepng_inspect(&width, &height, &state, bytes.data(), bytes.size()) != 0) {
        return std::nullopt;
    }

    const LodePNGColorMode& color = state.info_png.color;
    Image image;
    image.width = (int)width;
    image.height = (int)height;

    if (color.colortype == LCT_PALETTE) {
        state.info_raw.colortype = LCT_PALETTE;
        state.info_raw.bitdepth = 8;
        std::vector<uint8_t> indices;
        if (lodepng::decode(indices, width, height, state, bytes) != 0) {
            return std::nullopt;
        }
        image.hasPalette = true;
        image.indices = std::move(indices);
        bool hasAlpha = false;
        const LodePNGColorMode& pal = state.info_png.color;
        for (size_t i = 0; i < pal.palettesize; i++) {
            image.SetPaletteColor((int)i, pal.palette[i * 4], pal.palette[i * 4 + 1], pal.palette[i * 4 + 2],
                                  pal.palette[i * 4 + 3]);
            hasAlpha |= pal.palette[i * 4 + 3] != 255;
        }
        image.channels = hasAlpha ? 4 : 3;
        image.ResolvePalette();
        return image;
    }

    switch (color.colortype) {
        case LCT_GREY:
            image.channels = 1;
            break;
        case LCT_GREY_ALPHA:
            image.channels = 2;
            break;
        case LCT_RGB:
            image.channels = 3;
            break;
        default:
            image.channels = 4;
            break;
    }
    if (lodepng::decode(image.rgba, width, height, bytes, LCT_RGBA, 8) != 0) {
        return std::nullopt;
    }
    return image;
}

std::optional<Image> DecodeJpeg(const std::vector<uint8_t>& bytes) {
    int w = 0;
    int h = 0;
    int comp = 0;
    stbi_uc* data = stbi_load_from_memory(bytes.data(), (int)bytes.size(), &w, &h, &comp, 4);
    if (data == nullptr) {
        return std::nullopt;
    }
    Image image(w, h, 3);
    std::memcpy(image.rgba.data(), data, (size_t)w * h * 4);
    stbi_image_free(data);
    return image;
}

std::optional<Image> Decode(const std::vector<uint8_t>& bytes) {
    static const uint8_t kPngMagic[] = { 0x89, 'P', 'N', 'G' };
    if (bytes.size() >= 4 && std::memcmp(bytes.data(), kPngMagic, 4) == 0) {
        return DecodePng(bytes);
    }
    return DecodeJpeg(bytes);
}

std::optional<std::pair<int, int>> PngSize(const std::vector<uint8_t>& bytes) {
    lodepng::State state;
    unsigned width = 0;
    unsigned height = 0;
    if (lodepng_inspect(&width, &height, &state, bytes.data(), bytes.size()) != 0) {
        return std::nullopt;
    }
    return std::make_pair((int)width, (int)height);
}

std::optional<std::pair<int, int>> JpegSize(const std::vector<uint8_t>& bytes) {
    int w = 0;
    int h = 0;
    int comp = 0;
    if (!stbi_info_from_memory(bytes.data(), (int)bytes.size(), &w, &h, &comp)) {
        return std::nullopt;
    }
    return std::make_pair(w, h);
}

std::vector<uint8_t> EncodePng(const Image& image) {
    std::vector<uint8_t> out;
    lodepng::State state;
    state.encoder.auto_convert = 0;

    if (image.hasPalette) {
        state.info_raw.colortype = LCT_PALETTE;
        state.info_raw.bitdepth = 8;
        state.info_png.color.colortype = LCT_PALETTE;
        state.info_png.color.bitdepth = 8;
        const int size = std::max(image.paletteSize, 1);
        for (int i = 0; i < size; i++) {
            const uint8_t* c = &image.palette[i * 4];
            lodepng_palette_add(&state.info_png.color, c[0], c[1], c[2], c[3]);
            lodepng_palette_add(&state.info_raw, c[0], c[1], c[2], c[3]);
        }
        lodepng::encode(out, image.indices, (unsigned)image.width, (unsigned)image.height, state);
        return out;
    }

    const bool alpha = (image.channels % 2) == 0;
    const bool grey = image.channels <= 2;
    const LodePNGColorType type = grey ? (alpha ? LCT_GREY_ALPHA : LCT_GREY) : (alpha ? LCT_RGBA : LCT_RGB);
    state.info_raw.colortype = LCT_RGBA;
    state.info_raw.bitdepth = 8;
    state.info_png.color.colortype = type;
    state.info_png.color.bitdepth = 8;
    lodepng::encode(out, image.rgba, (unsigned)image.width, (unsigned)image.height, state);
    return out;
}
} // namespace ImageIO

namespace ImageOps {
Image Convert(const Image& image, int numChannels) {
    Image out(image.width, image.height, numChannels);
    out.rgba = image.rgba;
    if (numChannels == 1 || numChannels == 3) {
        for (size_t i = 3; i < out.rgba.size(); i += 4) {
            out.rgba[i] = 0xFF;
        }
    }
    if (numChannels <= 2) {
        for (size_t i = 0; i < out.rgba.size(); i += 4) {
            out.rgba[i + 1] = out.rgba[i];
            out.rgba[i + 2] = out.rgba[i];
        }
    }
    return out;
}

Image Resize(const Image& image, int width, int height, Interpolation interpolation) {
    if (image.hasPalette || interpolation == Interpolation::Nearest) {
        Image out = image;
        out.width = width;
        out.height = height;
        out.rgba.assign((size_t)width * height * 4, 0);
        if (image.hasPalette) {
            out.indices.assign((size_t)width * height, 0);
        }
        for (int y = 0; y < height; y++) {
            const int sy = std::min(image.height - 1, (int)((y + 0.5) * image.height / height));
            for (int x = 0; x < width; x++) {
                const int sx = std::min(image.width - 1, (int)((x + 0.5) * image.width / width));
                std::memcpy(out.Pixel(x, y), image.Pixel(sx, sy), 4);
                if (image.hasPalette) {
                    out.indices[(size_t)y * width + x] = image.indices[(size_t)sy * image.width + sx];
                }
            }
        }
        return out;
    }

    Image out(width, height, image.channels);
    stbir_resize(image.rgba.data(), image.width, image.height, 0, out.rgba.data(), width, height, 0, STBIR_RGBA,
                 STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP, STBIR_FILTER_CATMULLROM);
    return out;
}

Image Crop(const Image& image, int x, int y, int width, int height) {
    Image out = image;
    out.width = width;
    out.height = height;
    out.rgba.assign((size_t)width * height * 4, 0);
    if (image.hasPalette) {
        out.indices.assign((size_t)width * height, 0);
    }
    for (int dy = 0; dy < height; dy++) {
        const int sy = y + dy;
        if (sy < 0 || sy >= image.height) {
            continue;
        }
        for (int dx = 0; dx < width; dx++) {
            const int sx = x + dx;
            if (sx < 0 || sx >= image.width) {
                continue;
            }
            std::memcpy(out.Pixel(dx, dy), image.Pixel(sx, sy), 4);
            if (image.hasPalette) {
                out.indices[(size_t)dy * width + dx] = image.indices[(size_t)sy * image.width + sx];
            }
        }
    }
    return out;
}

void CompositeDirect(Image& dst, const Image& src, int dstX, int dstY) {
    for (int y = 0; y < src.height; y++) {
        const int ty = dstY + y;
        if (ty < 0 || ty >= dst.height) {
            continue;
        }
        for (int x = 0; x < src.width; x++) {
            const int tx = dstX + x;
            if (tx < 0 || tx >= dst.width) {
                continue;
            }
            std::memcpy(dst.Pixel(tx, ty), src.Pixel(x, y), 4);
        }
    }
}

void CompositeOver(Image& dst, const Image& src, int dstX, int dstY) {
    for (int y = 0; y < src.height; y++) {
        const int ty = dstY + y;
        if (ty < 0 || ty >= dst.height) {
            continue;
        }
        for (int x = 0; x < src.width; x++) {
            const int tx = dstX + x;
            if (tx < 0 || tx >= dst.width) {
                continue;
            }
            const uint8_t* s = src.Pixel(x, y);
            uint8_t* d = dst.Pixel(tx, ty);
            const float sa = s[3] / 255.0f;
            const float da = d[3] / 255.0f;
            const float oa = sa + da * (1.0f - sa);
            for (int c = 0; c < 3; c++) {
                const float value = oa > 0.0f ? (s[c] * sa + d[c] * da * (1.0f - sa)) / oa : 0.0f;
                d[c] = (uint8_t)std::clamp(std::lround(value), 0L, 255L);
            }
            d[3] = (uint8_t)std::lround(oa * 255.0f);
        }
    }
}

Image PadCanvas(const Image& image, int width, int height) {
    if (image.width == width && image.height == height) {
        return image;
    }
    Image canvas(width, height, 4);
    CompositeDirect(canvas, image, 0, 0);
    return canvas;
}

uint8_t Luminance(const uint8_t* rgba) {
    return (uint8_t)std::clamp(std::lround(0.299 * rgba[0] + 0.587 * rgba[1] + 0.114 * rgba[2]), 0L, 255L);
}
} // namespace ImageOps
