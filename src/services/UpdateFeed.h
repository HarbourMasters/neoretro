#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace UpdateFeed {
struct Release {
    std::string version;
    std::string notes;
    std::string url;
    std::string sha256;
};

const char* PlatformKey();

bool IsValidVersion(std::string_view version);
bool IsNewer(std::string_view candidate, std::string_view current);

std::optional<Release> Parse(const std::string& json, std::string_view platform, std::string* error = nullptr);
} // namespace UpdateFeed
