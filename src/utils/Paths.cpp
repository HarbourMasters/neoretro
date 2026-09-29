#include "Paths.h"

#include <algorithm>
#include <fstream>
#include <spdlog/spdlog.h>

namespace Paths {
std::string Normalize(std::string path) {
    std::replace(path.begin(), path.end(), '\\', '/');
    return path;
}

std::string ToPosix(const fs::path& path) {
    const auto u8 = path.generic_u8string();
    return Normalize(std::string(u8.begin(), u8.end()));
}

std::string Relative(const fs::path& path, const fs::path& base) {
    std::error_code ec;
    auto rel = fs::relative(path, base, ec);
    if (ec) {
        return ToPosix(path);
    }
    return ToPosix(rel);
}

std::string Basename(std::string_view path) {
    const auto normalized = Normalize(std::string(path));
    const auto slash = normalized.find_last_of('/');
    return slash == std::string::npos ? normalized : normalized.substr(slash + 1);
}

std::string BasenameWithoutExtension(std::string_view path) {
    const auto base = Basename(path);
    const auto dot = base.find_last_of('.');
    return (dot == std::string::npos || dot == 0) ? base : base.substr(0, dot);
}

std::string WithoutExtension(std::string_view path) {
    const auto normalized = Normalize(std::string(path));
    const auto slash = normalized.find_last_of('/');
    const auto dot = normalized.find_last_of('.');
    if (dot == std::string::npos || (slash != std::string::npos && dot < slash) || dot == slash + 1) {
        return normalized;
    }
    return normalized.substr(0, dot);
}

std::string Dirname(std::string_view path) {
    const auto normalized = Normalize(std::string(path));
    const auto slash = normalized.find_last_of('/');
    return slash == std::string::npos ? "." : normalized.substr(0, slash);
}

std::string Extension(std::string_view path) {
    const auto base = Basename(path);
    const auto dot = base.find_last_of('.');
    return (dot == std::string::npos || dot == 0) ? "" : base.substr(dot);
}

std::string Join(std::string_view a, std::string_view b) {
    if (a.empty() || a == ".") {
        return std::string(b);
    }
    if (a.back() == '/') {
        return std::string(a) + std::string(b);
    }
    return std::string(a) + "/" + std::string(b);
}

fs::path FromUtf8(const std::string& path) {
    return fs::path(std::u8string(path.begin(), path.end()));
}

std::string ToUtf8(const fs::path& path) {
    const auto u8 = path.u8string();
    return std::string(u8.begin(), u8.end());
}

std::optional<std::vector<uint8_t>> ReadFile(const fs::path& path) {
    std::ifstream stream(path, std::ios::binary);
    if (!stream) {
        return std::nullopt;
    }
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(stream), {});
}

std::optional<std::string> ReadText(const fs::path& path) {
    std::ifstream stream(path, std::ios::binary);
    if (!stream) {
        return std::nullopt;
    }
    return std::string(std::istreambuf_iterator<char>(stream), {});
}

bool WriteFile(const fs::path& path, const std::vector<uint8_t>& data) {
    std::error_code ec;
    if (path.has_parent_path()) {
        fs::create_directories(path.parent_path(), ec);
    }
    std::ofstream stream(path, std::ios::binary);
    if (!stream) {
        SPDLOG_ERROR("Failed to open {} for writing", ToUtf8(path));
        return false;
    }
    stream.write(reinterpret_cast<const char*>(data.data()), (std::streamsize)data.size());
    return (bool)stream;
}

bool WriteText(const fs::path& path, const std::string& text) {
    return WriteFile(path, std::vector<uint8_t>(text.begin(), text.end()));
}

std::vector<fs::path> ListFilesRecursive(const fs::path& folder) {
    std::vector<fs::path> files;
    std::error_code ec;
    for (auto it = fs::recursive_directory_iterator(folder, fs::directory_options::skip_permission_denied, ec);
         it != fs::recursive_directory_iterator(); it.increment(ec)) {
        if (ec) {
            break;
        }
        if (it->is_regular_file(ec)) {
            files.push_back(it->path());
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}
} // namespace Paths
