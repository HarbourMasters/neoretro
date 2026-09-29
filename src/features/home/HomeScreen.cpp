#include "HomeScreen.h"

#include "app/AppContext.h"
#include "app/Router.h"
#include "l10n/Strings.h"
#include "services/UpdateChecker.h"
#include "ui/clay/ClayUi.h"
#include "ui/components/GradientText.h"
#include "ui/components/OptionCard.h"
#include "ui/components/Widgets.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"
#include "build.h"
#include "utils/Platform.h"

#include <imgui.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace {
constexpr Clay_Color kHm64Colors[] = {
    { 0x3F, 0x51, 0xB5, 0xFF }, { 0x21, 0x96, 0xF3, 0xFF }, { 0x4C, 0xAF, 0x50, 0xFF },
    { 0xFF, 0xEB, 0x3B, 0xFF }, { 0xFF, 0x98, 0x00, 0xFF }, { 0xF4, 0x43, 0x36, 0xFF },
};

const Clay_Color kFaded = RetroColors::WithOpacity(RetroColors::White, 0.2f);

void UpdateBanner(ClayUi& ui) {
    const Clay_ElementId id = CLAY_ID("Home_Update");
    const bool hovered = ui.Hovered(id);
    if (hovered) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    CLAY({ .id = id,
           .layout = { .padding = { 10, 10, 5, 5 }, .childGap = 5, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } },
           .backgroundColor = RetroColors::WithOpacity(RetroColors::Black, hovered ? 0.45f : 0.3f),
           .cornerRadius = CLAY_CORNER_RADIUS(5),
           .floating = { .offset = { 20, 20 },
                         .attachPoints = { CLAY_ATTACH_POINT_LEFT_TOP, CLAY_ATTACH_POINT_LEFT_TOP },
                         .attachTo = CLAY_ATTACH_TO_PARENT } }) {
        Widgets::Text(ui, Icons::WarningAmber, 25, RetroColors::Yellow700, FONT_ICONS);
        Widgets::Text(ui, "New version available", 15, RetroColors::Yellow700);
    }
    if (ui.Clicked(id)) {
        Platform::OpenUrl(gReleasesUrl);
    }
}
} // namespace

void HomeScreen::OnEnter(AppContext& ctx) {
    UpdateChecker::Start();
}

void HomeScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    CLAY({ .id = CLAY_ID("Home"),
           .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                       .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
                       .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
        Widgets::Text(ui, "neoretro", TextSize::DisplaySmall, RetroColors::White);

        CLAY({ .id = CLAY_ID("Home_Credits"),
               .layout = { .childGap = 4, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
            Widgets::Text(ui, "Brought to you by", TextSize::TitleSmall, RetroColors::White, FONT_MEDIUM);
            GradientText(ui, "Home_HM64", "HM64", FONT_REGULAR, TextSize::BodyMedium, kHm64Colors, 2.0f);
            Widgets::Text(ui, Icons::Favorite, 24, RetroColors::Red, FONT_ICONS);
        }

        Widgets::Gap(0, 40);

        CLAY({ .id = CLAY_ID("Home_Options"), .layout = { .childGap = 20 } }) {
            if (OptionCard(ui, "Home_Create", Tr("home_createOption"), Icons::AddCircle)) {
                ctx.router.Push("/create_selection");
            }
            if (OptionCard(ui, "Home_Inspect", Tr("home_inspectOption"), Icons::Visibility)) {
                ctx.router.Push("/view_otr");
            }
            if (OptionCard(ui, "Home_Debug", "Debug", Icons::WarningRounded)) {
                ctx.router.Push("/debug_selection");
            }
        }

        Widgets::Gap(0, 40);

        const char* branch = gGitBranch[0] != '\0' ? gGitBranch : "unknown";
        const char* hash = gGitCommitHash[0] != '\0' ? gGitCommitHash : "0000000";
        Widgets::Text(ui, fmt::format("{} - {}", hash, branch), TextSize::TitleSmall, kFaded, FONT_MEDIUM);
        Widgets::Text(ui, gBuildDate, TextSize::TitleSmall, kFaded, FONT_MEDIUM);

        if (UpdateChecker::UpdateAvailable()) {
            UpdateBanner(ui);
        }
    }
}
