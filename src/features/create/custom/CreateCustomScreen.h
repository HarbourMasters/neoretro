#pragma once

#include "app/Screen.h"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class CreateCustomScreen : public Screen {
  public:
    void Layout(AppContext& ctx) override;

  private:
    struct State {
        std::string path;
        std::vector<std::filesystem::path> files;
        std::vector<std::string> relativeFiles;
        bool isProcessing = false;
    };

    void OnSelectFiles();

    std::shared_ptr<State> mState = std::make_shared<State>();
};
