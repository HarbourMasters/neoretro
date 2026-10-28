#include "UpdateFeed.h"

#include <algorithm>
#include <cctype>
#include <nlohmann/json.hpp>
#include <vector>

namespace UpdateFeed {
namespace {
std::vector<int> VersionParts(std::string_view version) {
    std::vector<int> parts;
    size_t start = 0;
    while (start <= version.size()) {
        const size_t dot = std::min(version.find('.', start), version.size());
        int value = 0;
        for (size_t i = start; i < dot; i++) {
            value = value * 10 + (version[i] - '0');
        }
        parts.push_back(value);
        start = dot + 1;
    }
    return parts;
}

bool Fail(std::string* error, const char* message) {
    if (error) {
        *error = message;
    }
    return false;
}

bool IsSha256(const std::string& text) {
    return text.size() == 64 &&
           std::all_of(text.begin(), text.end(), [](char c) { return std::isxdigit((unsigned char)c); });
}
bool IsAllowedUrl(const std::string& url) {
    return url.rfind("https://", 0) == 0 || url.rfind("http://127.0.0.1", 0) == 0 ||
           url.rfind("http://localhost", 0) == 0;
}
} // namespace

static std::optional<Release> ParseDocument(const nlohmann::json& document, std::string_view platform,
                                            std::string* error);

const char* PlatformKey() {
#if defined(_WIN32)
#if defined(_M_ARM64)
    return "windows-arm64";
#else
    return "windows-x64";
#endif
#elif defined(__APPLE__)
    return "mac-universal";
#elif defined(__aarch64__)
    return "linux-arm64";
#else
    return "linux-x64";
#endif
}

bool IsValidVersion(std::string_view version) {
    if (version.empty() || version.size() > 32 || version.front() == '.' || version.back() == '.') {
        return false;
    }
    for (size_t i = 0; i < version.size(); i++) {
        const char c = version[i];
        if (c == '.' ? version[i - 1] == '.' : !std::isdigit((unsigned char)c)) {
            return false;
        }
    }
    return true;
}

bool IsNewer(std::string_view candidate, std::string_view current) {
    const auto a = VersionParts(candidate);
    const auto b = VersionParts(current);
    for (size_t i = 0; i < std::max(a.size(), b.size()); i++) {
        const int x = i < a.size() ? a[i] : 0;
        const int y = i < b.size() ? b[i] : 0;
        if (x != y) {
            return x > y;
        }
    }
    return false;
}

std::optional<Release> Parse(const std::string& json, std::string_view platform, std::string* error) {
    const auto document = nlohmann::json::parse(json, nullptr, false);
    if (document.is_discarded() || !document.is_object()) {
        Fail(error, "The update feed is not valid JSON.");
        return std::nullopt;
    }
    try {
        return ParseDocument(document, platform, error);
    } catch (const nlohmann::json::exception&) {
        Fail(error, "The update feed is malformed.");
        return std::nullopt;
    }
}

std::optional<Release> ParseDocument(const nlohmann::json& document, std::string_view platform, std::string* error) {
    Release release;
    release.version = document.value("version", "");
    release.notes = document.value("notes", "");
    if (!IsValidVersion(release.version)) {
        Fail(error, "The update feed has no valid version.");
        return std::nullopt;
    }
    const auto assets = document.find("assets");
    if (assets == document.end() || !assets->is_object() || !assets->contains(platform)) {
        Fail(error, "The update has no build for this platform.");
        return std::nullopt;
    }
    const auto& asset = (*assets)[std::string(platform)];
    if (!asset.is_object()) {
        Fail(error, "The update feed is malformed.");
        return std::nullopt;
    }
    release.url = asset.value("url", "");
    release.sha256 = asset.value("sha256", "");
    std::transform(release.sha256.begin(), release.sha256.end(), release.sha256.begin(), ::tolower);
    if (!IsAllowedUrl(release.url) || !IsSha256(release.sha256)) {
        Fail(error, "The update feed is malformed.");
        return std::nullopt;
    }
    return release;
}
} // namespace UpdateFeed
