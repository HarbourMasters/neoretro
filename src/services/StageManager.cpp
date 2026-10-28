#include "StageManager.h"

#include "services/OtrGenerator.h"
#include "ui/Toasts.h"
#include "utils/FileDialog.h"
#include "utils/Jobs.h"
#include "utils/Paths.h"

#include <algorithm>
#include <spdlog/spdlog.h>

namespace {
const char* StateName(StageManager::AppState state) {
    return state == StageManager::AppState::ChangesStaged ? "changesStaged" : "none";
}
} // namespace

std::string StageManager::DisplayState() const {
    std::string state = StateName(mState);
    if (HasStagedFiles() && mState != AppState::ChangesStaged) {
        state += " (staged)";
    }
    return state;
}

void StageManager::Reset() {
    mRevision++;
    mState = AppState::None;
    mTotalFiles = 0;
    mProgress.reset();
    mEntries.clear();
}

void StageManager::AddCustomStageEntries(const std::vector<std::filesystem::path>& files,
                                         const std::filesystem::path& basePath) {
    mRevision++;
    std::map<std::string, std::vector<std::filesystem::path>> customEntries;
    for (const auto& file : files) {
        customEntries[Paths::Relative(file.parent_path(), basePath)].push_back(file);
    }
    for (auto& [entryPath, entryFiles] : customEntries) {
        auto it = mEntries.find(entryPath);
        if (it == mEntries.end()) {
            mEntries.emplace(entryPath, CustomStageEntry{ entryFiles });
        } else if (auto* custom = std::get_if<CustomStageEntry>(&it->second)) {
            custom->files.insert(custom->files.end(), entryFiles.begin(), entryFiles.end());
        } else {
            mToasts.Error("Cannot add custom stage entry to existing entry");
            continue;
        }
    }
    mTotalFiles += (int)files.size();
    mState = AppState::ChangesStaged;
}

void StageManager::AddCustomSequenceEntry(
    const std::vector<std::pair<std::filesystem::path, std::filesystem::path>>& pairs, const std::string& path) {
    mRevision++;
    auto it = mEntries.find(path);
    if (it == mEntries.end()) {
        mEntries.emplace(path, CustomSequencesEntry{ pairs });
    } else if (auto* sequences = std::get_if<CustomSequencesEntry>(&it->second)) {
        sequences->pairs.insert(sequences->pairs.end(), pairs.begin(), pairs.end());
    } else {
        mToasts.Error("Cannot add custom sequence entry to existing entry");
        return;
    }
    mTotalFiles += (int)pairs.size();
    mState = AppState::ChangesStaged;
}

void StageManager::AddCustomTextureEntry(const std::map<std::string, std::vector<TexturePair>>& replacementMap) {
    mRevision++;
    for (const auto& [key, pairs] : replacementMap) {
        auto it = mEntries.find(key);
        if (it == mEntries.end()) {
            mEntries.emplace(key, CustomTexturesEntry{ pairs });
        } else if (auto* textures = std::get_if<CustomTexturesEntry>(&it->second)) {
            textures->pairs.insert(textures->pairs.end(), pairs.begin(), pairs.end());
        } else {
            mToasts.Error("Cannot add custom texture entry to existing entry");
            continue;
        }
        mTotalFiles += (int)pairs.size();
    }
    mState = AppState::ChangesStaged;
}

void StageManager::AddResourceData(const std::string& path, const std::string& name, std::vector<char> data) {
    mRevision++;
    auto it = mEntries.find(path);
    if (it == mEntries.end()) {
        it = mEntries.emplace(path, ResourceDataEntry{}).first;
    }
    auto* resources = std::get_if<ResourceDataEntry>(&it->second);
    if (resources == nullptr) {
        mToasts.Error("Cannot add resource data to existing entry");
        return;
    }
    const auto existing = std::find_if(resources->resources.begin(), resources->resources.end(),
                                       [&](const auto& resource) { return resource.first == name; });
    if (existing != resources->resources.end()) {
        existing->second = std::move(data);
    } else {
        resources->resources.emplace_back(name, std::move(data));
        mTotalFiles++;
    }
    mState = AppState::ChangesStaged;
}

void StageManager::AddFile(const std::filesystem::path& file, const std::string& path) {
    mRevision++;
    mEntries[path] = CustomStageEntry{ { file } };
    mTotalFiles++;
    mState = AppState::ChangesStaged;
}

void StageManager::RemoveFile(const std::filesystem::path& file, const std::string& path) {
    mRevision++;
    auto it = mEntries.find(path);
    if (it == mEntries.end()) {
        mToasts.Error("Cannot remove file from non-existent entry");
        return;
    }
    if (auto* custom = std::get_if<CustomStageEntry>(&it->second)) {
        std::erase(custom->files, file);
    } else if (auto* sequences = std::get_if<CustomSequencesEntry>(&it->second)) {
        std::erase_if(sequences->pairs, [&](const auto& pair) { return pair.first == file || pair.second == file; });
    } else if (auto* textures = std::get_if<CustomTexturesEntry>(&it->second)) {
        std::erase_if(textures->pairs, [&](const auto& pair) { return pair.first == file; });
    } else if (auto* resources = std::get_if<ResourceDataEntry>(&it->second)) {
        std::erase_if(resources->resources, [&](const auto& resource) { return file == resource.first; });
    }
    mTotalFiles = std::max(0, mTotalFiles - 1);

    if (StageEntryFiles(it->second).empty()) {
        mEntries.erase(it);
    }
    if (mEntries.empty()) {
        mState = AppState::None;
    }
}

void StageManager::GenerateOtr(std::function<void()> onCompletion) {
    if (mGenerating) {
        return;
    }
    const auto outputFile = FileDialog::SaveFile("generated.o2r", FileDialog::kArchiveFilters);
    if (!outputFile) {
        return;
    }

    mGenerating = true;
    mProgress = std::make_shared<std::atomic<int>>(0);
    auto progress = mProgress;
    auto entries = std::make_shared<std::map<std::string, StageEntry>>(mEntries);
    const bool alt = prependAlt;
    Toasts& toasts = mToasts;

    Jobs::Instance().Run(
        [entries, output = *outputFile, alt, progress, &toasts] {
            OtrGenerator::Generate(*entries, output, alt, *progress,
                                   [&toasts](const std::string& message) { toasts.Error(message); });
        },
        [this, onCompletion = std::move(onCompletion)] {
            mGenerating = false;
            Reset();
            if (onCompletion) {
                onCompletion();
            }
        });
}
