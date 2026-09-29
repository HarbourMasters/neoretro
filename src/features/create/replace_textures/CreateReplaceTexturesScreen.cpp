#include "CreateReplaceTexturesScreen.h"

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
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace {
Clay_Sizing HalfWidthButton() {
    return { CLAY_SIZING_FIXED(ImGui::GetMainViewport()->WorkSize.x * 0.5f), CLAY_SIZING_FIXED(50) };
}
} // namespace

void CreateReplaceTexturesScreen::Layout(AppContext& ctx) {
    ScaffoldOptions options;
    options.title = Tr("createReplaceTexturesScreen_Option");
    options.subtitle = Tr("createReplaceTexturesScreen_OptionDescription");
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };

    CustomScaffold(ctx.ui, options, [&] {
        CLAY({ .id = CLAY_ID("Replace_Body"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                           .padding = CLAY_PADDING_ALL(20),
                           .childAlignment = { .x = CLAY_ALIGN_X_CENTER },
                           .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
            switch (mState->currentStep) {
                case Step::Question:
                    QuestionContent(ctx);
                    break;
                case Step::SelectFolder:
                    FolderContent(ctx);
                    break;
                case Step::SelectOtr:
                    OtrContent(ctx);
                    break;
            }
        }
    });
}

void CreateReplaceTexturesScreen::QuestionContent(AppContext& ctx) {
    auto& ui = ctx.ui;
    CLAY({ .id = CLAY_ID("Replace_Question"),
           .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                       .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
                       .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
        Widgets::Text(ui, Tr("questionContentView_mainQuestion"), TextSize::TitleLarge, RetroColors::White,
                      FONT_MEDIUM);
        CLAY({ .id = CLAY_ID("Replace_QuestionText"),
               .layout = { .sizing = { CLAY_SIZING_FIXED(500) }, .childAlignment = { .x = CLAY_ALIGN_X_CENTER } } }) {
            Widgets::Text(ui, Tr("questionContentView_mainText"), 12, RetroColors::Grey, FONT_REGULAR,
                          CLAY_TEXT_ALIGN_CENTER);
        }
        Widgets::Gap(0, 20);
        CLAY({ .id = CLAY_ID("Replace_Answers"), .layout = { .childGap = 20 } }) {
            if (Widgets::Button(ui, "Replace_Yes", Tr("questionContentView_yes"),
                                { CLAY_SIZING_FIT(64), CLAY_SIZING_FIXED(36) })) {
                mState->currentStep = Step::SelectFolder;
            }
            if (Widgets::Button(ui, "Replace_No", Tr("questionContentView_no"),
                                { CLAY_SIZING_FIT(64), CLAY_SIZING_FIXED(36) })) {
                mState->currentStep = Step::SelectOtr;
            }
        }
    }
}

void CreateReplaceTexturesScreen::RebuildFolderRows(State& state, bool prependAlt) {
    state.rows.clear();
    for (const auto& [key, pairs] : state.processedFolder) {
        state.rows.push_back(fmt::format("{}{} ({} tex)", prependAlt ? "alt/" : "", key, pairs.size()));
    }
}

void CreateReplaceTexturesScreen::OnSelectFolder() {
    const auto selected = FileDialog::PickFolder();
    if (!selected) {
        return;
    }
    mState->selectedFolderPath = Paths::Normalize(*selected);
    mState->isProcessing = true;

    auto result = std::make_shared<std::optional<TextureReplacement::ProcessedFolder>>();
    const std::string folder = *mState->selectedFolderPath;
    Jobs::Instance().Run([folder, result] { *result = TextureReplacement::ProcessFolder(Paths::FromUtf8(folder)); },
                         [state = mState, folder, result] {
                             if (!result->has_value()) {
                                 SPDLOG_ERROR("Error processing folder: {}", folder);
                             } else if (state->selectedFolderPath == folder) {
                                 state->processedFolder = std::move(**result);
                             }
                             state->isProcessing = false;
                             state->rows.clear();
                         });
}

void CreateReplaceTexturesScreen::FolderContent(AppContext& ctx) {
    auto& ui = ctx.ui;
    CLAY({ .id = CLAY_ID("Replace_FolderPicker"),
           .layout = {
               .sizing = { CLAY_SIZING_GROW(0) }, .childGap = 12, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
        Widgets::OutlinedField(ui, "Replace_FolderPath",
                               mState->selectedFolderPath.value_or(Tr("folderContentView_customTexturePath")));
        if (Widgets::Button(ui, "Replace_FolderSelect", Tr("folderContentView_selectButton"),
                            { CLAY_SIZING_FIT(100), CLAY_SIZING_FIXED(50) })) {
            OnSelectFolder();
        }
    }
    Widgets::Gap(0, 10);
    CLAY({ .id = CLAY_ID("Replace_Alt"),
           .layout = {
               .sizing = { CLAY_SIZING_GROW(0) }, .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
        if (Widgets::Switch(ui, "Replace_AltSwitch", ctx.stage.prependAlt)) {
            ctx.stage.prependAlt = !ctx.stage.prependAlt;
        }
        Widgets::Text(ui, Tr("folderContentView_prependAltToggle"), TextSize::BodyMedium, RetroColors::White);
    }

    const bool hasFiles = !mState->processedFolder.empty();
    if (!hasFiles && !mState->isProcessing) {
        Widgets::Spacer();
    } else {
        CLAY({ .id = CLAY_ID("Replace_FolderResults"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) }, .padding = { 0, 0, 20, 0 } } }) {
            if (mState->isProcessing) {
                Widgets::CenteredSpinner(ui, "Replace_FolderSpinner");
            } else {
                if (mState->rows.empty() || mRowsAlt != ctx.stage.prependAlt) {
                    mRowsAlt = ctx.stage.prependAlt;
                    RebuildFolderRows(*mState, mRowsAlt);
                }
                Widgets::TextList(ui, "Replace_FolderList", &mState->rows);
            }
        }
    }

    Widgets::Gap(0, 20);
    if (Widgets::Button(ui, "Replace_Stage", Tr("folderContentView_stageTextures"), HalfWidthButton(), hasFiles)) {
        ctx.stage.AddCustomTextureEntry(mState->processedFolder);
        ctx.router.PopUntil("/create_selection");
    }
}

void CreateReplaceTexturesScreen::OnSelectOtr() {
    const auto selected = FileDialog::PickFiles(FileDialog::kArchiveFilters);
    if (!selected.empty()) {
        mState->selectedOtrPaths = selected;
    }
}

void CreateReplaceTexturesScreen::OnProcessOtr(AppContext& ctx) {
    if (mState->selectedOtrPaths.empty()) {
        return;
    }
    const auto selectedDirectory = FileDialog::PickFolder();
    if (!selectedDirectory) {
        return;
    }
    mState->isProcessing = true;

    auto result = std::make_shared<TextureReplacement::ExtractedTextures>();
    const auto otrPaths = mState->selectedOtrPaths;
    const std::filesystem::path outputDir = Paths::FromUtf8(*selectedDirectory);
    const std::filesystem::path assetsDir = ctx.assetsDir;
    Jobs::Instance().Run(
        [otrPaths, outputDir, assetsDir, result] {
            if (auto extracted = TextureReplacement::ProcessOtr(otrPaths, outputDir)) {
                *result = std::move(*extracted);
            }
            const auto folder = outputDir / TextureReplacement::OutputFolderName(otrPaths.front());
            if (auto font = TextureReplacement::DumpFont(folder, assetsDir)) {
                (*result)[TextureReplacement::kFontTextureName] = *font;
            }
            TextureReplacement::WriteManifest(folder / "manifest.json", *result);
        },
        [state = mState, result] {
            state->extracted = std::move(*result);
            state->rows.clear();
            for (const auto& [name, entry] : state->extracted) {
                state->rows.push_back(name);
            }
            state->isProcessing = false;
        });
}

void CreateReplaceTexturesScreen::OtrContent(AppContext& ctx) {
    auto& ui = ctx.ui;
    std::string label = Tr("otrContentView_otrPath");
    if (!mState->selectedOtrPaths.empty()) {
        label.clear();
        for (const auto& path : mState->selectedOtrPaths) {
            label += (label.empty() ? "" : ", ") + Paths::Basename(path);
        }
    }

    CLAY({ .id = CLAY_ID("Replace_OtrPicker"),
           .layout = {
               .sizing = { CLAY_SIZING_GROW(0) }, .childGap = 12, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
        Widgets::OutlinedField(ui, "Replace_OtrPath", label);
        if (Widgets::Button(ui, "Replace_OtrSelect", Tr("otrContentView_otrSelect"),
                            { CLAY_SIZING_FIT(100), CLAY_SIZING_FIXED(50) })) {
            OnSelectOtr();
        }
    }

    const bool hasFiles = !mState->extracted.empty();
    if (!hasFiles && !mState->isProcessing) {
        CLAY({ .id = CLAY_ID("Replace_Details"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                           .padding = { 0, 0, 20, 0 },
                           .childAlignment = { .x = CLAY_ALIGN_X_CENTER },
                           .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
            Widgets::Text(ui, Tr("otrContentView_details"), TextSize::HeadlineSmall, RetroColors::White);
            for (const char* step : { "otrContentView_step1", "otrContentView_step2", "otrContentView_step3",
                                      "otrContentView_step4", "otrContentView_step5" }) {
                Widgets::Text(ui, Tr(step), TextSize::BodyMedium, RetroColors::White);
            }
        }
    } else {
        CLAY({ .id = CLAY_ID("Replace_OtrResults"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) }, .padding = { 0, 0, 20, 0 } } }) {
            if (mState->isProcessing) {
                Widgets::CenteredSpinner(ui, "Replace_OtrSpinner");
            } else {
                Widgets::TextList(ui, "Replace_OtrList", &mState->rows);
            }
        }
    }

    Widgets::Gap(0, 20);
    const std::string buttonText = mState->isProcessing ? Tr("otrContentView_processing")
                                   : hasFiles           ? fmt::format("Extracted {} Textures", mState->extracted.size())
                                                        : std::string("Process");
    const bool enabled = !mState->selectedOtrPaths.empty() && !mState->isProcessing && !hasFiles;
    if (Widgets::Button(ui, "Replace_Process", buttonText, HalfWidthButton(), enabled)) {
        OnProcessOtr(ctx);
    }
}
