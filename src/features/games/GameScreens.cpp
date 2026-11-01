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
        const auto shipRow = [&] {
            if (OptionCard(ui, "GameSelection_Soh", Tr("gameSelectionScreenSoh_title"), Icons::DirectionsBoatFilled)) {
                ctx.router.Push("/game_selection/soh");
            }
            if (OptionCard(ui, "GameSelection_Ghostship", Tr("gameSelectionScreenGhostship_title"), Icons::Star)) {
                ctx.router.Push("/game_selection/ghostship");
            }
            if (OptionCard(ui, "GameSelection_2Ship", Tr("gameSelectionScreen2Ship_title"), Icons::DarkMode)) {
                ctx.toasts.Post({ Tr("gameSelection2ShipComingSoon_text"), RetroColors::Cream, RetroColors::Black,
                                  Icons::Favorite, RetroColors::Red, 4.0f });
            }
        };
        const auto paperboatRow = [&] {
            if (OptionCard(ui, "GameSelection_Paperboat", Tr("gameSelectionScreenPaperboat_title"), Icons::Sailing)) {
                ctx.router.Push("/game_selection/paperboat");
            }
        };
        CenteredCardRows(ui, "GameSelection_Cards", 15, { shipRow, paperboatRow });
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

void GhostshipGameScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    ScaffoldOptions options;
    options.title = Tr("gameSelectionScreenGhostship_title");
    options.subtitle = Tr("gameSelectionScreenGhostship_subtitle");
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };

    CustomScaffold(ui, options, [&] {
        CenteredCards(ui, "GhostshipGame_Cards", 15, [&] {
            if (OptionCard(ui, "GhostshipGame_Dialogs", Tr("ghostshipTextScreen_dialogs"), Icons::Chat)) {
                ctx.router.Push("/game_selection/ghostship/dialogs");
            }
            if (OptionCard(ui, "GhostshipGame_Texts", Tr("ghostshipTextScreen_texts"), Icons::Title)) {
                ctx.router.Push("/game_selection/ghostship/texts");
            }
            if (OptionCard(ui, "GhostshipGame_Strings", Tr("ghostshipTextScreen_strings"), Icons::Translate)) {
                ctx.router.Push("/game_selection/ghostship/strings");
            }
        });
    });
}

void PaperboatGameScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    ScaffoldOptions options;
    options.title = Tr("gameSelectionScreenPaperboat_title");
    options.subtitle = Tr("gameSelectionScreenPaperboat_subtitle");
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };

    CustomScaffold(ui, options, [&] {
        CenteredCards(ui, "PaperboatGame_Cards", 15, [&] {
            if (OptionCard(ui, "PaperboatGame_Strings", Tr("paperboatTextScreen_strings"), Icons::Translate)) {
                ctx.router.Push("/game_selection/paperboat/strings");
            }
            if (OptionCard(ui, "PaperboatGame_Sprites", Tr("paperboatImageScreen_sprites"), Icons::Face)) {
                ctx.router.Push("/game_selection/paperboat/sprites");
            }
            if (OptionCard(ui, "PaperboatGame_MapTextures", Tr("paperboatImageScreen_mapTextures"), Icons::Landscape)) {
                ctx.router.Push("/game_selection/paperboat/map_textures");
            }
            if (OptionCard(ui, "PaperboatGame_Pictures", Tr("paperboatImageScreen_pictures"), Icons::Wallpaper)) {
                ctx.router.Push("/game_selection/paperboat/pictures");
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
