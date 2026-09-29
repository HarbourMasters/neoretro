#include "Texture.h"
#include "Resource.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <n64graphics/n64graphics.h>
#include <spdlog/spdlog.h>
#include <sstream>

namespace Otr {
namespace {
template <typename T> struct MallocDeleter {
    void operator()(T* ptr) const {
        std::free(ptr);
    }
};
template <typename T> using CPtr = std::unique_ptr<T, MallocDeleter<T>>;

std::vector<ia> ToIntensity(const Image& image, bool withAlpha) {
    std::vector<ia> out((size_t)image.width * image.height);
    for (size_t i = 0; i < out.size(); i++) {
        out[i].intensity = image.rgba[i * 4];
        out[i].alpha = withAlpha ? image.rgba[i * 4 + 3] : 0xFF;
    }
    return out;
}

std::string Trim(std::string value) {
    const auto begin = value.find_first_not_of(" \t\r\n");
    const auto end = value.find_last_not_of(" \t\r\n");
    return begin == std::string::npos ? "" : value.substr(begin, end - begin + 1);
}
} // namespace

std::optional<Texture> Texture::Read(const std::vector<uint8_t>& resource) {
    const auto header = ReadHeader(resource);
    if (!header || header->type != (uint32_t)ResourceType::Texture) {
        return std::nullopt;
    }
    try {
        ResourceReader reader(resource, header->bigEndian);
        Texture texture;
        texture.version = (Version)header->version;
        const uint32_t type = reader.ReadU32();
        if (type >= (uint32_t)kTextureTypeCount) {
            return std::nullopt;
        }
        texture.type = (TextureType)type;
        texture.width = (int)reader.ReadU32();
        texture.height = (int)reader.ReadU32();
        if (texture.version == Version::Roy) {
            texture.flags = reader.ReadU32();
            texture.hByteScale = reader.ReadFloat();
            texture.vPixelScale = reader.ReadFloat();
        }
        const uint32_t size = reader.ReadU32();
        texture.data = reader.ReadBytes(size);
        return texture;
    } catch (const std::exception& e) {
        SPDLOG_WARN("Malformed texture resource: {}", e.what());
        return std::nullopt;
    }
}

namespace TextureCodec {
bool FromImage(Texture& texture, const Image& image) {
    texture.width = image.width;
    texture.height = image.height;
    const int w = image.width;
    const int h = image.height;
    texture.data.assign(BufferSize(texture.type, w, h), 0);
    uint8_t* raw = texture.data.data();
    const auto* pixels = reinterpret_cast<const rgba*>(image.rgba.data());

    switch (texture.type) {
        case TextureType::RGBA16bpp:
            return rgba2raw(raw, pixels, w, h, 16) >= 0;
        case TextureType::RGBA32bpp:
            return rgba2raw(raw, pixels, w, h, 32) >= 0;
        case TextureType::Palette4bpp:
        case TextureType::Palette8bpp: {
            std::vector<ci> indices((size_t)w * h);
            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    indices[(size_t)y * w + x].index = image.Index(x, y);
                }
            }
            if (texture.type == TextureType::Palette4bpp && (w % 2) != 0) {
                indices.push_back({ 0 });
            }
            return ci2raw_torch(raw, indices.data(), w, h, BitSize(texture.type)) >= 0;
        }
        case TextureType::Grayscale4bpp:
        case TextureType::Grayscale8bpp: {
            const auto intensity = ToIntensity(image, false);
            return i2raw(raw, intensity.data(), w, h, BitSize(texture.type)) >= 0;
        }
        case TextureType::GrayscaleAlpha4bpp:
        case TextureType::GrayscaleAlpha8bpp:
        case TextureType::GrayscaleAlpha16bpp: {
            const auto intensity = ToIntensity(image, true);
            return ia2raw(raw, intensity.data(), w, h, BitSize(texture.type)) >= 0;
        }
        default:
            SPDLOG_ERROR("Unknown texture type {}", (int)texture.type);
            return false;
    }
}

std::optional<Image> ToImage(const Texture& texture, const Texture* tlut) {
    const int w = texture.width;
    const int h = texture.height;
    if (w <= 0 || h <= 0 || texture.data.size() < BufferSize(texture.type, w, h)) {
        return std::nullopt;
    }
    const uint8_t* raw = texture.data.data();
    Image image(w, h, HasAlpha(texture.type) ? 4 : 3);

    switch (texture.type) {
        case TextureType::RGBA32bpp:
        case TextureType::RGBA16bpp: {
            CPtr<rgba> pixels(raw2rgba(raw, w, h, texture.type == TextureType::RGBA32bpp ? 32 : 16));
            if (!pixels) {
                return std::nullopt;
            }
            std::memcpy(image.rgba.data(), pixels.get(), (size_t)w * h * 4);
            break;
        }
        case TextureType::Palette4bpp:
        case TextureType::Palette8bpp: {
            const bool ci4 = texture.type == TextureType::Palette4bpp;
            CPtr<ci> indices(raw2ci_torch(raw, w, h, ci4 ? 4 : 8));
            if (!indices) {
                return std::nullopt;
            }
            image.hasPalette = true;
            image.indices.resize((size_t)w * h);
            for (size_t i = 0; i < image.indices.size(); i++) {
                image.indices[i] = indices.get()[i].index;
            }
            const int colors = ci4 ? 16 : 256;
            for (int i = 0; i < colors; i++) {
                const uint8_t grey = (uint8_t)(ci4 ? i * 16 : i);
                image.SetPaletteColor(i, grey, grey, grey, 0xFF);
            }
            if (tlut != nullptr) {
                if (const auto pal = ToImage(*tlut)) {
                    const int count = std::min(pal->width * pal->height, 256);
                    for (int i = 0; i < count; i++) {
                        const uint8_t* c = &pal->rgba[(size_t)i * 4];
                        image.SetPaletteColor(i, c[0], c[1], c[2], c[3]);
                    }
                }
            }
            image.ResolvePalette();
            break;
        }
        case TextureType::Grayscale4bpp:
        case TextureType::Grayscale8bpp: {
            CPtr<ia> pixels(raw2i(raw, w, h, BitSize(texture.type)));
            if (!pixels) {
                return std::nullopt;
            }
            for (size_t i = 0; i < (size_t)w * h; i++) {
                const uint8_t v = pixels.get()[i].intensity;
                uint8_t* p = &image.rgba[i * 4];
                p[0] = p[1] = p[2] = v;
                p[3] = 0xFF;
            }
            break;
        }
        case TextureType::GrayscaleAlpha4bpp:
        case TextureType::GrayscaleAlpha8bpp:
        case TextureType::GrayscaleAlpha16bpp: {
            CPtr<ia> pixels(raw2ia(raw, w, h, BitSize(texture.type)));
            if (!pixels) {
                return std::nullopt;
            }
            for (size_t i = 0; i < (size_t)w * h; i++) {
                const ia& px = pixels.get()[i];
                uint8_t* p = &image.rgba[i * 4];
                p[0] = p[1] = p[2] = px.intensity;
                p[3] = px.alpha;
            }
            break;
        }
        default:
            return std::nullopt;
    }
    return image;
}
} // namespace TextureCodec

std::optional<Background> Background::Read(const std::vector<uint8_t>& resource) {
    const auto header = ReadHeader(resource);
    if (!header || header->type != (uint32_t)ResourceType::OoTBackground) {
        return std::nullopt;
    }
    try {
        ResourceReader reader(resource, header->bigEndian);
        const uint32_t size = reader.ReadU32();
        return Background{ reader.ReadBytes(size) };
    } catch (const std::exception& e) {
        SPDLOG_WARN("Malformed background resource: {}", e.what());
        return std::nullopt;
    }
}

std::optional<Sequence> Sequence::FromSeqFile(const std::vector<uint8_t>& seq, const std::string& meta) {
    std::vector<std::string> lines;
    std::stringstream stream(meta);
    for (std::string line; std::getline(stream, line);) {
        lines.push_back(line);
    }
    if (lines.size() < 2) {
        SPDLOG_ERROR("Sequence meta file needs at least a name and a font index");
        return std::nullopt;
    }

    std::string fontLine = Trim(lines[1]);
    std::transform(fontLine.begin(), fontLine.end(), fontLine.begin(), ::tolower);
    if (fontLine.rfind("0x", 0) == 0) {
        fontLine = fontLine.substr(2);
    }
    char* end = nullptr;
    const unsigned long fontIndex = std::strtoul(fontLine.c_str(), &end, 16);
    if (fontLine.empty() || end == fontLine.c_str()) {
        SPDLOG_ERROR("Invalid font index in sequence meta: {}", lines[1]);
        return std::nullopt;
    }

    std::string type = lines.size() > 2 ? Trim(lines[2]) : "";
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    if (type.empty()) {
        type = "bgm";
    }
    std::string name = Trim(lines[0]);
    std::replace(name.begin(), name.end(), '/', '|');

    Sequence sequence;
    sequence.rawBinary = seq;
    sequence.cachePolicy = type == "bgm" ? 2 : 1;
    sequence.medium = 2;
    sequence.sequenceNum = 0;
    sequence.fontIndices = { (uint8_t)fontIndex };
    sequence.path = name + "_" + type;
    return sequence;
}
} // namespace Otr
