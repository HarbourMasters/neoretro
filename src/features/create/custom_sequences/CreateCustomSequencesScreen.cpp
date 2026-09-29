#include "CreateCustomSequencesScreen.h"

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
#include <spdlog/spdlog.h>

std::vector<CreateCustomSequencesScreen::SequenceMetaPair>
CreateCustomSequencesScreen::ListSequenceMetaPairs(const std::filesystem::path& folder) {
    std::vector<SequenceMetaPair> pairs;
    for (const auto& file : Paths::ListFilesRecursive(folder)) {
        if (file.extension() != ".seq") {
            continue;
        }
        auto meta = file;
        meta.replace_extension(".meta");
        if (!std::filesystem::exists(meta)) {
            SPDLOG_ERROR("Meta file not found for sequence file: {}! Skipping.", Paths::ToUtf8(file.filename()));
            continue;
        }
        pairs.emplace_back(file, meta);
    }
    return pairs;
}

void CreateCustomSequencesScreen::OnSelectFolder() {
    const auto selected = FileDialog::PickFolder();
    if (!selected) {
        return;
    }
    mState->selectedFolderPath = *selected;
    mState->isProcessing = true;

    auto result = std::make_shared<State>();
    result->selectedFolderPath = *selected;
    Jobs::Instance().Run(
        [result] {
            const std::string base = Paths::Normalize(*result->selectedFolderPath);
            result->sequenceMetaPairs = ListSequenceMetaPairs(Paths::FromUtf8(*result->selectedFolderPath));
            for (const auto& [seq, meta] : result->sequenceMetaPairs) {
                std::string name = Paths::ToPosix(seq);
                if (const auto at = name.find(base + "/"); at != std::string::npos) {
                    name = name.substr(at + base.size() + 1);
                }
                result->names.push_back(name.substr(0, name.find('.')));
            }
        },
        [state = mState, result] {
            if (state->selectedFolderPath == result->selectedFolderPath) {
                state->sequenceMetaPairs = std::move(result->sequenceMetaPairs);
                state->names = std::move(result->names);
            }
            state->isProcessing = false;
        });
}

void CreateCustomSequencesScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    ScaffoldOptions options;
    options.title = Tr("createCustomSequences_addCustomSequences");
    options.subtitle = Tr("createCustomSequences_addCustomSequencesDescription");
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };

    CustomScaffold(ui, options, [&] {
        CLAY({ .id = CLAY_ID("Sequences_Body"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                           .padding = CLAY_PADDING_ALL(20),
                           .childAlignment = { .x = CLAY_ALIGN_X_CENTER },
                           .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
            CLAY({ .id = CLAY_ID("Sequences_Picker"),
                   .layout = { .sizing = { CLAY_SIZING_GROW(0) },
                               .childGap = 12,
                               .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
                Widgets::OutlinedField(
                    ui, "Sequences_Path",
                    mState->selectedFolderPath.value_or(Tr("createCustomSequences_SequencesFolderPath")));
                if (Widgets::Button(ui, "Sequences_Select", Tr("createCustomSequences_selectButton"),
                                    { CLAY_SIZING_FIT(100), CLAY_SIZING_FIXED(50) })) {
                    OnSelectFolder();
                }
            }
            if (mState->isProcessing) {
                Widgets::CenteredSpinner(ui, "Sequences_Spinner");
            } else if (!mState->sequenceMetaPairs.empty()) {
                CLAY({ .id = CLAY_ID("Sequences_ListPad"),
                       .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                                   .padding = { 0, 0, 10, 0 } } }) {
                    Widgets::TextList(ui, "Sequences_List", &mState->names);
                }
            } else {
                Widgets::Spacer();
            }
            const float width = ImGui::GetMainViewport()->WorkSize.x * 0.5f;
            if (Widgets::Button(ui, "Sequences_Stage", Tr("createCustomSequences_stageFiles"),
                                { CLAY_SIZING_FIXED(width), CLAY_SIZING_FIXED(50) },
                                !mState->sequenceMetaPairs.empty())) {
                ctx.stage.AddCustomSequenceEntry(mState->sequenceMetaPairs, "custom/music");
                ctx.router.PopUntil("/create_selection");
            }
        }
    });
}
