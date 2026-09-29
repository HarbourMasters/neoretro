#pragma once

#include "app/Screen.h"
#include "services/TextureReplacement.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

class CreateReplaceTexturesScreen : public Screen {
  public:
    void Layout(AppContext& ctx) override;

  private:
    enum class Step { Question, SelectFolder, SelectOtr };

    struct State {
        Step currentStep = Step::Question;
        std::optional<std::string> selectedFolderPath;
        std::vector<std::string> selectedOtrPaths;
        bool isProcessing = false;
        TextureReplacement::ProcessedFolder processedFolder;
        TextureReplacement::ExtractedTextures extracted;
        std::vector<std::string> rows;
    };

    void QuestionContent(AppContext& ctx);
    void FolderContent(AppContext& ctx);
    void OtrContent(AppContext& ctx);

    void OnSelectFolder();
    void OnSelectOtr();
    void OnProcessOtr(AppContext& ctx);
    static void RebuildFolderRows(State& state, bool prependAlt);

    std::shared_ptr<State> mState = std::make_shared<State>();
    bool mRowsAlt = false;
};
