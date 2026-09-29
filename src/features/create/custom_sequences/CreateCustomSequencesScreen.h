#pragma once

#include "app/Screen.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

class CreateCustomSequencesScreen : public Screen {
  public:
    void Layout(AppContext& ctx) override;

    using SequenceMetaPair = std::pair<std::filesystem::path, std::filesystem::path>;
    static std::vector<SequenceMetaPair> ListSequenceMetaPairs(const std::filesystem::path& folder);

  private:
    struct State {
        std::optional<std::string> selectedFolderPath;
        std::vector<SequenceMetaPair> sequenceMetaPairs;
        std::vector<std::string> names;
        bool isProcessing = false;
    };

    void OnSelectFolder();

    std::shared_ptr<State> mState = std::make_shared<State>();
};
