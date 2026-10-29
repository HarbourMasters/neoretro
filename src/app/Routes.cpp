#include "Router.h"

#include "features/create/custom/CreateCustomScreen.h"
#include "features/create/custom_sequences/CreateCustomSequencesScreen.h"
#include "features/create/replace_textures/CreateReplaceTexturesScreen.h"
#include "features/create/selection/CreateSelectionScreen.h"
#include "features/debug/DebugConvertTexturesScreen.h"
#include "features/debug/DebugFontGeneratorScreen.h"
#include "features/debug/DebugSelectionScreen.h"
#include "features/games/GameScreens.h"
#include "features/games/ghostship/GhostshipTextScreen.h"
#include "features/games/paperboat/PaperboatImageScreen.h"
#include "features/games/paperboat/PaperboatTextScreen.h"
#include "features/home/HomeScreen.h"
#include "features/inspect_otr/InspectOtrScreen.h"

#include <memory>

void RegisterRoutes(Router& router) {
    auto route = [&router]<typename T>(const char* name) {
        router.Register(name, [] { return std::make_unique<T>(); });
    };
    route.operator()<HomeScreen>("/");
    route.operator()<CreateSelectionScreen>("/create_selection");
    route.operator()<GameSelectionScreen>("/game_selection");
    route.operator()<SohGameScreen>("/game_selection/soh");
    route.operator()<GhostshipGameScreen>("/game_selection/ghostship");
    route.operator()<Ship2GameScreen>("/game_selection/2ship");
    auto ghostshipText = [&router](const char* name, GhostshipTextKind kind) {
        router.Register(name, [kind] { return std::make_unique<GhostshipTextScreen>(kind); });
    };
    ghostshipText("/game_selection/ghostship/dialogs", GhostshipTextKind::Dialogs);
    ghostshipText("/game_selection/ghostship/texts", GhostshipTextKind::Texts);
    ghostshipText("/game_selection/ghostship/strings", GhostshipTextKind::Strings);
    route.operator()<PaperboatGameScreen>("/game_selection/paperboat");
    route.operator()<PaperboatTextScreen>("/game_selection/paperboat/strings");
    auto paperboatImages = [&router](const char* name, Pm64::ImageKind kind) {
        router.Register(name, [kind] { return std::make_unique<PaperboatImageScreen>(kind); });
    };
    paperboatImages("/game_selection/paperboat/sprites", Pm64::ImageKind::Sprites);
    paperboatImages("/game_selection/paperboat/map_textures", Pm64::ImageKind::MapTextures);
    paperboatImages("/game_selection/paperboat/pictures", Pm64::ImageKind::Pictures);
    route.operator()<CreateCustomScreen>("/create_custom");
    route.operator()<InspectOtrScreen>("/view_otr");
    route.operator()<CreateCustomSequencesScreen>("/create_custom_sequences");
    route.operator()<CreateReplaceTexturesScreen>("/create_replace_textures");
    route.operator()<DebugSelectionScreen>("/debug_selection");
    route.operator()<DebugConvertTexturesScreen>("/debug_convert_textures");
    route.operator()<DebugFontGeneratorScreen>("/debug_generate_font");
}
