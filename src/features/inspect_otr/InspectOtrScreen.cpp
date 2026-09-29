#include "InspectOtrScreen.h"

#include "app/AppContext.h"
#include "app/Router.h"
#include "l10n/Strings.h"
#include "otr/Archive.h"
#include "ui/clay/ClayUi.h"
#include "ui/components/CustomScaffold.h"
#include "ui/components/Widgets.h"
#include "utils/FileDialog.h"
#include "utils/Jobs.h"

#include <algorithm>

namespace {
std::string ToLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return value;
}
} // namespace

void InspectOtrScreen::OnSelectOtr() {
    const auto selected = FileDialog::PickFile(FileDialog::kArchiveFilters);
    if (!selected) {
        return;
    }
    mState->selectedOtrPath = *selected;
    mState->isProcessing = true;

    auto files = std::make_shared<std::optional<std::vector<std::string>>>();
    Jobs::Instance().Run(
        [path = *selected, files] {
            if (auto archive = Otr::ArchiveReader::Open(path)) {
                *files = archive->List();
            }
        },
        [state = mState, path = *selected, files] {
            if (state->selectedOtrPath == path && files->has_value()) {
                state->filesInOtr = std::move(**files);
                std::sort(state->filesInOtr.begin(), state->filesInOtr.end());
                Filter(*state);
            }
            state->isProcessing = false;
        });
}

void InspectOtrScreen::Filter(State& state) {
    const std::string query = ToLower(state.query);
    state.filteredFilesInOtr.clear();
    for (const auto& file : state.filesInOtr) {
        if (ToLower(file).find(query) != std::string::npos) {
            state.filteredFilesInOtr.push_back(file);
        }
    }
}

void InspectOtrScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    ScaffoldOptions options;
    options.title = Tr("inspectOtrScreen_inspectOtr");
    options.subtitle = Tr("inspectOtrScreen_inspectOtrSubtitle");
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };
    options.topRightWidget = [&] {
        Widgets::SearchField(ui, "Inspect_Search", &mState->query, Tr("inspectOtrScreen_search"), 300,
                             [state = mState] { Filter(*state); });
    };

    CustomScaffold(ui, options, [&] {
        CLAY({ .id = CLAY_ID("Inspect_Body"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                           .padding = CLAY_PADDING_ALL(20),
                           .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
            CLAY({ .id = CLAY_ID("Inspect_Picker"),
                   .layout = { .sizing = { CLAY_SIZING_GROW(0) },
                               .childGap = 12,
                               .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
                Widgets::OutlinedField(ui, "Inspect_Path",
                                       mState->selectedOtrPath.value_or(Tr("inspectOtrScreen_noOtrSelected")));
                if (Widgets::Button(ui, "Inspect_Select", Tr("inspectOtrScreen_selectButton"),
                                    { CLAY_SIZING_FIT(100), CLAY_SIZING_FIXED(50) })) {
                    OnSelectOtr();
                }
            }
            if (mState->isProcessing) {
                Widgets::CenteredSpinner(ui, "Inspect_Spinner");
            } else if (!mState->filteredFilesInOtr.empty()) {
                CLAY({ .id = CLAY_ID("Inspect_ListPad"),
                       .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                                   .padding = { 0, 0, 10, 0 } } }) {
                    Widgets::TextList(ui, "Inspect_List", &mState->filteredFilesInOtr);
                }
            }
        }
    });
}
