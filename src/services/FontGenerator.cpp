#include "FontGenerator.h"

#include "utils/Paths.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <spdlog/spdlog.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include <stb_truetype.h>

namespace {
const char* const kFontTable[] = {
    "0",  "4",  "1",  "5",  "2",  "6",  "3",  "7",  "8",  "C",  "9",  "D",  "A",  "E",  "B",  "F",  "0",  "4",  "1",
    "5",  "2",  "6",  "3",  "7",  "8",  "←",  "9",  "→",  "A",  "↑",  "B",  "↓",  " ",  "$",  "!",  "%",  "\"", "&",
    "#",  "'",  "(",  ",",  ")",  "-",  "*",  ".",  "+",  "/",  "0",  "4",  "1",  "5",  "2",  "6",  "3",  "7",  "8",
    "<",  "9",  "=",  ":",  ">",  ";",  "?",  "@",  "D",  "A",  "E",  "B",  "F",  "C",  "G",  "H",  "L",  "I",  "M",
    "J",  "N",  "K",  "O",  "P",  "T",  "Q",  "U",  "R",  "V",  "S",  "W",  "X",  "¥",  "Y",  "]",  "Z",  "^",  "[",
    "_",  "'",  "d",  "a",  "e",  "b",  "f",  "c",  "g",  "h",  "l",  "i",  "m",  "j",  "n",  "k",  "o",  "p",  "t",
    "q",  "u",  "r",  "v",  "s",  "w",  "x",  ";",  "y",  "}",  "z",  "~",  "{",  "␡",  " ",  ",",  ".",  " ",  " ",
    " ",  " ",  " ",  "い", "や", "う", "や", "え", "や", "え", "や", "-",  "え", "あ", "お", "い", "か", "い", "か",
    "く", "し", "け", "チ", "こ", "せ", "こ", "せ", " ",  ",",  " ",  ".",  " ",  " ",  " ",  " ",  "イ", "ヤ", "ウ",
    "コ", "ェ", "ヨ", "エ", "ッ", "-",  "エ", "ア", "オ", "イ", "カ", "ウ", "キ", "ワ", "シ", "ク", "ス", "コ", "セ",
    "サ", "ン", "タ", "ト", "子", "ナ", "ッ", "ニ", "テ", "ヌ", "ネ", "つ", "ノ", "へ", "い", "ホ", "い", "マ", " ",
    " ",  " ",  " ",  " ",  " ",  " ",  " ",  "リ", "ワ", "ル", "ン", "レ", "\"", "口", "ロ", " ",  " ",  " ",  " ",
    " ",  " ",  " ",  " ",  "ね", "ふ", "の", "へ", "は", "ほ", "は", "ま", "み", "せ", "せ", "ゆ", "の", "よ", "も",
    "ら", " ",  "わ", "る", "ん", "れ", "\"", "ろ", "ロ",
};
constexpr int kFontTableSize = (int)(sizeof(kFontTable) / sizeof(kFontTable[0]));

std::vector<unsigned int> DecodeUtf8(const char* text) {
    std::vector<unsigned int> codepoints;
    const auto* s = reinterpret_cast<const unsigned char*>(text);
    while (*s != 0) {
        unsigned int cp = *s;
        int extra = 0;
        if (cp >= 0xF0) {
            cp &= 0x07;
            extra = 3;
        } else if (cp >= 0xE0) {
            cp &= 0x0F;
            extra = 2;
        } else if (cp >= 0xC0) {
            cp &= 0x1F;
            extra = 1;
        }
        s++;
        for (int i = 0; i < extra && (*s & 0xC0) == 0x80; i++, s++) {
            cp = (cp << 6) | (*s & 0x3F);
        }
        codepoints.push_back(cp);
    }
    return codepoints;
}

bool IsSymbol(const char* text) {
    static const char kSymbols[] = "^$*.[]{}()?-\"!@#%&/\\,><:;_~`+='";
    return text[0] != '\0' && text[1] == '\0' && std::strchr(kSymbols, text[0]) != nullptr;
}

void BlendWhite(Image& image, int x, int y, uint8_t coverage) {
    if (x < 0 || y < 0 || x >= image.width || y >= image.height || coverage == 0) {
        return;
    }
    uint8_t* p = image.Pixel(x, y);
    const float sa = coverage / 255.0f;
    const float da = p[3] / 255.0f;
    const float oa = sa + da * (1.0f - sa);
    for (int c = 0; c < 3; c++) {
        p[c] = (uint8_t)std::lround((255.0f * sa + p[c] * da * (1.0f - sa)) / oa);
    }
    p[3] = (uint8_t)std::lround(oa * 255.0f);
}

void StrokeRect(Image& image, int x0, int y0, int w, int h) {
    for (int x = x0; x < x0 + w; x++) {
        for (int y : { y0, y0 + h - 1 }) {
            if (x >= 0 && y >= 0 && x < image.width && y < image.height) {
                uint8_t* p = image.Pixel(x, y);
                p[0] = p[1] = p[2] = 0;
                p[3] = 0xFF;
            }
        }
    }
    for (int y = y0; y < y0 + h; y++) {
        for (int x : { x0, x0 + w - 1 }) {
            if (x >= 0 && y >= 0 && x < image.width && y < image.height) {
                uint8_t* p = image.Pixel(x, y);
                p[0] = p[1] = p[2] = 0;
                p[3] = 0xFF;
            }
        }
    }
}
} // namespace

bool FontGenerator::Load(const std::filesystem::path& ttf) {
    auto data = Paths::ReadFile(ttf);
    stbtt_fontinfo info;
    if (!data || !stbtt_InitFont(&info, data->data(), stbtt_GetFontOffsetForIndex(data->data(), 0))) {
        SPDLOG_ERROR("Failed to load font {}", Paths::ToUtf8(ttf));
        return false;
    }
    mFontData = std::move(*data);
    mFontName = Paths::ToUtf8(ttf.stem());
    return true;
}

Image FontGenerator::Render(const Settings& settings, int width, int height) const {
    Image image(width, height, 4);
    if (!IsLoaded()) {
        return image;
    }
    stbtt_fontinfo font;
    stbtt_InitFont(&font, mFontData.data(), stbtt_GetFontOffsetForIndex(mFontData.data(), 0));
    int ascent = 0;
    int descent = 0;
    int lineGap = 0;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);

    const int gw = settings.glyphWidth;
    const int gh = settings.glyphHeight;
    for (int id = 0; id < settings.rows * settings.perRow; id++) {
        const int tx = id % settings.perRow;
        const int ty = id / settings.perRow;
        if (id >= kFontTableSize) {
            continue;
        }
        const char* text = kFontTable[id];
        if (settings.drawGrid) {
            StrokeRect(image, tx * gw, ty * gh, gw, gh);
        }
        const bool space = std::strcmp(text, " ") == 0;
        const char* glyph = space && ty >= 16 ? "�" : text;
        const float fontSize = space ? 20.0f : (float)(IsSymbol(text) ? gh - 5 : gh) * settings.scale;
        if (fontSize <= 0.0f) {
            continue;
        }
        const float scale = stbtt_ScaleForMappingEmToPixels(&font, fontSize);

        const std::vector<unsigned int> codepoints = DecodeUtf8(glyph);
        float textWidth = 0.0f;
        for (const unsigned int cp : codepoints) {
            if (stbtt_FindGlyphIndex(&font, (int)cp) == 0) {
                continue;
            }
            int advance = 0;
            int lsb = 0;
            stbtt_GetCodepointHMetrics(&font, (int)cp, &advance, &lsb);
            textWidth += advance * scale;
        }
        const float textHeight = (ascent - descent) * scale;
        float penX = tx * gw + gw / 2.0f - textWidth / 2.0f;
        const float top = ty * gh + gh / 2.0f - textHeight / 2.0f - settings.offset;
        const float baseline = top + ascent * scale;

        for (const unsigned int cp : codepoints) {
            if (stbtt_FindGlyphIndex(&font, (int)cp) == 0) {
                continue;
            }
            int advance = 0;
            int lsb = 0;
            stbtt_GetCodepointHMetrics(&font, (int)cp, &advance, &lsb);
            int x0 = 0;
            int y0 = 0;
            int x1 = 0;
            int y1 = 0;
            const float shiftX = penX - std::floor(penX);
            stbtt_GetCodepointBitmapBoxSubpixel(&font, (int)cp, scale, scale, shiftX, 0.0f, &x0, &y0, &x1, &y1);
            const int bw = x1 - x0;
            const int bh = y1 - y0;
            if (bw > 0 && bh > 0) {
                std::vector<uint8_t> bitmap((size_t)bw * bh);
                stbtt_MakeCodepointBitmapSubpixel(&font, bitmap.data(), bw, bh, bw, scale, scale, shiftX, 0.0f,
                                                  (int)cp);
                const int ox = (int)std::floor(penX) + x0;
                const int oy = (int)std::lround(baseline) + y0;
                for (int y = 0; y < bh; y++) {
                    for (int x = 0; x < bw; x++) {
                        BlendWhite(image, ox + x, oy + y, bitmap[(size_t)y * bw + x]);
                    }
                }
            }
            penX += advance * scale;
        }
    }
    return image;
}

Image FontGenerator::RenderTable(const Settings& settings) const {
    Settings table = settings;
    table.drawGrid = false;
    table.perRow = 8;
    table.rows = 32;
    return Render(table, settings.glyphWidth * 8, settings.glyphHeight * 32);
}
