#pragma once

#include "app/Screen.h"
#include "ui/components/TextEntryEditor.h"

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Otr {
class ArchiveReader;
}

class StageManager;

class TextEditorGame {
  public:
    using Apply = std::function<void(TextEntryList& list)>;

    virtual ~TextEditorGame() = default;

    virtual Apply LoadArchive(Otr::ArchiveReader& archive) = 0;
    virtual Apply LoadMod(Otr::ArchiveReader& archive) = 0;
    virtual void Validate(TextEntry& entry, int index) = 0;
    virtual void Stage(StageManager& stage, const TextEntryList& list, const std::vector<int>& modified) = 0;

    virtual void Revert(TextEntry& entry, int index) {
    }
    virtual void DrawFields(TextEntry& entry, int index) {
    }
    virtual std::string Preview(const std::string& text) const {
        return TextEntryPreview(text);
    }
};

struct TextEditorScreenOptions {
    std::string id;
    const char* titleKey;
    const char* subtitleKey;
    const char* archivePathKey;
    const char* noTextKey;
    const char* hintKey;
};

class TextEditorScreen : public Screen {
  public:
    TextEditorScreen(TextEditorScreenOptions options, std::shared_ptr<TextEditorGame> game);

    void Layout(AppContext& ctx) override;

  private:
    struct State {
        std::shared_ptr<TextEditorGame> game;
        TextEntryList list;
        std::optional<std::string> archivePath;
        std::optional<std::string> modPath;
        bool isProcessing = false;
        bool loaded = false;
    };

    void OnSelectArchive();
    void OnLoadMod();
    void StageChanges(AppContext& ctx);

    TextEditorScreenOptions mOptions;
    std::shared_ptr<State> mState = std::make_shared<State>();
};
