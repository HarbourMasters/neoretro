#include "CreateCustomScreen.h"

#include "app/AppContext.h"
#include "app/Router.h"
#include "l10n/Strings.h"
#include "services/StageManager.h"
#include "ui/clay/ClayUi.h"
#include "ui/components/CustomScaffold.h"
#include "ui/components/Widgets.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"
#include "utils/FileDialog.h"
#include "utils/Jobs.h"
#include "utils/Paths.h"

#include <imgui.h>

void CreateCustomScreen::OnSelectFiles() {
    const auto selected = FileDialog::PickFolder();
    mState->path = selected.value_or("");
    mState->files.clear();
    mState->relativeFiles.clear();
    if (!selected) {
        return;
    }

    mState->isProcessing = true;
    auto result = std::make_shared<State>();
    result->path = *selected;
    Jobs::Instance().Run(
        [result] {
            result->files = Paths::ListFilesRecursive(Paths::FromUtf8(result->path));
            for (const auto& file : result->files) {
                result->relativeFiles.push_back(Paths::Relative(file, Paths::FromUtf8(result->path)));
            }
        },
        [state = mState, result] {
            if (state->path == result->path) {
                state->files = std::move(result->files);
                state->relativeFiles = std::move(result->relativeFiles);
            }
            state->isProcessing = false;
        });
}

void CreateCustomScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    ScaffoldOptions options;
    options.title = Tr("createCustomScreen_title");
    options.subtitle = Tr("createCustomScreen_subtitle");
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };

    CustomScaffold(ui, options, [&] {
        CLAY({ .id = CLAY_ID("CreateCustom_Body"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                           .padding = CLAY_PADDING_ALL(20),
                           .childAlignment = { .x = CLAY_ALIGN_X_CENTER },
                           .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
            CLAY({ .id = CLAY_ID("CreateCustom_Picker"),
                   .layout = { .sizing = { CLAY_SIZING_GROW(0) },
                               .padding = { 0, 0, 12, 12 },
                               .childGap = 12,
                               .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
                if (Widgets::Button(ui, "CreateCustom_Select", Tr("createCustomScreen_selectButton"),
                                    { CLAY_SIZING_FIXED(200), CLAY_SIZING_FIXED(50) }, true,
                                    Widgets::OutlinedStyle())) {
                    OnSelectFiles();
                }
                Widgets::Text(ui, mState->path, TextSize::BodyMedium, RetroColors::White);
            }
            CLAY({ .id = CLAY_ID("CreateCustom_Count"),
                   .layout = { .sizing = { CLAY_SIZING_GROW(0) }, .padding = { 0, 0, 0, 4 } } }) {
                Widgets::Text(ui,
                              std::string(Tr("createCustomScreen_fileToInsert")) + std::to_string(mState->files.size()),
                              16, RetroColors::White, FONT_MEDIUM);
            }
            if (mState->isProcessing) {
                Widgets::CenteredSpinner(ui, "CreateCustom_Spinner");
            } else {
                Widgets::TextList(ui, "CreateCustom_Files", &mState->relativeFiles);
            }
            const float width = ImGui::GetMainViewport()->WorkSize.x * 0.5f;
            if (Widgets::Button(ui, "CreateCustom_Stage", Tr("createCustomScreen_stageFiles"),
                                { CLAY_SIZING_FIXED(width), CLAY_SIZING_FIXED(50) },
                                !mState->files.empty() && !mState->path.empty())) {
                ctx.stage.AddCustomStageEntries(mState->files, Paths::FromUtf8(mState->path));
                ctx.router.PopUntil("/create_selection");
            }
        }
    });
}
