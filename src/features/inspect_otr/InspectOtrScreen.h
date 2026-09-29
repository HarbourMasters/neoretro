#pragma once

#include "app/Screen.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

class InspectOtrScreen : public Screen {
  public:
    void Layout(AppContext& ctx) override;

  private:
    struct State {
        std::optional<std::string> selectedOtrPath;
        std::vector<std::string> filesInOtr;
        std::vector<std::string> filteredFilesInOtr;
        bool isProcessing = false;
        std::string query;
    };

    void OnSelectOtr();
    static void Filter(State& state);

    std::shared_ptr<State> mState = std::make_shared<State>();
};
