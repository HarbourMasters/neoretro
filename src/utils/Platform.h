#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace Platform {
bool OpenUrl(const char* url);
std::filesystem::path ExecutableDir();
std::vector<std::string> PreferredLanguages();
} // namespace Platform
