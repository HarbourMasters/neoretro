#include "StageEntry.h"

std::vector<std::filesystem::path> StageEntryFiles(const StageEntry& entry) {
    std::vector<std::filesystem::path> files;
    if (const auto* custom = std::get_if<CustomStageEntry>(&entry)) {
        files = custom->files;
    } else if (const auto* sequences = std::get_if<CustomSequencesEntry>(&entry)) {
        for (const auto& pair : sequences->pairs) {
            files.push_back(pair.first);
        }
    } else if (const auto* textures = std::get_if<CustomTexturesEntry>(&entry)) {
        for (const auto& pair : textures->pairs) {
            files.push_back(pair.first);
        }
    }
    return files;
}
