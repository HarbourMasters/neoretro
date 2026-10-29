#include "ImageEditorScreen.h"

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
#include "utils/Paths.h"

#include <atomic>
#include <imgui.h>
#include <spdlog/fmt/fmt.h>

namespace {
void Notify(Toasts& toasts, std::string text) {
    toasts.Post({ std::move(text), RetroColors::Cream, RetroColors::Black, Icons::Check, RetroColors::Green, 3.0f });
}

bool SamePixels(const Image& a, const Image& b) {
    return a.width == b.width && a.height == b.height && a.rgba == b.rgba;
}
} // namespace

ImageEditorScreen::ImageEditorScreen(ImageEditorScreenOptions options, std::shared_ptr<ImageEditorGame> game)
    : mOptions(std::move(options)) {
    mState->game = std::move(game);
}

void ImageEditorScreen::OnSelectArchive() {
    const auto selected = FileDialog::PickFile(FileDialog::kArchiveFilters);
    if (!selected) {
        return;
    }
    mState->archivePath = *selected;
    mState->isProcessing = true;

    auto apply = std::make_shared<ImageEditorGame::Apply>();
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
                state->list.selected = state->list.entries.empty() ? -1 : 0;
                state->editor.shownIndex = -1;
                state->editor.error.clear();
                state->loaded = true;
                state->list.Filter();
            }
            state->isProcessing = false;
        });
}

void ImageEditorScreen::ExportOne(AppContext& ctx, int index) {
    const auto& entry = mState->list.entries[index];
    const auto path = FileDialog::SaveFile(Paths::Basename(entry.path) + ".png", FileDialog::kPngFilters);
    if (!path) {
        return;
    }
    if (!Paths::WriteFile(Paths::FromUtf8(*path), ImageIO::EncodePng(entry.Current()))) {
        ctx.toasts.Error(Tr("imageEditorScreen_exportFailed"));
    }
}

void ImageEditorScreen::ImportOne(AppContext& ctx, int index) {
    const auto path = FileDialog::PickFile(FileDialog::kPngFilters);
    if (!path) {
        return;
    }
    const auto bytes = Paths::ReadFile(Paths::FromUtf8(*path));
    const auto image = bytes ? ImageIO::DecodePng(*bytes) : std::nullopt;
    auto& editor = mState->editor;
    if (!image) {
        editor.error = Tr("imageEditorScreen_unreadable");
        return;
    }
    if (const auto error = mState->game->Import(mState->list.entries[index], index, ImageOps::Convert(*image, 4))) {
        editor.error = *error;
        return;
    }
    editor.error.clear();
    editor.showOriginal = false;
    editor.shownIndex = -1;
}

void ImageEditorScreen::OnExportAll(AppContext& ctx) {
    const auto folder = FileDialog::PickFolder();
    if (!folder) {
        return;
    }
    mState->isProcessing = true;
    auto written = std::make_shared<std::atomic<int>>(0);
    Jobs::Instance().Run(
        [state = mState, root = Paths::FromUtf8(*folder), written] {
            for (const auto& entry : state->list.entries) {
                if (Paths::WriteFile(root / Paths::FromUtf8(entry.path + ".png"), ImageIO::EncodePng(entry.Current()))) {
                    (*written)++;
                }
            }
        },
        [state = mState, &toasts = ctx.toasts, folder = *folder, written] {
            state->isProcessing = false;
            Notify(toasts, fmt::format(fmt::runtime(Tr("imageEditorScreen_exported")), written->load(), folder));
        });
}

void ImageEditorScreen::OnImportFolder(AppContext& ctx) {
    const auto folder = FileDialog::PickFolder();
    if (!folder) {
        return;
    }
    mState->isProcessing = true;

    struct Outcome {
        int imported = 0;
        std::vector<std::string> failures;
        std::vector<std::pair<int, ImageEntry>> updates;
    };
    auto outcome = std::make_shared<Outcome>();
    Jobs::Instance().Run(
        [state = mState, root = Paths::FromUtf8(*folder), outcome] {
            for (int i = 0; i < (int)state->list.entries.size(); i++) {
                const auto& entry = state->list.entries[i];
                const auto bytes = Paths::ReadFile(root / Paths::FromUtf8(entry.path + ".png"));
                if (!bytes) {
                    continue;
                }
                auto image = ImageIO::DecodePng(*bytes);
                if (!image) {
                    outcome->failures.push_back(entry.path + ": " + Tr("imageEditorScreen_unreadable"));
                    continue;
                }
                const Image rgba = ImageOps::Convert(*image, 4);
                if (SamePixels(rgba, entry.Current())) {
                    continue;
                }
                ImageEntry updated = entry;
                if (const auto error = state->game->Import(updated, i, rgba)) {
                    outcome->failures.push_back(entry.path + ": " + *error);
                    continue;
                }
                outcome->updates.emplace_back(i, std::move(updated));
            }
        },
        [state = mState, &toasts = ctx.toasts, outcome] {
            for (auto& [index, entry] : outcome->updates) {
                state->list.entries[index] = std::move(entry);
            }
            state->editor.shownIndex = -1;
            state->editor.error = outcome->failures.empty() ? "" : outcome->failures.front();
            state->isProcessing = false;
            if (!outcome->failures.empty()) {
                toasts.Error(fmt::format(fmt::runtime(Tr("imageEditorScreen_importFailures")),
                                         outcome->failures.size()));
            }
            Notify(toasts, fmt::format(fmt::runtime(Tr("imageEditorScreen_imported")), outcome->updates.size()));
        });
}

void ImageEditorScreen::StageChanges(AppContext& ctx) {
    std::vector<int> modified;
    for (int i = 0; i < (int)mState->list.entries.size(); i++) {
        if (mState->list.entries[i].Modified()) {
            modified.push_back(i);
        }
    }
    if (modified.empty()) {
        ctx.toasts.Error(Tr("imageEditorScreen_nothingChanged"));
        return;
    }
    mState->game->Stage(ctx.stage, mState->list, modified);
}

void ImageEditorScreen::Layout(AppContext& ctx) {
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

    ImageEntryEditorOptions editorOptions;
    editorOptions.id = id;
    editorOptions.exportPng = [this, &ctx](int index) { ExportOne(ctx, index); };
    editorOptions.importPng = [this, &ctx](int index) { ImportOne(ctx, index); };
    editorOptions.revert = [state = mState](int index) {
        auto& entry = state->list.entries[index];
        entry.edited.reset();
        entry.note.clear();
        state->editor.error.clear();
        state->editor.shownIndex = -1;
    };
    const ClayUi::WidgetFn editor = [this, editorOptions](ImVec2 min, ImVec2 max) {
        ImageEntryEditor(mState->list, mState->editor, editorOptions, max.x - min.x, max.y - min.y);
    };

    CustomScaffold(ui, options, [&] {
        CLAY({ .id = ClayUi::Id(id + "_Body"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                           .padding = CLAY_PADDING_ALL(20),
                           .childGap = 10,
                           .childAlignment = { .x = CLAY_ALIGN_X_CENTER },
                           .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
            const bool ready = mState->loaded && !mState->list.entries.empty() && !mState->isProcessing;
            CLAY({ .id = ClayUi::Id(id + "_Picker"),
                   .layout = { .sizing = { CLAY_SIZING_GROW(0) },
                               .childGap = 12,
                               .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
                Widgets::OutlinedField(ui, id + "_Path", mState->archivePath.value_or(Tr(mOptions.archivePathKey)));
                if (Widgets::Button(ui, id + "_Select", Tr("textEditorScreen_selectButton"),
                                    { CLAY_SIZING_FIT(100), CLAY_SIZING_FIXED(50) }, !mState->isProcessing)) {
                    OnSelectArchive();
                }
                if (Widgets::Button(ui, id + "_ExportAll", Tr("imageEditorScreen_exportAll"),
                                    { CLAY_SIZING_FIT(100), CLAY_SIZING_FIXED(50) }, ready,
                                    Widgets::OutlinedStyle())) {
                    OnExportAll(ctx);
                }
                if (Widgets::Button(ui, id + "_ImportFolder", Tr("imageEditorScreen_importFolder"),
                                    { CLAY_SIZING_FIT(100), CLAY_SIZING_FIXED(50) }, ready,
                                    Widgets::OutlinedStyle())) {
                    OnImportFolder(ctx);
                }
            }
            if (mState->isProcessing) {
                Widgets::CenteredSpinner(ui, id + "_Spinner");
            } else if (mState->loaded && mState->list.entries.empty()) {
                CLAY({ .id = ClayUi::Id(id + "_Empty"),
                       .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                                   .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } } }) {
                    Widgets::Text(ui, Tr(mOptions.noImagesKey), TextSize::BodyLarge, RetroColors::Grey);
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
