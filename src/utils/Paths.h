#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace Paths {
std::string Normalize(std::string path);
std::string ToPosix(const fs::path& path);
std::string Relative(const fs::path& path, const fs::path& base);
std::string Basename(std::string_view path);
std::string BasenameWithoutExtension(std::string_view path);
std::string WithoutExtension(std::string_view path);
std::string Dirname(std::string_view path);
std::string Extension(std::string_view path);
std::string Join(std::string_view a, std::string_view b);
fs::path FromUtf8(const std::string& path);
std::string ToUtf8(const fs::path& path);

std::optional<std::vector<uint8_t>> ReadFile(const fs::path& path);
std::optional<std::string> ReadText(const fs::path& path);
bool WriteFile(const fs::path& path, const std::vector<uint8_t>& data);
bool WriteText(const fs::path& path, const std::string& text);

std::vector<fs::path> ListFilesRecursive(const fs::path& folder);
} // namespace Paths
