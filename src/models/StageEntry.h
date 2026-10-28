#pragma once

#include "TextureManifestEntry.h"

#include <filesystem>
#include <string>
#include <utility>
#include <variant>
#include <vector>

struct CustomStageEntry {
    std::vector<std::filesystem::path> files;
};

struct CustomSequencesEntry {
    std::vector<std::pair<std::filesystem::path, std::filesystem::path>> pairs;
};

using TexturePair = std::pair<std::filesystem::path, TextureManifestEntry>;

struct CustomTexturesEntry {
    std::vector<TexturePair> pairs;
};

struct ResourceDataEntry {
    std::vector<std::pair<std::string, std::vector<char>>> resources;
};

using StageEntry = std::variant<CustomStageEntry, CustomSequencesEntry, CustomTexturesEntry, ResourceDataEntry>;

std::vector<std::filesystem::path> StageEntryFiles(const StageEntry& entry);
