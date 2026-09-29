#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <utility>
#include <vector>

struct AppContext;

class EphemeralBar {
  public:
    static constexpr float kHeight = 24.0f;

    void Layout(AppContext& ctx, float windowHeight);

  private:
    struct Row {
        std::string text;
        bool header = false;
        std::string key;
        std::filesystem::path file;
    };

    void Bar(AppContext& ctx, bool expanded);
    void FinishPanel(AppContext& ctx);
    void RebuildRows(AppContext& ctx);
    void Dismiss(AppContext& ctx);

    float mAnimation = 0.0f;
    uint64_t mRowsRevision = ~0ull;
    bool mRowsAlt = false;
    std::vector<Row> mRows;
    std::optional<std::pair<std::filesystem::path, std::string>> mPendingRemove;
};
