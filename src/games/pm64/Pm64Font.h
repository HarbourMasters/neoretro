#pragma once

#include "Pm64Text.h"
#include "image/Image.h"

#include <array>
#include <cstdint>
#include <vector>

namespace Otr {
class ArchiveReader;
}

namespace Pm64 {
constexpr int kGlyphSize = 16;
constexpr int kStandardPalette = 0x0A;

struct Font {
    std::vector<std::vector<uint8_t>> glyphs;
    std::vector<std::array<uint8_t, 16 * 4>> palettes;

    bool Empty() const {
        return glyphs.empty() || palettes.empty();
    }
};

Font LoadFont(Otr::ArchiveReader& archive);

Image RenderText(const Font& font, const std::vector<uint8_t>& bytes, int palette = kStandardPalette);
} // namespace Pm64
