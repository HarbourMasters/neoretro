#include "Pm64Text.h"

#include "otr/Archive.h"
#include "otr/Exporters.h"
#include "otr/Resource.h"
#include "utils/Paths.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace Pm64 {
namespace {
struct Arg {
    uint8_t width = 1;
    bool hex = false;
};

struct Code {
    std::vector<uint8_t> prefix;
    std::string_view name;
    std::vector<Arg> args;
    bool breakAfter = false;
};

constexpr Arg kByte{ 1, false };
constexpr Arg kHexByte{ 1, true };
constexpr Arg kShort{ 2, false };
constexpr Arg kHexShort{ 2, true };
constexpr Arg kHexWord{ 4, true };

constexpr uint8_t kNewline = 0xF0;
constexpr uint8_t kSpace = 0xF7;
constexpr uint8_t kStyle = 0xFC;
constexpr uint8_t kFunction = 0xFF;
constexpr uint8_t kLiteralBracket = 0x3B;

std::vector<Code> CommonCodes() {
    std::vector<Code> codes = {
        { { 0xF1 }, "Wait" },
        { { 0xF2 }, "Pause", { kByte } },
        { { 0xF3 }, "Variant0" },
        { { 0xF4 }, "Variant1" },
        { { 0xF5 }, "Variant2" },
        { { 0xF6 }, "Variant3" },
        { { 0xF8 }, "FullSpace" },
        { { 0xF9 }, "HalfSpace" },
        { { 0xFB }, "Next", {}, true },

        { { kStyle }, "Style", { kHexByte }, true },
        { { kStyle, 0x01 }, "Style right", {}, true },
        { { kStyle, 0x02 }, "Style left", {}, true },
        { { kStyle, 0x03 }, "Style center", {}, true },
        { { kStyle, 0x04 }, "Style tattle", {}, true },
        { { kStyle, 0x05 }, "Style choice", { kByte, kByte, kByte, kByte }, true },
        { { kStyle, 0x06 }, "Style inspect", {}, true },
        { { kStyle, 0x07 }, "Style sign", {}, true },
        { { kStyle, 0x08 }, "Style lamppost", { kByte }, true },
        { { kStyle, 0x09 }, "Style postcard", { kByte }, true },
        { { kStyle, 0x0A }, "Style popup", {}, true },
        { { kStyle, 0x0B }, "Style popup2", {}, true },
        { { kStyle, 0x0C }, "Style upgrade", { kByte, kByte, kByte, kByte }, true },
        { { kStyle, 0x0D }, "Style narrate", {}, true },
        { { kStyle, 0x0E }, "Style epilogue", {}, true },

        { { kFunction }, "Func", { kHexByte } },
        { { kFunction, 0x00 }, "Font", { kByte }, true },
        { { kFunction, 0x00, 0x00 }, "Font standard", {}, true },
        { { kFunction, 0x00, 0x01 }, "Font menu", {}, true },
        { { kFunction, 0x00, 0x02 }, "Font menu2", {}, true },
        { { kFunction, 0x00, 0x03 }, "Font title", {}, true },
        { { kFunction, 0x00, 0x04 }, "Font subtitle", {}, true },
        { { kFunction, 0x01 }, "FontVariant", { kByte } },
        { { kFunction, 0x02 }, "FramePalette", { kByte } },
        { { kFunction, 0x03 }, "ResetGfx" },
        { { kFunction, 0x04 }, "Yield" },
        { { kFunction, 0x05 }, "Color", { kHexByte } },
        { { kFunction, 0x06 }, "NoSkip" },
        { { kFunction, 0x07 }, "InputOff", {}, true },
        { { kFunction, 0x08 }, "InputOn", {}, true },
        { { kFunction, 0x09 }, "DelayOff", {}, true },
        { { kFunction, 0x0A }, "DelayOn", {}, true },
        { { kFunction, 0x0B }, "CharWidth", { kByte } },
        { { kFunction, 0x0C }, "Scroll", { kByte } },
        { { kFunction, 0x0D }, "Size", { kByte, kByte }, true },
        { { kFunction, 0x0E }, "SizeReset", {}, true },
        { { kFunction, 0x0F }, "Speed", { kByte, kByte } },
        { { kFunction, 0x10 }, "SetPosX", { kShort } },
        { { kFunction, 0x11 }, "SetPosY", { kByte } },
        { { kFunction, 0x12 }, "Right", { kByte } },
        { { kFunction, 0x13 }, "Down", { kByte } },
        { { kFunction, 0x14 }, "Up", { kByte } },
        { { kFunction, 0x15 }, "InlineImage", { kByte }, true },
        { { kFunction, 0x16 }, "AnimSprite", { kHexShort, kByte }, true },
        { { kFunction, 0x17 }, "ItemIcon", { kHexShort }, true },
        { { kFunction, 0x18 }, "Image", { kByte, kShort, kByte, kByte, kByte, kByte }, true },
        { { kFunction, 0x19 }, "HideImage", { kByte }, true },
        { { kFunction, 0x1A }, "AnimDelay", { kByte, kByte, kByte } },
        { { kFunction, 0x1B }, "AnimLoop", { kByte, kByte } },
        { { kFunction, 0x1C }, "AnimDone", { kByte } },
        { { kFunction, 0x1D }, "SetCursorPos", { kByte, kShort, kByte } },
        { { kFunction, 0x1E }, "Cursor", { kByte } },
        { { kFunction, 0x1F }, "EndChoice", { kByte } },
        { { kFunction, 0x20 }, "SetCancel", { kByte } },
        { { kFunction, 0x21 }, "Option", { kByte } },
        { { kFunction, 0x22 }, "SavePos" },
        { { kFunction, 0x23 }, "RestorePos" },
        { { kFunction, 0x24 }, "SaveColor" },
        { { kFunction, 0x25 }, "RestoreColor" },
        { { kFunction, 0x26 }, "FX", { kHexByte } },
        { { kFunction, 0x27 }, "/FX", { kHexByte } },
        { { kFunction, 0x28 }, "Var", { kByte } },
        { { kFunction, 0x29 }, "CenterX", { kByte } },
        { { kFunction, 0x2A }, "SetRewind", { kByte } },
        { { kFunction, 0x2B }, "EnableCDownNext" },
        { { kFunction, 0x2C }, "CustomVoice", { kHexWord, kHexWord } },
        { { kFunction, 0x2E }, "Volume", { kByte } },
        { { kFunction, 0x2F }, "Voice", { kByte }, true },
        { { kFunction, 0x2F, 0x00 }, "Voice normal", {}, true },
        { { kFunction, 0x2F, 0x01 }, "Voice bowser", {}, true },
        { { kFunction, 0x2F, 0x02 }, "Voice star", {}, true },
        { { kFunction, 0x26, 0x05, 0x00 }, "Blur x" },
        { { kFunction, 0x26, 0x05, 0x01 }, "Blur y" },
        { { kFunction, 0x26, 0x05, 0x02 }, "Blur xy" },
    };

    struct Effect {
        uint8_t id;
        std::string_view start;
        std::string_view end;
        bool hasArg;
    };
    static constexpr Effect kEffects[] = {
        { 0x00, "Shake", "/Shake", false },
        { 0x01, "Wave", "/Wave", false },
        { 0x02, "NoiseOutline", "/NoiseOutline", false },
        { 0x03, "Static", "/Static", true },
        { 0x05, "Blur", "/Blur", true },
        { 0x06, "Rainbow", "/Rainbow", false },
        { 0x07, "DitherFade", "/DitherFade", true },
        { 0x08, "GlobalWave", "/GlobalWave", false },
        { 0x09, "GlobalRainbow", "/GlobalRainbow", false },
        { 0x0A, "PrintRising", "/PrintRising", false },
        { 0x0B, "PrintGrowing", "/PrintGrowing", false },
        { 0x0C, "SizeJitter", "/SizeJitter", false },
        { 0x0D, "SizeWave", "/SizeWave", false },
        { 0x0E, "DropShadow", "/DropShadow", false },
    };
    for (const auto& effect : kEffects) {
        codes.push_back({ { kFunction, 0x26, effect.id },
                          effect.start,
                          effect.hasArg ? std::vector<Arg>{ kByte } : std::vector<Arg>{} });
        codes.push_back({ { kFunction, 0x27, effect.id }, effect.end });
    }
    return codes;
}

std::vector<Code> StandardCodes() {
    std::vector<Code> codes = CommonCodes();
    const auto button = [](uint8_t color, uint8_t glyph) {
        return std::vector<uint8_t>{ kFunction, 0x24, kFunction, 0x05, color, glyph, kFunction, 0x25 };
    };
    const std::vector<Code> extra = {
        { button(0x10, 0x98), "A" },
        { button(0x11, 0x99), "B" },
        { button(0x12, 0xA1), "START" },
        { button(0x13, 0x9D), "C-UP" },
        { button(0x13, 0x9E), "C-DOWN" },
        { button(0x13, 0x9F), "C-LEFT" },
        { button(0x13, 0xA0), "C-RIGHT" },
        { button(0x14, 0x9C), "Z" },
        { { 0x00 }, "NOTE" },
        { { 0x90 }, "HEART" },
        { { 0x91 }, "STAR" },
        { { 0x92 }, "UP" },
        { { 0x93 }, "DOWN" },
        { { 0x94 }, "LEFT" },
        { { 0x95 }, "RIGHT" },
        { { 0x96 }, "CIRCLE" },
        { { 0x97 }, "CROSS" },
        { { 0x98 }, "~A" },
        { { 0x99 }, "~B" },
        { { 0x9A }, "~L" },
        { { 0x9B }, "~R" },
        { { 0x9C }, "~Z" },
        { { 0x9D }, "~C-UP" },
        { { 0x9E }, "~C-DOWN" },
        { { 0x9F }, "~C-LEFT" },
        { { 0xA0 }, "~C-RIGHT" },
        { { 0xA1 }, "~START" },
    };
    codes.insert(codes.end(), extra.begin(), extra.end());
    return codes;
}

using GlyphTable = std::array<std::string_view, 256>;

GlyphTable StandardGlyphs() {
    GlyphTable glyphs{};
    static constexpr std::string_view kAscii = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (size_t i = 0; i < kAscii.size(); i++) {
        glyphs[0x01 + i] = kAscii.substr(i, 1);
    }
    glyphs[kLiteralBracket] = "\\[";
    static constexpr std::string_view kTail[] = {
        "\xc2\xa5", "]",        "^",        "_",        "`",        "a",        "b",        "c",        "d",
        "e",        "f",        "g",        "h",        "i",        "j",        "k",        "l",        "m",
        "n",        "o",        "p",        "q",        "r",        "s",        "t",        "u",        "v",
        "w",        "x",        "y",        "z",        "{",        "|",        "}",        "~",        "\xc2\xb0",
        "\xc3\x80", "\xc3\x81", "\xc3\x82", "\xc3\x84", "\xc3\x87", "\xc3\x88", "\xc3\x89", "\xc3\x8a", "\xc3\x8b",
        "\xc3\x8c", "\xc3\x8d", "\xc3\x8e", "\xc3\x8f", "\xc3\x91", "\xc3\x92", "\xc3\x93", "\xc3\x94", "\xc3\x96",
        "\xc3\x99", "\xc3\x9a", "\xc3\x9b", "\xc3\x9c", "\xc3\x9f", "\xc3\xa0", "\xc3\xa1", "\xc3\xa2", "\xc3\xa4",
        "\xc3\xa7", "\xc3\xa8", "\xc3\xa9", "\xc3\xaa", "\xc3\xab", "\xc3\xac", "\xc3\xad", "\xc3\xae", "\xc3\xaf",
        "\xc3\xb1", "\xc3\xb2", "\xc3\xb3", "\xc3\xb4", "\xc3\xb6", "\xc3\xb9", "\xc3\xba", "\xc3\xbb", "\xc3\xbc",
        "\xc2\xa1", "\xc2\xbf", "\xc2\xaa",
    };
    for (size_t i = 0; i < std::size(kTail); i++) {
        glyphs[0x3C + i] = kTail[i];
    }
    glyphs[0xA2] = "\xe2\x80\x9c";
    glyphs[0xA3] = "\xe2\x80\x9d";
    glyphs[0xA4] = "\xe2\x80\x98";
    glyphs[0xA5] = "\xe2\x80\x99";
    return glyphs;
}

GlyphTable CreditsGlyphs() {
    GlyphTable glyphs{};
    static constexpr std::string_view kChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ'.,0123456789";
    for (size_t i = 0; i < kChars.size(); i++) {
        glyphs[i] = kChars.substr(i, 1);
    }
    glyphs[0x27] = "\xc2\xa9";
    glyphs[0x28] = "&";
    return glyphs;
}

struct Table {
    GlyphTable glyphs;
    std::map<std::string_view, uint8_t> reverse;
    std::vector<Code> codes;
    std::array<std::vector<const Code*>, 256> byFirstByte;
    std::vector<const Code*> byNameWords;
};

size_t WordCount(std::string_view name) {
    return std::count(name.begin(), name.end(), ' ') + 1;
}

const Table& TableFor(Charset charset) {
    const auto build = [](GlyphTable glyphs, std::vector<Code> codes) {
        Table table;
        table.glyphs = glyphs;
        table.glyphs[kNewline] = "\n";
        table.glyphs[kSpace] = " ";
        for (int i = 0; i < 256; i++) {
            if (!table.glyphs[i].empty()) {
                table.reverse.emplace(table.glyphs[i], (uint8_t)i);
            }
        }
        table.codes = std::move(codes);
        for (const auto& code : table.codes) {
            table.byFirstByte[code.prefix[0]].push_back(&code);
            table.byNameWords.push_back(&code);
        }
        for (auto& list : table.byFirstByte) {
            std::stable_sort(list.begin(), list.end(),
                             [](const Code* a, const Code* b) { return a->prefix.size() > b->prefix.size(); });
        }
        std::stable_sort(table.byNameWords.begin(), table.byNameWords.end(),
                         [](const Code* a, const Code* b) { return WordCount(a->name) > WordCount(b->name); });
        return table;
    };
    static const Table sStandard = build(StandardGlyphs(), StandardCodes());
    static const Table sCredits = build(CreditsGlyphs(), CommonCodes());
    return charset == Charset::Credits ? sCredits : sStandard;
}

size_t ArgsSize(const Code& code) {
    size_t size = 0;
    for (const auto& arg : code.args) {
        size += arg.width;
    }
    return size;
}

bool Matches(const Code& code, const std::vector<uint8_t>& bytes, size_t at) {
    if (at + code.prefix.size() + ArgsSize(code) > bytes.size()) {
        return false;
    }
    return std::equal(code.prefix.begin(), code.prefix.end(), bytes.begin() + at);
}

std::string FormatCode(const Code& code, const std::vector<uint8_t>& bytes, size_t at) {
    std::string out = "[" + std::string(code.name);
    for (const auto& arg : code.args) {
        uint32_t value = 0;
        for (int i = 0; i < arg.width; i++) {
            value = value << 8 | bytes[at++];
        }
        out += arg.hex ? fmt::format(" 0x{:0{}X}", value, arg.width * 2) : fmt::format(" {}", value);
    }
    out += "]";
    if (code.breakAfter) {
        out += "\n";
    }
    return out;
}

std::string Lower(std::string_view value) {
    std::string out(value);
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return out;
}

std::vector<std::string> SplitCode(std::string_view content) {
    std::vector<std::string> words;
    std::string current;
    const auto flush = [&] {
        if (const auto equals = current.find('='); equals != std::string::npos) {
            current.erase(0, equals + 1);
        }
        if (!current.empty()) {
            words.push_back(current);
        }
        current.clear();
    };
    for (char c : content) {
        if (c == ' ' || c == ',' || c == '\t') {
            flush();
        } else {
            current += c;
        }
    }
    flush();
    return words;
}

std::optional<uint32_t> ParseNumber(const std::string& word, uint8_t width) {
    try {
        size_t used = 0;
        const bool hex = word.size() > 2 && word[0] == '0' && (word[1] == 'x' || word[1] == 'X');
        const std::string digits = hex ? word.substr(2) : word;
        const unsigned long value = std::stoul(digits, &used, hex ? 16 : 10);
        if (used != digits.size() || !std::isxdigit((unsigned char)digits[0]) ||
            (width < 4 && value >= (1ul << (width * 8))) || value > 0xFFFFFFFFul) {
            return std::nullopt;
        }
        return (uint32_t)value;
    } catch (const std::exception&) { return std::nullopt; }
}

bool EncodeCode(std::string_view content, const Table& table, std::vector<uint8_t>& out, bool& breakAfter) {
    const auto words = SplitCode(content);
    if (words.empty()) {
        return false;
    }
    if (words.size() == 2 && Lower(words[0]) == "raw") {
        if (const auto value = ParseNumber(words[1], 1)) {
            out.push_back((uint8_t)*value);
            breakAfter = false;
            return true;
        }
        return false;
    }
    for (const Code* code : table.byNameWords) {
        const auto nameWords = SplitCode(code->name);
        if (words.size() != nameWords.size() + code->args.size()) {
            continue;
        }
        bool match = true;
        for (size_t i = 0; i < nameWords.size() && match; i++) {
            match = Lower(words[i]) == Lower(nameWords[i]);
        }
        std::vector<uint8_t> bytes = code->prefix;
        for (size_t i = 0; i < code->args.size() && match; i++) {
            const auto& arg = code->args[i];
            const auto value = ParseNumber(words[nameWords.size() + i], arg.width);
            match = value.has_value();
            for (int shift = (arg.width - 1) * 8; match && shift >= 0; shift -= 8) {
                bytes.push_back((uint8_t)(*value >> shift));
            }
        }
        if (match) {
            out.insert(out.end(), bytes.begin(), bytes.end());
            breakAfter = code->breakAfter;
            return true;
        }
    }
    return false;
}

size_t Utf8Length(std::string_view text, size_t at) {
    const auto lead = (uint8_t)text[at];
    const size_t length = lead < 0x80 ? 1 : lead < 0xE0 ? 2 : lead < 0xF0 ? 3 : 4;
    return std::min(length, text.size() - at);
}

class MessageData : public IParsedData {
  public:
    explicit MessageData(std::vector<uint8_t> text) : mText(std::move(text)) {
    }
    std::vector<uint8_t> mText;
};

class MessageBinaryExporter : public BaseExporter {
  public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node& node,
                        std::string* replacement) override {
        auto text = std::static_pointer_cast<MessageData>(raw)->mText;
        text.resize((text.size() + 3) & ~(size_t)3, 0);
        auto writer = LUS::BinaryWriter();
        WriteHeader(writer, Torch::ResourceType::Blob, 0);
        writer.Write((uint32_t)text.size());
        writer.Write((char*)text.data(), text.size());
        writer.Finish(write);
        return std::nullopt;
    }
};
} // namespace

Charset CharsetFor(std::string_view path) {
    const auto slash = path.rfind('/');
    const std::string_view name = slash == std::string_view::npos ? path : path.substr(slash + 1);
    return name.starts_with("MSG_Credits_") ? Charset::Credits : Charset::Standard;
}

std::string DecodeText(const std::vector<uint8_t>& bytes, Charset charset) {
    const Table& table = TableFor(charset);
    std::string out;
    size_t at = 0;
    while (at < bytes.size() && bytes[at] != kTerminator) {
        const Code* match = nullptr;
        for (const Code* code : table.byFirstByte[bytes[at]]) {
            if (Matches(*code, bytes, at)) {
                match = code;
                break;
            }
        }
        if (match != nullptr) {
            out += FormatCode(*match, bytes, at + match->prefix.size());
            at += match->prefix.size() + ArgsSize(*match);
        } else if (!table.glyphs[bytes[at]].empty()) {
            out += table.glyphs[bytes[at]];
            at++;
        } else {
            out += fmt::format("[Raw 0x{:02X}]", bytes[at]);
            at++;
        }
    }
    return out;
}

std::vector<std::pair<size_t, size_t>> Tokenize(const std::vector<uint8_t>& bytes, Charset charset) {
    const Table& table = TableFor(charset);
    std::vector<std::pair<size_t, size_t>> tokens;
    size_t at = 0;
    while (at < bytes.size() && bytes[at] != kTerminator) {
        size_t length = 1;
        for (const Code* code : table.byFirstByte[bytes[at]]) {
            if (Matches(*code, bytes, at)) {
                length = code->prefix.size() + ArgsSize(*code);
                break;
            }
        }
        tokens.emplace_back(at, length);
        at += length;
    }
    return tokens;
}

std::optional<std::vector<uint8_t>> EncodeText(std::string_view text, Charset charset, std::string* error) {
    const Table& table = TableFor(charset);
    const auto fail = [&](std::string message) -> std::optional<std::vector<uint8_t>> {
        if (error) {
            *error = std::move(message);
        }
        return std::nullopt;
    };
    std::vector<uint8_t> out;
    size_t at = 0;
    while (at < text.size()) {
        if (text[at] == '\r') {
            at++;
            continue;
        }
        if (text[at] == '[') {
            const size_t close = text.find(']', at);
            if (close == std::string_view::npos) {
                return fail(fmt::format("Unclosed '[' at position {} (write \\[ for a literal bracket)", at + 1));
            }
            const auto content = text.substr(at + 1, close - at - 1);
            bool breakAfter = false;
            if (!EncodeCode(content, table, out, breakAfter)) {
                return fail(fmt::format("Unknown code [{}] at position {}", content, at + 1));
            }
            at = close + 1;
            if (breakAfter && at < text.size() && text[at] == '\r') {
                at++;
            }
            if (breakAfter && at < text.size() && text[at] == '\n') {
                at++;
            }
            continue;
        }
        const size_t length = text[at] == '\\' ? std::min<size_t>(2, text.size() - at) : Utf8Length(text, at);
        const auto glyph = table.reverse.find(text.substr(at, length));
        if (glyph == table.reverse.end()) {
            return fail(fmt::format("Unsupported character '{}' at position {}", text.substr(at, length), at + 1));
        }
        out.push_back(glyph->second);
        at += length;
    }
    out.push_back(kTerminator);
    return out;
}

std::optional<std::vector<uint8_t>> ParseMessage(const std::vector<uint8_t>& resource) {
    const auto header = Otr::ReadHeader(resource);
    if (!header || header->type != (uint32_t)Otr::ResourceType::Blob) {
        return std::nullopt;
    }
    try {
        Otr::ResourceReader reader(resource, header->bigEndian);
        auto bytes = reader.ReadBytes(reader.ReadU32());
        while (!bytes.empty() && bytes.back() == 0) {
            bytes.pop_back();
        }
        if (bytes.empty() || bytes.back() != kTerminator) {
            return std::nullopt;
        }
        return bytes;
    } catch (const std::out_of_range&) { return std::nullopt; }
}

Messages LoadMessages(Otr::ArchiveReader& archive) {
    const std::string prefix = std::string(kMessagesDir) + "/";
    Messages messages;
    for (const auto& name : archive.List()) {
        if (name.size() <= prefix.size() || name.compare(0, prefix.size(), prefix) != 0) {
            continue;
        }
        if (const auto data = archive.Read(name)) {
            if (auto message = ParseMessage(*data)) {
                messages[name] = std::move(*message);
            }
        }
    }
    SPDLOG_INFO("Loaded {} messages", messages.size());
    return messages;
}

std::vector<char> ExportMessage(const std::vector<uint8_t>& text, const std::string& entryName) {
    MessageBinaryExporter exporter;
    return Otr::ExportResource(exporter, std::make_shared<MessageData>(text), Paths::Basename(entryName));
}
} // namespace Pm64
