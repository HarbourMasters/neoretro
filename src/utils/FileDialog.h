#pragma once

#include <optional>
#include <string>
#include <vector>

namespace FileDialog {
struct Filter {
    const char* name;
    const char* extensions;
};

void Init();
void Shutdown();

std::optional<std::string> PickFolder();
std::optional<std::string> PickFile(const std::vector<Filter>& filters = {});
std::vector<std::string> PickFiles(const std::vector<Filter>& filters = {});
std::optional<std::string> SaveFile(const std::string& defaultName, const std::vector<Filter>& filters = {});

inline const std::vector<Filter> kArchiveFilters = { { "OTR / O2R", "otr,o2r" } };
inline const std::vector<Filter> kImageFilters = { { "Images", "png,jpg,jpeg" } };
inline const std::vector<Filter> kPngFilters = { { "PNG", "png" } };
inline const std::vector<Filter> kFontFilters = { { "Fonts", "ttf,otf" } };
} // namespace FileDialog
