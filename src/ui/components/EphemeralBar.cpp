#include "EphemeralBar.h"

#include "CustomScaffold.h"
#include "Widgets.h"
#include "app/AppContext.h"
#include "l10n/Strings.h"
#include "services/StageManager.h"
#include "ui/clay/ClayUi.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"
#include "utils/BuildInfo.h"
#include "utils/Platform.h"
#include "utils/Paths.h"

#include <algorithm>
#include <imgui.h>
#include <imgui_internal.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

namespace {
constexpr float kAnimationSeconds = 0.5f;

float FastOutSlowIn(float t) {
    float x = t;
    for (int i = 0; i < 6; i++) {
        const float cx = 3 * 0.4f * x * (1 - x) * (1 - x) + 3 * 0.2f * x * x * (1 - x) + x * x * x - t;
        const float dx = 3 * 0.4f * (1 - x) * (1 - 3 * x) + 3 * 0.2f * x * (2 - 3 * x) + 3 * x * x;
        if (std::abs(dx) < 1e-5f) {
            break;
        }
        x = std::clamp(x - cx / dx, 0.0f, 1.0f);
    }
    return 3 * x * x * (1 - x) + x * x * x;
}
} // namespace

void EphemeralBar::Layout(AppContext& ctx, float windowHeight) {
    if (mPendingRemove) {
        ctx.stage.RemoveFile(mPendingRemove->first, mPendingRemove->second);
        mPendingRemove.reset();
    }

    const float step = ImGui::GetIO().DeltaTime / kAnimationSeconds;
    mAnimation = std::clamp(mAnimation + (ctx.stage.isEphemeralBarExpanded ? step : -step), 0.0f, 1.0f);
    const float height = std::max(windowHeight * FastOutSlowIn(mAnimation), kHeight);
    const bool expanded = ctx.stage.isEphemeralBarExpanded;

    CLAY({ .id = CLAY_ID("EphemeralBar_Container"),
           .layout = { .sizing = { CLAY_SIZING_FIXED(ImGui::GetMainViewport()->WorkSize.x), CLAY_SIZING_FIXED(height) },
                       .layoutDirection = CLAY_TOP_TO_BOTTOM },
           .backgroundColor = mAnimation > 0.0f ? RetroColors::BigStone : RetroColors::Transparent,
           .floating = { .zIndex = 50,
                         .attachPoints = { CLAY_ATTACH_POINT_LEFT_BOTTOM, CLAY_ATTACH_POINT_LEFT_BOTTOM },
                         .attachTo = CLAY_ATTACH_TO_ROOT },
           .clip = { .vertical = true } }) {
        Bar(ctx, expanded);
        if (mAnimation > 0.0f) {
            FinishPanel(ctx);
        }
    }
}

void EphemeralBar::Bar(AppContext& ctx, bool expanded) {
    auto& ui = ctx.ui;
    const bool hasStagedFiles = ctx.stage.HasStagedFiles();
    const Clay_Color background =
        expanded ? RetroColors::BlueBayoux : (hasStagedFiles ? RetroColors::Green : RetroColors::BlueAccent);

    CLAY({ .id = CLAY_ID("EphemeralBar"),
           .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(kHeight) },
                       .padding = { 12, 12, 0, 0 },
                       .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } },
           .backgroundColor = background }) {
        CLAY({ .id = CLAY_ID("EphemeralBar_State"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0) },
                           .childGap = 4,
                           .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
            Widgets::Text(ui, Icons::Gamepad, 14, RetroColors::White, FONT_ICONS);
            Widgets::Text(ui, "state/" + ctx.stage.DisplayState(), TextSize::BodyMedium, RetroColors::White);
        }

        if (hasStagedFiles && !expanded) {
            const Clay_ElementId finalizeId = CLAY_ID("EphemeralBar_Finalize");
            const bool hovered = ui.Hovered(finalizeId);
            if (hovered) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            }
            CLAY({ .id = finalizeId,
                   .layout = { .sizing = { CLAY_SIZING_FIT(0), CLAY_SIZING_GROW(0) },
                               .padding = { 8, 8, 0, 2 },
                               .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } },
                   .backgroundColor =
                       hovered ? RetroColors::WithOpacity(RetroColors::White, 0.15f) : RetroColors::Transparent }) {
                Widgets::Text(ui, Tr("components_ephemeralBar_finalizeOtr"), TextSize::BodyMedium, RetroColors::White,
                              FONT_BOLD);
            }
            if (ui.Clicked(finalizeId)) {
                ctx.stage.ToggleEphemeralBar();
            }
        }

        CLAY({ .id = CLAY_ID("EphemeralBar_Version"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                           .childGap = 4,
                           .childAlignment = { CLAY_ALIGN_X_RIGHT, CLAY_ALIGN_Y_CENTER } } }) {
            Widgets::Text(ui,
                          fmt::format("neoretro: {}.{}.{}", gBuildVersionMajor, gBuildVersionMinor, gBuildVersionPatch),
                          TextSize::BodyMedium, RetroColors::White);
            if (!expanded && gRepositoryUrl[0] != '\0' &&
                Widgets::IconButton(ui, "EphemeralBar_Repository", Icons::Memory, 14, RetroColors::White, 5)) {
                Platform::OpenUrl(gRepositoryUrl);
            }
        }
    }
}

void EphemeralBar::RebuildRows(AppContext& ctx) {
    if (mRowsRevision == ctx.stage.Revision() && mRowsAlt == ctx.stage.prependAlt) {
        return;
    }
    mRowsRevision = ctx.stage.Revision();
    mRowsAlt = ctx.stage.prependAlt;
    mRows.clear();
    for (const auto& [key, entry] : ctx.stage.Entries()) {
        const bool shouldPrependAlt = std::holds_alternative<CustomTexturesEntry>(entry) && ctx.stage.prependAlt;
        mRows.push_back({ (shouldPrependAlt ? "alt/" : "") + key, true, key, {} });
        for (const auto& file : StageEntryFiles(entry)) {
            mRows.push_back({ "• " + Paths::ToUtf8(file), false, key, file });
        }
    }
}

void EphemeralBar::Dismiss(AppContext& ctx) {
    if (ctx.stage.isEphemeralBarExpanded) {
        ctx.stage.ToggleEphemeralBar();
    }
}

void EphemeralBar::FinishPanel(AppContext& ctx) {
    auto& ui = ctx.ui;
    RebuildRows(ctx);

    ScaffoldOptions options;
    options.id = "Finish";
    options.title = Tr("createFinishScreen_finish");
    options.subtitle = Tr("createFinishScreen_finishSubtitle");
    options.topRightWidget = [&] {
        if (Widgets::IconButton(ui, "Finish_Close", Icons::Close, 24, RetroColors::White)) {
            Dismiss(ctx);
        }
    };

    CustomScaffold(ui, options, [&] {
        CLAY({ .id = CLAY_ID("Finish_Body"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                           .padding = CLAY_PADDING_ALL(20),
                           .childAlignment = { .x = CLAY_ALIGN_X_CENTER },
                           .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
            CLAY({ .id = CLAY_ID("Finish_List"),
                   .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } },
                   .custom = ui.Widget([this](ImVec2 min, ImVec2 max) {
                       ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
                       ImGui::BeginChild("##FinishList", max - min);
                       constexpr float rowHeight = 20.0f;
                       ImGuiListClipper clipper;
                       clipper.Begin((int)mRows.size(), rowHeight);
                       while (clipper.Step()) {
                           for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
                               const Row& row = mRows[i];
                               const ImVec2 start = ImGui::GetCursorScreenPos();
                               const float width = ImGui::GetContentRegionAvail().x;
                               if (row.header) {
                                   ImGui::PushFont(Theme::GetFont(FONT_MEDIUM), (float)TextSize::TitleSmall);
                                   ImGui::TextUnformatted(row.text.c_str());
                                   ImGui::PopFont();
                               } else {
                                   ImGui::PushFont(Theme::GetFont(FONT_REGULAR), (float)TextSize::BodyMedium);
                                   const ImVec2 textMax(start.x + width - 40.0f, start.y + rowHeight);
                                   ImGui::RenderTextEllipsis(ImGui::GetWindowDrawList(), start, textMax, textMax.x,
                                                             row.text.c_str(), nullptr, nullptr);
                                   ImGui::PopFont();
                                   ImGui::SetCursorScreenPos(ImVec2(start.x + width - 40.0f, start.y));
                                   ImGui::PushID(i);
                                   ImGui::PushFont(Theme::GetFont(FONT_ICONS), 20.0f);
                                   ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
                                   ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                                   if (ImGui::Button(Icons::Delete, ImVec2(20, 20))) {
                                       mPendingRemove = std::make_pair(row.file, row.key);
                                   }
                                   ImGui::PopStyleVar();
                                   ImGui::PopStyleColor();
                                   ImGui::PopFont();
                                   ImGui::PopID();
                               }
                               ImGui::SetCursorScreenPos(ImVec2(start.x, start.y + rowHeight));
                               ImGui::Dummy(ImVec2(0, 0));
                           }
                       }
                       ImGui::EndChild();
                       ImGui::PopStyleColor();
                   }) }) {
            }

            Widgets::Gap(0, 20);
            const float buttonWidth = ImGui::GetMainViewport()->WorkSize.x * 0.5f;
            const bool generating = ctx.stage.IsGenerating();
            Widgets::ButtonStyle style = Widgets::ElevatedStyle();
            style.background = RetroColors::Green;
            const Clay_Sizing sizing = { CLAY_SIZING_FIXED(buttonWidth), CLAY_SIZING_FIXED(50) };
            if (generating) {
                CLAY({ .id = CLAY_ID("Finish_Generating"),
                       .layout = { .sizing = sizing,
                                   .childGap = 10,
                                   .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } },
                       .backgroundColor = RetroColors::Green,
                       .cornerRadius = CLAY_CORNER_RADIUS(4) }) {
                    Widgets::Spinner(ui, "Finish_Spinner", 20, RetroColors::White, 3);
                    Widgets::Text(ui, fmt::format("{}/{}", ctx.stage.FilesProcessed(), ctx.stage.TotalFiles()),
                                  TextSize::BodyMedium, RetroColors::White, FONT_MEDIUM);
                }
            } else if (Widgets::Button(ui, "Finish_Generate", Tr("createFinishScreen_generateOtr"), sizing,
                                       ctx.stage.HasStagedFiles(), style)) {
                ctx.stage.GenerateOtr([this, &ctx] { Dismiss(ctx); });
            }
        }
    });
}
