#include "Sm64Text.h"

#include "otr/Archive.h"
#include "otr/Exporters.h"
#include "otr/Resource.h"

#include <algorithm>
#include <array>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace Sm64 {
namespace {
struct Glyph {
    std::vector<uint8_t> bytes;
    std::string_view text;
};

const std::vector<Glyph>& Glyphs() {
    static const std::vector<Glyph> sGlyphs = [] {
        std::vector<Glyph> glyphs;
        static constexpr std::string_view kAlnum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'.";
        for (size_t i = 0; i < kAlnum.size(); i++) {
            glyphs.push_back({ { (uint8_t)i }, kAlnum.substr(i, 1) });
        }
        const std::vector<Glyph> symbols = {
            { { 0x40, 0x41 }, "{mario}" },
            { { 0x50 }, "^" },
            { { 0x51 }, "|" },
            { { 0x52 }, "<" },
            { { 0x53 }, ">" },
            { { 0x54 }, "[A]" },
            { { 0x55 }, "[B]" },
            { { 0x56 }, "[C]" },
            { { 0x57 }, "[Z]" },
            { { 0x58 }, "[R]" },
            { { 0x60 }, "\xc3\xa0" },
            { { 0x61 }, "\xc3\xa2" },
            { { 0x62 }, "\xc3\xa4" },
            { { 0x64 }, "\xc3\x80" },
            { { 0x65 }, "\xc3\x82" },
            { { 0x66 }, "\xc3\x84" },
            { { 0x6F }, "," },
            { { 0x70 }, "\xc3\xa8" },
            { { 0x71 }, "\xc3\xaa" },
            { { 0x72 }, "\xc3\xab" },
            { { 0x73 }, "\xc3\xa9" },
            { { 0x74 }, "\xc3\x88" },
            { { 0x75 }, "\xc3\x8a" },
            { { 0x76 }, "\xc3\x8b" },
            { { 0x77 }, "\xc3\x89" },
            { { 0x80 }, "\xc3\xb9" },
            { { 0x81 }, "\xc3\xbb" },
            { { 0x82 }, "\xc3\xbc" },
            { { 0x84 }, "\xc3\x99" },
            { { 0x85 }, "\xc3\x9b" },
            { { 0x86 }, "\xc3\x9c" },
            { { 0x91 }, "\xc3\xb4" },
            { { 0x92 }, "\xc3\xb6" },
            { { 0x95 }, "\xc3\x94" },
            { { 0x96 }, "\xc3\x96" },
            { { 0x9E }, " " },
            { { 0x9F }, "-" },
            { { 0xA1 }, "\xc3\xae" },
            { { 0xA2 }, "\xc3\xaf" },
            { { 0xD0 }, "/" },
            { { 0xD1 }, "{the}" },
            { { 0xD2 }, "{you}" },
            { { 0xE0 }, "[%]" },
            { { 0xE1 }, "(" },
            { { 0xE3 }, ")" },
            { { 0xE4 }, "+" },
            { { 0xE5 }, "&" },
            { { 0xE6 }, ":" },
            { { 0xEC }, "\xc3\x9f" },
            { { 0xED }, "\xc3\x87" },
            { { 0xEE }, "\xc3\xa7" },
            { { 0xF2 }, "!" },
            { { 0xF3 }, "%" },
            { { 0xF4 }, "?" },
            { { 0xF5 }, "\xe2\x80\x9c" },
            { { 0xF6 }, "\xe2\x80\x9d" },
            { { 0xF7 }, "~" },
            { { 0xF8 }, "\xe2\x80\xa6" },
            { { 0xF9 }, "$" },
            { { 0xFA }, "{star}" },
            { { 0xFB }, "\xc3\x97" },
            { { 0xFD }, "{hollow_star}" },
            { { kNewline }, "\n" },
        };
        glyphs.insert(glyphs.end(), symbols.begin(), symbols.end());
        return glyphs;
    }();
    return sGlyphs;
}

const std::vector<Glyph>& Aliases() {
    static const std::vector<Glyph> sAliases = {
        { { 0x40, 0x41 }, "\xe2\x98\xba" },
        { { 0xFA }, "\xe2\x98\x85" },
        { { 0xFD }, "\xe2\x98\x86" },
    };
    return sAliases;
}

const std::vector<const Glyph*>& GlyphsByTextLength() {
    static const std::vector<const Glyph*> sSorted = [] {
        std::vector<const Glyph*> sorted;
        for (const auto& glyph : Glyphs()) {
            sorted.push_back(&glyph);
        }
        for (const auto& glyph : Aliases()) {
            sorted.push_back(&glyph);
        }
        std::stable_sort(sorted.begin(), sorted.end(),
                         [](const Glyph* a, const Glyph* b) { return a->text.size() > b->text.size(); });
        return sorted;
    }();
    return sSorted;
}

std::optional<int> HexValue(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return std::nullopt;
}

size_t Utf8Length(std::string_view text, size_t at) {
    const auto lead = (uint8_t)text[at];
    const size_t length = lead < 0x80 ? 1 : lead < 0xE0 ? 2 : lead < 0xF0 ? 3 : 4;
    return std::min(length, text.size() - at);
}

std::optional<Otr::ResourceReader> OpenResource(const std::vector<uint8_t>& resource, Otr::ResourceType type) {
    const auto header = Otr::ReadHeader(resource);
    if (!header || header->type != (uint32_t)type) {
        return std::nullopt;
    }
    return Otr::ResourceReader(resource, header->bigEndian);
}

std::string BaseName(const std::string& path) {
    const auto slash = path.rfind('/');
    return slash == std::string::npos ? path : path.substr(slash + 1);
}

class DialogData : public IParsedData {
  public:
    explicit DialogData(Dialog dialog) : mDialog(std::move(dialog)) {
    }
    Dialog mDialog;
};

class TextData : public IParsedData {
  public:
    explicit TextData(std::vector<uint8_t> text) : mText(std::move(text)) {
    }
    std::vector<uint8_t> mText;
};

class DictionaryData : public IParsedData {
  public:
    explicit DictionaryData(Dictionary dictionary) : mDictionary(std::move(dictionary)) {
    }
    Dictionary mDictionary;
};

class DialogBinaryExporter : public BaseExporter {
  public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node& node,
                        std::string* replacement) override {
        const auto& dialog = std::static_pointer_cast<DialogData>(raw)->mDialog;
        auto writer = LUS::BinaryWriter();
        WriteHeader(writer, Torch::ResourceType::SDialog, 0);
        writer.Write((uint32_t)dialog.unused);
        writer.Write((int8_t)dialog.linesPerBox);
        writer.Write((int16_t)dialog.leftOffset);
        writer.Write((int16_t)dialog.width);
        writer.Write((uint32_t)dialog.text.size());
        writer.Write((char*)dialog.text.data(), dialog.text.size());
        writer.Finish(write);
        return std::nullopt;
    }
};

class TextBinaryExporter : public BaseExporter {
  public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node& node,
                        std::string* replacement) override {
        const auto& text = std::static_pointer_cast<TextData>(raw)->mText;
        auto writer = LUS::BinaryWriter();
        WriteHeader(writer, Torch::ResourceType::Blob, 0);
        writer.Write((uint32_t)text.size());
        writer.Write((char*)text.data(), text.size());
        writer.Finish(write);
        return std::nullopt;
    }
};

class DictionaryBinaryExporter : public BaseExporter {
  public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node& node,
                        std::string* replacement) override {
        const auto& dictionary = std::static_pointer_cast<DictionaryData>(raw)->mDictionary;
        auto writer = LUS::BinaryWriter();
        WriteHeader(writer, Torch::ResourceType::Dictionary, 0);
        writer.Write((uint32_t)dictionary.size());
        for (const auto& [key, value] : dictionary) {
            writer.Write(key);
            writer.Write((uint32_t)value.size());
            writer.Write((char*)value.data(), value.size());
        }
        writer.Finish(write);
        return std::nullopt;
    }
};
} // namespace

std::string DecodeText(const std::vector<uint8_t>& bytes) {
    std::array<const Glyph*, 256> single{};
    for (const auto& glyph : Glyphs()) {
        if (glyph.bytes.size() == 1) {
            single[glyph.bytes[0]] = &glyph;
        }
    }
    std::string out;
    for (size_t i = 0; i < bytes.size() && bytes[i] != kTerminator; i++) {
        if (bytes[i] == 0x40 && i + 1 < bytes.size() && bytes[i + 1] == 0x41) {
            out += "{mario}";
            i++;
        } else if (const Glyph* glyph = single[bytes[i]]) {
            out += glyph->text;
        } else {
            out += fmt::format("{{{:02X}}}", bytes[i]);
        }
    }
    return out;
}

std::optional<std::vector<uint8_t>> EncodeText(std::string_view text, std::string* error) {
    std::vector<uint8_t> out;
    size_t at = 0;
    while (at < text.size()) {
        if (text[at] == '\r') {
            at++;
            continue;
        }
        if (text[at] == '{' && at + 3 < text.size() && text[at + 3] == '}') {
            const auto hi = HexValue(text[at + 1]);
            const auto lo = HexValue(text[at + 2]);
            if (hi && lo) {
                out.push_back((uint8_t)(*hi << 4 | *lo));
                at += 4;
                continue;
            }
        }
        const Glyph* match = nullptr;
        for (const Glyph* glyph : GlyphsByTextLength()) {
            if (text.substr(at, glyph->text.size()) == glyph->text) {
                match = glyph;
                break;
            }
        }
        if (match == nullptr) {
            if (error) {
                *error = fmt::format("Unsupported character '{}' at position {}", text.substr(at, Utf8Length(text, at)),
                                     at + 1);
            }
            return std::nullopt;
        }
        out.insert(out.end(), match->bytes.begin(), match->bytes.end());
        at += match->text.size();
    }
    out.push_back(kTerminator);
    return out;
}

std::optional<Dialog> ParseDialog(const std::vector<uint8_t>& resource) {
    auto reader = OpenResource(resource, Otr::ResourceType::SDialog);
    if (!reader) {
        return std::nullopt;
    }
    try {
        Dialog dialog;
        dialog.unused = reader->ReadU32();
        dialog.linesPerBox = (int8_t)reader->ReadU8();
        dialog.leftOffset = reader->ReadS16();
        dialog.width = reader->ReadS16();
        dialog.text = reader->ReadBytes(reader->ReadU32());
        return dialog;
    } catch (const std::out_of_range&) { return std::nullopt; }
}

std::optional<std::vector<uint8_t>> ParseText(const std::vector<uint8_t>& resource) {
    auto reader = OpenResource(resource, Otr::ResourceType::Blob);
    if (!reader) {
        return std::nullopt;
    }
    try {
        return reader->ReadBytes(reader->ReadU32());
    } catch (const std::out_of_range&) { return std::nullopt; }
}

std::optional<Dictionary> ParseDictionary(const std::vector<uint8_t>& resource) {
    auto reader = OpenResource(resource, Otr::ResourceType::Dictionary);
    if (!reader) {
        return std::nullopt;
    }
    try {
        Dictionary dictionary;
        const uint32_t count = reader->ReadU32();
        for (uint32_t i = 0; i < count; i++) {
            const auto key = reader->ReadBytes(reader->ReadU32());
            dictionary[std::string(key.begin(), key.end())] = reader->ReadBytes(reader->ReadU32());
        }
        return dictionary;
    } catch (const std::out_of_range&) { return std::nullopt; }
}

TextAssets LoadTextAssets(Otr::ArchiveReader& archive) {
    const auto inDir = [](const std::string& name, std::string_view dir) {
        return name.size() > dir.size() + 1 && name.compare(0, dir.size(), dir) == 0 && name[dir.size()] == '/';
    };
    TextAssets assets;
    for (const auto& name : archive.List()) {
        const bool isDialog = inDir(name, kDialogsDir);
        const bool isText = inDir(name, kCoursesDir) || inDir(name, kActsDir);
        const bool isStrings = name == std::string(kStringsDir) + "/" + std::string(kStringsName);
        if (!isDialog && !isText && !isStrings) {
            continue;
        }
        const auto data = archive.Read(name);
        if (!data) {
            continue;
        }
        if (isDialog) {
            if (auto dialog = ParseDialog(*data)) {
                assets.dialogs[name] = std::move(*dialog);
            }
        } else if (isText) {
            if (auto text = ParseText(*data)) {
                assets.texts[name] = std::move(*text);
            }
        } else if (auto dictionary = ParseDictionary(*data)) {
            assets.strings = std::move(*dictionary);
        }
    }
    SPDLOG_INFO("Loaded {} dialogs, {} texts, {} strings", assets.dialogs.size(), assets.texts.size(),
                assets.strings ? assets.strings->size() : 0);
    return assets;
}

std::vector<char> ExportDialog(const Dialog& dialog, const std::string& entryName) {
    DialogBinaryExporter exporter;
    return Otr::ExportResource(exporter, std::make_shared<DialogData>(dialog), BaseName(entryName));
}

std::vector<char> ExportText(const std::vector<uint8_t>& text, const std::string& entryName) {
    TextBinaryExporter exporter;
    return Otr::ExportResource(exporter, std::make_shared<TextData>(text), BaseName(entryName));
}

std::vector<char> ExportDictionary(const Dictionary& dictionary, const std::string& entryName) {
    DictionaryBinaryExporter exporter;
    return Otr::ExportResource(exporter, std::make_shared<DictionaryData>(dictionary), BaseName(entryName));
}
} // namespace Sm64
