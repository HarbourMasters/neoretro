#pragma once

#include "models/StageEntry.h"

#include <atomic>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Toasts;

class StageManager {
  public:
    enum class AppState { None, ChangesStaged };

    explicit StageManager(Toasts& toasts) : mToasts(toasts) {
    }

    const std::map<std::string, StageEntry>& Entries() const {
        return mEntries;
    }
    bool HasStagedFiles() const {
        return !mEntries.empty();
    }
    std::string DisplayState() const;

    bool isEphemeralBarExpanded = false;
    bool prependAlt = false;

    void ToggleEphemeralBar() {
        isEphemeralBarExpanded = !isEphemeralBarExpanded;
    }
    void Reset();

    void AddCustomStageEntries(const std::vector<std::filesystem::path>& files, const std::filesystem::path& basePath);
    void AddCustomSequenceEntry(const std::vector<std::pair<std::filesystem::path, std::filesystem::path>>& pairs,
                                const std::string& path);
    void AddCustomTextureEntry(const std::map<std::string, std::vector<TexturePair>>& replacementMap);
    void AddFile(const std::filesystem::path& file, const std::string& path);
    void RemoveFile(const std::filesystem::path& file, const std::string& path);

    void GenerateOtr(std::function<void()> onCompletion);

    bool IsGenerating() const {
        return mGenerating;
    }
    int TotalFiles() const {
        return mTotalFiles;
    }
    int FilesProcessed() const {
        return mProgress ? mProgress->load() : 0;
    }
    uint64_t Revision() const {
        return mRevision;
    }

  private:
    Toasts& mToasts;
    AppState mState = AppState::None;
    std::map<std::string, StageEntry> mEntries;
    int mTotalFiles = 0;
    bool mGenerating = false;
    std::shared_ptr<std::atomic<int>> mProgress;
    uint64_t mRevision = 0;
};
