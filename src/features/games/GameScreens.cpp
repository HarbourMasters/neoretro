#include "GameScreens.h"

#include "app/AppContext.h"
#include "app/Router.h"
#include "l10n/Strings.h"
#include "ui/Toasts.h"
#include "ui/clay/ClayUi.h"
#include "ui/components/CardWrap.h"
#include "ui/components/CustomScaffold.h"
#include "ui/components/OptionCard.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"

void GameSelectionScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    ScaffoldOptions options;
    options.title = Tr("gameSelectionScreen_title");
    options.subtitle = Tr("gameSelectionScreen_subtitle");
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };

    CustomScaffold(ui, options, [&] {
        CenteredCards(ui, "GameSelection_Cards", 15, [&] {
            if (OptionCard(ui, "GameSelection_Soh", Tr("gameSelectionScreenSoh_title"), Icons::DirectionsBoatFilled)) {
                ctx.router.Push("/game_selection/soh");
            }
            if (OptionCard(ui, "GameSelection_2Ship", Tr("gameSelectionScreen2Ship_title"), Icons::DarkMode)) {
                ctx.toasts.Post({ Tr("gameSelection2ShipComingSoon_text"), RetroColors::Cream, RetroColors::Black,
                                  Icons::Favorite, RetroColors::Red, 4.0f });
            }
        });
    });
}

void SohGameScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    ScaffoldOptions options;
    options.title = Tr("gameSelectionScreenSoh_title");
    options.subtitle = Tr("gameSelectionScreenSoh_subtitle");
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };

    CustomScaffold(ui, options, [&] {
        CenteredCards(ui, "SohGame_Cards", 15, [&] {
            if (OptionCard(ui, "SohGame_Sequences", Tr("createSelectionScreen_customSequences"), Icons::LibraryMusic)) {
                ctx.router.Push("/create_custom_sequences");
            }
            if (OptionCard(ui, "SohGame_Font", Tr("sohCreateDebugFontScreen_title"), Icons::TextFields)) {
                ctx.router.Push("/debug_generate_font");
            }
        });
    });
}

void Ship2GameScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    ScaffoldOptions options;
    options.title = Tr("gameSelectionScreen2Ship_title");
    options.subtitle = Tr("gameSelectionScreen2Ship_subtitle");
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };

    CustomScaffold(ui, options, [&] {
        CenteredCards(ui, "Ship2Game_Cards", 15, [&] {
            if (OptionCard(ui, "Ship2Game_Soh", Tr("gameSelectionScreenSoh_title"), Icons::Extension)) {
                ctx.router.Push("/game_selection/soh");
            }
            if (OptionCard(ui, "Ship2Game_Textures", Tr("gameSelectionScreen2Ship_title"), Icons::MusicNote)) {
                ctx.router.Push("/create_replace_textures");
            }
        });
    });
}
