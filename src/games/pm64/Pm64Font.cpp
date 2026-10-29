#include "Pm64Font.h"

#include "otr/Archive.h"
#include "otr/Texture.h"

#include <algorithm>
#include <cstring>
#include <spdlog/spdlog.h>

namespace Pm64 {
namespace {
constexpr std::string_view kGlyphPrefix = "charset/charset_standard_";
constexpr std::string_view kPalettePrefix = "charset/charset_standard_palette_";
constexpr int kLineHeight = 16;
constexpr int kBoxGap = 8;
constexpr int kMonospaceWidth = 10;
constexpr float kSpaceMultiplier = 0.6f;

constexpr uint8_t kGlyphWidths[] = {
    11, 8, 8, 10, 9,  9,  9,  4,  8,  8,  9,  9,  5,  9,  5,  10, 9,  7,  9,  9,  9,  9,  9, 9, 9, 9, 5, 5,
    8,  9, 8, 9,  10, 9,  9,  9,  9,  9,  9,  9,  9,  7,  9,  9,  9,  9,  9,  9,  9,  9,  9, 9, 9, 9, 9, 9,
    9,  9, 9, 7,  9,  6,  7,  9,  6,  9,  8,  8,  9,  8,  8,  8,  7,  4,  7,  8,  4,  9,  8, 7, 9, 8, 8, 8,
    8,  8, 8, 9,  8,  8,  8,  7,  3,  7,  10, 7,  9,  9,  9,  9,  9,  9,  9,  9,  9,  7,  7, 7, 8, 9, 9, 9,
    9,  9, 9, 9,  9,  9,  9,  9,  9,  9,  9,  8,  8,  8,  8,  8,  6,  6,  6,  7,  9,  7,  7, 7, 8, 8, 8, 8,
    8,  6, 9, 7,  11, 11, 11, 11, 12, 12, 11, 11, 13, 13, 14, 14, 12, 13, 13, 13, 13, 16, 9, 9, 5, 5,
};
constexpr int kGlyphCount = (int)sizeof(kGlyphWidths);

std::optional<Image> ReadImage(Otr::ArchiveReader& archive, const std::string& name) {
    const auto data = archive.Read(name);
    const auto texture = data ? Otr::Texture::Read(*data) : std::nullopt;
    return texture ? Otr::TextureCodec::ToImage(*texture) : std::nullopt;
}

int SpaceWidth(uint8_t code) {
    switch (code) {
        case 0xF7:
            return (int)(kMonospaceWidth * kSpaceMultiplier);
        case 0xF8:
            return kMonospaceWidth;
        case 0xF9:
            return kMonospaceWidth / 2;
        default:
            return 0;
    }
}
} // namespace

Font LoadFont(Otr::ArchiveReader& archive) {
    Font font;
    for (int i = 0; i < kGlyphCount; i++) {
        const auto glyph = ReadImage(archive, std::string(kGlyphPrefix) + std::to_string(i));
        if (!glyph || !glyph->hasPalette || glyph->width != kGlyphSize || glyph->height != kGlyphSize) {
            SPDLOG_WARN("Paper Mario font glyph {} is missing", i);
            return {};
        }
        font.glyphs.push_back(glyph->indices);
    }
    for (int i = 0;; i++) {
        const auto palette = ReadImage(archive, std::string(kPalettePrefix) + std::to_string(i));
        if (!palette || palette->width * palette->height < 16) {
            break;
        }
        auto& colors = font.palettes.emplace_back();
        std::memcpy(colors.data(), palette->rgba.data(), colors.size());
    }
    SPDLOG_INFO("Loaded Paper Mario font with {} palettes", font.palettes.size());
    return font;
}

Image RenderText(const Font& font, const std::vector<uint8_t>& bytes, int palette) {
    struct Placed {
        int glyph;
        int x;
        int y;
        int palette;
    };
    std::vector<Placed> placed;
    int x = 0;
    int y = 0;
    int width = 0;
    int savedPalette = palette;
    for (const auto& [at, length] : Tokenize(bytes)) {
        const uint8_t code = bytes[at];
        if (length == 1 && code < kGlyphCount) {
            placed.push_back({ code, x, y, palette });
            x += kGlyphWidths[code];
        } else if (length == 1 && code == 0xF0) {
            x = 0;
            y += kLineHeight;
        } else if (length == 1 && SpaceWidth(code) > 0) {
            x += SpaceWidth(code);
        } else if (code == 0xFB) {
            x = 0;
            y += kLineHeight + kBoxGap;
        } else if (code == 0xFF && length >= 2) {
            const uint8_t function = bytes[at + 1];
            if (function == 0x05 && length == 3) {
                palette = bytes[at + 2];
            } else if (function == 0x24) {
                savedPalette = palette;
            } else if (function == 0x25) {
                palette = savedPalette;
            }
        }
        width = std::max(width, x);
    }

    Image image(std::max(width, 1), y + kLineHeight, 4);
    for (const auto& glyph : placed) {
        const auto& colors = font.palettes[std::clamp(glyph.palette, 0, (int)font.palettes.size() - 1)];
        const auto& indices = font.glyphs[glyph.glyph];
        for (int gy = 0; gy < kGlyphSize; gy++) {
            for (int gx = 0; gx < kGlyphSize; gx++) {
                const int px = glyph.x + gx;
                const int py = glyph.y + gy;
                const uint8_t* color = &colors[indices[gy * kGlyphSize + gx] * 4];
                if (px >= image.width || py >= image.height || color[3] == 0) {
                    continue;
                }
                std::memcpy(image.Pixel(px, py), color, 4);
            }
        }
    }
    return image;
}
} // namespace Pm64
