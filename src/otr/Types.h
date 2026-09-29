#pragma once

#include <cstdint>
#include <factories/ResourceType.h>
#include <string_view>

namespace Otr {
using ResourceType = Torch::ResourceType;

enum class Version : uint32_t {
    Deckard = 0,
    Roy = 1,
    Rachael = 2,
    Zhora = 3,
};

enum class TextureType : uint32_t {
    Error = 0,
    RGBA32bpp = 1,
    RGBA16bpp = 2,
    Palette4bpp = 3,
    Palette8bpp = 4,
    Grayscale4bpp = 5,
    Grayscale8bpp = 6,
    GrayscaleAlpha4bpp = 7,
    GrayscaleAlpha8bpp = 8,
    GrayscaleAlpha16bpp = 9,
    JPEG32bpp = 10,
};
constexpr int kTextureTypeCount = 11;

constexpr uint32_t kTextureFlagLoadAsRaw = 1 << 0;

std::string_view TextureTypeName(TextureType type);
double PixelMultiplier(TextureType type);
int BitSize(TextureType type);
size_t BufferSize(TextureType type, int width, int height);
bool IsPalette(TextureType type);
bool HasAlpha(TextureType type);
} // namespace Otr
