#include "CreateSelectionScreen.h"

#include "app/AppContext.h"
#include "app/Router.h"
#include "l10n/Strings.h"
#include "ui/clay/ClayUi.h"
#include "ui/components/CardWrap.h"
#include "ui/components/CustomScaffold.h"
#include "ui/components/OptionCard.h"
#include "ui/components/Widgets.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"

namespace {
void ExtractModsWarning(ClayUi& ui) {
    CLAY({ .id = CLAY_ID("CreateSelection_Warning"),
           .layout = { .sizing = { CLAY_SIZING_FIXED(190) },
                       .padding = CLAY_PADDING_ALL(5),
                       .layoutDirection = CLAY_TOP_TO_BOTTOM },
           .backgroundColor = RetroColors::WithOpacity(RetroColors::Yellow800, 0.7f),
           .cornerRadius = CLAY_CORNER_RADIUS(5) }) {
        CLAY({ .layout = { .childGap = 2, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
            Widgets::Text(ui, Icons::WarningAmber, 15, RetroColors::White, FONT_ICONS);
            Widgets::Text(ui, Tr("extractModsWarning_part1"), 11, RetroColors::White);
        }
        Widgets::Text(ui, Tr("extractModsWarning_part2"), 11, RetroColors::White);
    }
}
} // namespace

void CreateSelectionScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    ScaffoldOptions options;
    options.title = Tr("createSelectionScreen_title");
    options.subtitle = Tr("createSelectionScreen_subtitle");
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };

    CustomScaffold(ui, options, [&] {
        CenteredCards(ui, "CreateSelection_Cards", 15, [&] {
            if (OptionCard(ui, "CreateSelection_Replace", Tr("createSelectionScreen_nonHdTex"), Icons::Texture, nullptr,
                           [&] { ExtractModsWarning(ui); })) {
                ctx.router.Push("/create_replace_textures");
            }
            if (OptionCard(ui, "CreateSelection_Games", Tr("gameSelectionScreen_title"), Icons::VideogameAsset)) {
                ctx.router.Push("/game_selection");
            }
            if (OptionCard(ui, "CreateSelection_Custom", Tr("createSelectionScreen_custom"), Icons::SettingsSuggest)) {
                ctx.router.Push("/create_custom");
            }
        });
    });
}
