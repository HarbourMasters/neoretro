#include "FileDialog.h"

#include <nfd.h>
#include <spdlog/spdlog.h>

namespace FileDialog {
namespace {
std::vector<nfdu8filteritem_t> ToNfd(const std::vector<Filter>& filters) {
    std::vector<nfdu8filteritem_t> items;
    items.reserve(filters.size());
    for (const auto& filter : filters) {
        items.push_back({ filter.name, filter.extensions });
    }
    return items;
}

void LogError(nfdresult_t result) {
    if (result == NFD_ERROR) {
        SPDLOG_ERROR("File dialog error: {}", NFD_GetError());
    }
}
} // namespace

void Init() {
    if (NFD_Init() != NFD_OKAY) {
        SPDLOG_ERROR("Failed to initialize file dialogs: {}", NFD_GetError());
    }
}

void Shutdown() {
    NFD_Quit();
}

std::optional<std::string> PickFolder() {
    nfdu8char_t* out = nullptr;
    const nfdresult_t result = NFD_PickFolderU8(&out, nullptr);
    if (result != NFD_OKAY) {
        LogError(result);
        return std::nullopt;
    }
    std::string path(out);
    NFD_FreePathU8(out);
    return path;
}

std::optional<std::string> PickFile(const std::vector<Filter>& filters) {
    auto items = ToNfd(filters);
    nfdu8char_t* out = nullptr;
    const nfdresult_t result =
        NFD_OpenDialogU8(&out, items.empty() ? nullptr : items.data(), (nfdfiltersize_t)items.size(), nullptr);
    if (result != NFD_OKAY) {
        LogError(result);
        return std::nullopt;
    }
    std::string path(out);
    NFD_FreePathU8(out);
    return path;
}

std::vector<std::string> PickFiles(const std::vector<Filter>& filters) {
    auto items = ToNfd(filters);
    const nfdpathset_t* set = nullptr;
    const nfdresult_t result =
        NFD_OpenDialogMultipleU8(&set, items.empty() ? nullptr : items.data(), (nfdfiltersize_t)items.size(), nullptr);
    std::vector<std::string> paths;
    if (result != NFD_OKAY) {
        LogError(result);
        return paths;
    }
    nfdpathsetsize_t count = 0;
    NFD_PathSet_GetCount(set, &count);
    for (nfdpathsetsize_t i = 0; i < count; i++) {
        nfdu8char_t* path = nullptr;
        if (NFD_PathSet_GetPathU8(set, i, &path) == NFD_OKAY) {
            paths.emplace_back(path);
            NFD_PathSet_FreePathU8(path);
        }
    }
    NFD_PathSet_Free(set);
    return paths;
}

std::optional<std::string> SaveFile(const std::string& defaultName, const std::vector<Filter>& filters) {
    auto items = ToNfd(filters);
    nfdu8char_t* out = nullptr;
    const nfdresult_t result = NFD_SaveDialogU8(&out, items.empty() ? nullptr : items.data(),
                                                (nfdfiltersize_t)items.size(), nullptr, defaultName.c_str());
    if (result != NFD_OKAY) {
        LogError(result);
        return std::nullopt;
    }
    std::string path(out);
    NFD_FreePathU8(out);
    return path;
}
} // namespace FileDialog
