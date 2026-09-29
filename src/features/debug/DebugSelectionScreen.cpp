#include "DebugSelectionScreen.h"

#include "app/AppContext.h"
#include "app/Router.h"
#include "ui/clay/ClayUi.h"
#include "ui/components/CardWrap.h"
#include "ui/components/CustomScaffold.h"
#include "ui/components/OptionCard.h"
#include "ui/theme/Theme.h"

void DebugSelectionScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    ScaffoldOptions options;
    options.title = "Debug Selection";
    options.subtitle = "This options are on an experimental state and may not work as expected";
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };

    CustomScaffold(ui, options, [&] {
        CenteredCards(ui, "DebugSelection_Cards", 0, [&] {
            if (OptionCard(ui, "DebugSelection_Textures", "Textures", Icons::Texture)) {
                ctx.router.Push("/debug_convert_textures");
            }
        });
    });
}
