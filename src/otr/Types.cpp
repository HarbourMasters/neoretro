#include "Types.h"

#include <cmath>

namespace Otr {
std::string_view TextureTypeName(TextureType type) {
    switch (type) {
        case TextureType::RGBA32bpp:
            return "RGBA32bpp";
        case TextureType::RGBA16bpp:
            return "RGBA16bpp";
        case TextureType::Palette4bpp:
            return "Palette4bpp";
        case TextureType::Palette8bpp:
            return "Palette8bpp";
        case TextureType::Grayscale4bpp:
            return "Grayscale4bpp";
        case TextureType::Grayscale8bpp:
            return "Grayscale8bpp";
        case TextureType::GrayscaleAlpha4bpp:
            return "GrayscaleAlpha4bpp";
        case TextureType::GrayscaleAlpha8bpp:
            return "GrayscaleAlpha8bpp";
        case TextureType::GrayscaleAlpha16bpp:
            return "GrayscaleAlpha16bpp";
        case TextureType::JPEG32bpp:
            return "JPEG32bpp";
        default:
            return "Error";
    }
}

double PixelMultiplier(TextureType type) {
    switch (type) {
        case TextureType::RGBA32bpp:
            return 4;
        case TextureType::RGBA16bpp:
        case TextureType::GrayscaleAlpha16bpp:
            return 2;
        case TextureType::Palette8bpp:
        case TextureType::Grayscale8bpp:
        case TextureType::GrayscaleAlpha8bpp:
            return 1;
        case TextureType::Palette4bpp:
        case TextureType::Grayscale4bpp:
        case TextureType::GrayscaleAlpha4bpp:
            return 0.5;
        default:
            return 0;
    }
}

int BitSize(TextureType type) {
    return (int)std::lround(8 * PixelMultiplier(type));
}

size_t BufferSize(TextureType type, int width, int height) {
    return (size_t)(width * height * PixelMultiplier(type));
}

bool IsPalette(TextureType type) {
    return type == TextureType::Palette4bpp || type == TextureType::Palette8bpp;
}

bool HasAlpha(TextureType type) {
    switch (type) {
        case TextureType::Palette4bpp:
        case TextureType::Palette8bpp:
        case TextureType::RGBA32bpp:
        case TextureType::RGBA16bpp:
        case TextureType::GrayscaleAlpha16bpp:
        case TextureType::GrayscaleAlpha8bpp:
        case TextureType::GrayscaleAlpha4bpp:
            return true;
        default:
            return false;
    }
}
} // namespace Otr
