#include "TextEditorScreen.h"

#include "app/AppContext.h"
#include "app/Router.h"
#include "l10n/Strings.h"
#include "otr/Archive.h"
#include "services/StageManager.h"
#include "ui/Toasts.h"
#include "ui/clay/ClayUi.h"
#include "ui/components/CustomScaffold.h"
#include "ui/components/Widgets.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"
#include "utils/FileDialog.h"
#include "utils/Jobs.h"

#include <imgui.h>

TextEditorScreen::TextEditorScreen(TextEditorScreenOptions options, std::shared_ptr<TextEditorGame> game)
    : mOptions(std::move(options)) {
    mState->game = std::move(game);
}

void TextEditorScreen::OnSelectArchive() {
    const auto selected = FileDialog::PickFile(FileDialog::kArchiveFilters);
    if (!selected) {
        return;
    }
    mState->archivePath = *selected;
    mState->isProcessing = true;

    auto apply = std::make_shared<TextEditorGame::Apply>();
    Jobs::Instance().Run(
        [game = mState->game, path = *selected, apply] {
            if (auto archive = Otr::ArchiveReader::Open(path)) {
                *apply = game->LoadArchive(*archive);
            }
        },
        [state = mState, path = *selected, apply] {
            if (state->archivePath == path) {
                state->list.entries.clear();
                if (*apply) {
                    (*apply)(state->list);
                }
                state->modPath.reset();
                state->list.selected = state->list.entries.empty() ? -1 : 0;
                state->loaded = true;
                state->list.Filter();
            }
            state->isProcessing = false;
        });
}

void TextEditorScreen::OnLoadMod() {
    const auto selected = FileDialog::PickFile(FileDialog::kArchiveFilters);
    if (!selected) {
        return;
    }
    mState->modPath = *selected;
    mState->isProcessing = true;

    auto apply = std::make_shared<TextEditorGame::Apply>();
    Jobs::Instance().Run(
        [game = mState->game, path = *selected, apply] {
            if (auto archive = Otr::ArchiveReader::Open(path)) {
                *apply = game->LoadMod(*archive);
            }
        },
        [state = mState, path = *selected, apply] {
            if (state->modPath == path && *apply) {
                (*apply)(state->list);
                for (int i = 0; i < (int)state->list.entries.size(); i++) {
                    state->game->Validate(state->list.entries[i], i);
                }
                state->list.Filter();
            }
            state->isProcessing = false;
        });
}

void TextEditorScreen::StageChanges(AppContext& ctx) {
    const auto& entries = mState->list.entries;
    std::vector<int> modified;
    for (int i = 0; i < (int)entries.size(); i++) {
        if (!entries[i].Modified()) {
            continue;
        }
        if (!entries[i].error.empty()) {
            ctx.toasts.Error(Tr("textEditorScreen_invalidText"));
            return;
        }
        modified.push_back(i);
    }
    if (modified.empty()) {
        ctx.toasts.Error(Tr("textEditorScreen_nothingChanged"));
        return;
    }
    mState->game->Stage(ctx.stage, mState->list, modified);
}

void TextEditorScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    const std::string& id = mOptions.id;
    ScaffoldOptions options;
    options.title = Tr(mOptions.titleKey);
    options.subtitle = Tr(mOptions.subtitleKey);
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };
    if (mState->loaded) {
        options.topRightWidget = [&] {
            Widgets::SearchField(ui, id + "_Search", &mState->list.query, Tr("textEditorScreen_search"), 300,
                                 [state = mState] { state->list.Filter(); });
        };
    }

    TextEntryEditorOptions editorOptions;
    editorOptions.id = id;
    editorOptions.hint = Tr(mOptions.hintKey);
    editorOptions.validate = [game = mState->game](TextEntry& entry, int index) { game->Validate(entry, index); };
    editorOptions.revert = [game = mState->game](TextEntry& entry, int index) { game->Revert(entry, index); };
    editorOptions.drawFields = [game = mState->game](TextEntry& entry, int index) { game->DrawFields(entry, index); };
    editorOptions.preview = [game = mState->game](const std::string& text) { return game->Preview(text); };
    const ClayUi::WidgetFn editor = [this, editorOptions](ImVec2 min, ImVec2 max) {
        TextEntryEditor(mState->list, editorOptions, max.x - min.x, max.y - min.y);
    };

    CustomScaffold(ui, options, [&] {
        CLAY({ .id = ClayUi::Id(id + "_Body"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                           .padding = CLAY_PADDING_ALL(20),
                           .childGap = 10,
                           .childAlignment = { .x = CLAY_ALIGN_X_CENTER },
                           .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
            CLAY({ .id = ClayUi::Id(id + "_Picker"),
                   .layout = { .sizing = { CLAY_SIZING_GROW(0) },
                               .childGap = 12,
                               .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
                Widgets::OutlinedField(ui, id + "_Path", mState->archivePath.value_or(Tr(mOptions.archivePathKey)));
                if (Widgets::Button(ui, id + "_Select", Tr("textEditorScreen_selectButton"),
                                    { CLAY_SIZING_FIT(100), CLAY_SIZING_FIXED(50) }, !mState->isProcessing)) {
                    OnSelectArchive();
                }
                if (Widgets::Button(ui, id + "_LoadMod", Tr("textEditorScreen_loadModButton"),
                                    { CLAY_SIZING_FIT(100), CLAY_SIZING_FIXED(50) },
                                    mState->loaded && !mState->list.entries.empty() && !mState->isProcessing,
                                    Widgets::OutlinedStyle())) {
                    OnLoadMod();
                }
            }
            if (mState->isProcessing) {
                Widgets::CenteredSpinner(ui, id + "_Spinner");
            } else if (mState->loaded && mState->list.entries.empty()) {
                CLAY({ .id = ClayUi::Id(id + "_Empty"),
                       .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                                   .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } } }) {
                    Widgets::Text(ui, Tr(mOptions.noTextKey), TextSize::BodyLarge, RetroColors::Grey);
                }
            } else if (mState->loaded) {
                CLAY({ .id = ClayUi::Id(id + "_Editor"),
                       .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } },
                       .custom = ui.Widget(editor) }) {
                }
            } else {
                Widgets::Spacer();
            }
            const float width = ImGui::GetMainViewport()->WorkSize.x * 0.5f;
            if (Widgets::Button(ui, id + "_Stage", Tr("textEditorScreen_stageChanges"),
                                { CLAY_SIZING_FIXED(width), CLAY_SIZING_FIXED(50) },
                                mState->list.HasChanges() && !mState->isProcessing)) {
                StageChanges(ctx);
            }
        }
    });
}
