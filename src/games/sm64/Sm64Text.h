#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace Otr {
class ArchiveReader;
}

namespace Sm64 {
inline constexpr std::string_view kDialogsDir = "texts/dialogs";
inline constexpr std::string_view kCoursesDir = "texts/courses";
inline constexpr std::string_view kActsDir = "texts/acts";
inline constexpr std::string_view kStringsDir = "texts/strings";
inline constexpr std::string_view kStringsName = "global";

constexpr uint8_t kNewline = 0xFE;
constexpr uint8_t kTerminator = 0xFF;

struct Dialog {
    uint32_t unused = 1;
    int8_t linesPerBox = 4;
    int16_t leftOffset = 30;
    int16_t width = 200;
    std::vector<uint8_t> text;
};

using Dictionary = std::map<std::string, std::vector<uint8_t>>;

struct TextAssets {
    std::map<std::string, Dialog> dialogs;
    std::map<std::string, std::vector<uint8_t>> texts;
    std::optional<Dictionary> strings;

    bool Empty() const {
        return dialogs.empty() && texts.empty() && !strings;
    }
};

std::string DecodeText(const std::vector<uint8_t>& bytes);
std::optional<std::vector<uint8_t>> EncodeText(std::string_view text, std::string* error = nullptr);

std::optional<Dialog> ParseDialog(const std::vector<uint8_t>& resource);
std::optional<std::vector<uint8_t>> ParseText(const std::vector<uint8_t>& resource);
std::optional<Dictionary> ParseDictionary(const std::vector<uint8_t>& resource);

TextAssets LoadTextAssets(Otr::ArchiveReader& archive);

std::vector<char> ExportDialog(const Dialog& dialog, const std::string& entryName);
std::vector<char> ExportText(const std::vector<uint8_t>& text, const std::string& entryName);
std::vector<char> ExportDictionary(const Dictionary& dictionary, const std::string& entryName);
} // namespace Sm64
