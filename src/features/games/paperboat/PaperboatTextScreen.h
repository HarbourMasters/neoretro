#pragma once

#include "app/Screen.h"
#include "games/pm64/Pm64Text.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

class PaperboatTextScreen : public Screen {
  public:
    void Layout(AppContext& ctx) override;

  private:
    struct Item {
        std::string path;
        std::string name;
        Pm64::Charset charset = Pm64::Charset::Standard;
        std::string original;
        std::string text;
        std::string error;

        bool Modified() const;
        void Validate();
    };

    struct State {
        std::optional<std::string> archivePath;
        std::optional<std::string> modPath;
        std::vector<Item> items;
        std::vector<int> filtered;
        int selected = -1;
        std::string query;
        bool isProcessing = false;
        bool loaded = false;
    };

    static std::vector<Item> BuildItems(const Pm64::Messages& messages);
    static void ApplyMod(State& state, const Pm64::Messages& mod);
    static void Filter(State& state);

    void OnSelectArchive();
    void OnLoadMod();
    void StageChanges(AppContext& ctx);
    void DrawEditor(float width, float height);

    std::shared_ptr<State> mState = std::make_shared<State>();
};
