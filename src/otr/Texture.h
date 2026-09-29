#pragma once

#include "Types.h"
#include "image/Image.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace Otr {
struct Texture {
    TextureType type = TextureType::Error;
    int width = 0;
    int height = 0;
    uint32_t flags = 0;
    float hByteScale = 1.0f;
    float vPixelScale = 1.0f;
    Version version = Version::Deckard;
    std::vector<uint8_t> data;

    void SetFlags(uint32_t value) {
        version = Version::Roy;
        flags |= value;
    }
    void SetScale(float hByte, float vPixel) {
        version = Version::Roy;
        hByteScale = hByte;
        vPixelScale = vPixel;
    }

    static std::optional<Texture> Read(const std::vector<uint8_t>& resource);
};

namespace TextureCodec {
bool FromImage(Texture& texture, const Image& image);
std::optional<Image> ToImage(const Texture& texture, const Texture* tlut = nullptr);
} // namespace TextureCodec

struct Background {
    std::vector<uint8_t> data;

    static std::optional<Background> Read(const std::vector<uint8_t>& resource);
};

struct Sequence {
    std::vector<uint8_t> rawBinary;
    uint8_t sequenceNum = 0;
    uint8_t medium = 2;
    uint8_t cachePolicy = 2;
    std::vector<uint8_t> fontIndices;
    std::string path;

    static std::optional<Sequence> FromSeqFile(const std::vector<uint8_t>& seq, const std::string& meta);
};
} // namespace Otr
