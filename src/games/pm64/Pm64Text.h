#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Otr {
class ArchiveReader;
}

namespace Pm64 {
inline constexpr std::string_view kMessagesDir = "messages";

constexpr uint8_t kTerminator = 0xFD;

enum class Charset { Standard, Credits };

using Messages = std::map<std::string, std::vector<uint8_t>>;

Charset CharsetFor(std::string_view path);

std::string DecodeText(const std::vector<uint8_t>& bytes, Charset charset = Charset::Standard);
std::optional<std::vector<uint8_t>> EncodeText(std::string_view text, Charset charset = Charset::Standard,
                                               std::string* error = nullptr);

std::vector<std::pair<size_t, size_t>> Tokenize(const std::vector<uint8_t>& bytes, Charset charset = Charset::Standard);

std::optional<std::vector<uint8_t>> ParseMessage(const std::vector<uint8_t>& resource);

Messages LoadMessages(Otr::ArchiveReader& archive);

std::vector<char> ExportMessage(const std::vector<uint8_t>& text, const std::string& entryName);
} // namespace Pm64
