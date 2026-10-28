#pragma once

#include "app/Screen.h"
#include "games/sm64/Sm64Text.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

enum class GhostshipTextKind { Dialogs, Texts, Strings };

class GhostshipTextScreen : public Screen {
  public:
    explicit GhostshipTextScreen(GhostshipTextKind kind) : mKind(kind) {
    }

    void Layout(AppContext& ctx) override;

  private:
    struct Item {
        std::string path;
        std::string name;
        std::string original;
        std::string text;
        Sm64::Dialog originalDialog;
        Sm64::Dialog dialog;
        std::string error;

        bool Modified() const;
        void Validate();
    };

    struct State {
        GhostshipTextKind kind;
        std::optional<std::string> archivePath;
        std::optional<std::string> modPath;
        std::optional<Sm64::Dictionary> strings;
        std::vector<Item> items;
        std::vector<int> filtered;
        int selected = -1;
        std::string query;
        bool isProcessing = false;
        bool loaded = false;
    };

    static std::vector<Item> BuildItems(GhostshipTextKind kind, const Sm64::TextAssets& assets);
    static void ApplyMod(State& state, const Sm64::TextAssets& mod);
    static void Filter(State& state);

    void OnSelectArchive();
    void OnLoadMod();
    void StageChanges(AppContext& ctx);
    void DrawEditor(float width, float height);

    GhostshipTextKind mKind;
    std::shared_ptr<State> mState = std::make_shared<State>();
};
