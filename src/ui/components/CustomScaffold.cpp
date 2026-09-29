#include "CustomScaffold.h"

#include "Widgets.h"
#include "ui/clay/ClayUi.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"

#include <string>

namespace {
constexpr uint16_t kContentPaddingVertical = 15;
constexpr uint16_t kContentPaddingHorizontal = 15;
} // namespace

void CustomScaffold(ClayUi& ui, const ScaffoldOptions& options, const std::function<void()>& content) {
    const std::string prefix(options.id);
    CLAY({ .id = ClayUi::Id(prefix),
           .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                       .padding = { kContentPaddingHorizontal, kContentPaddingHorizontal, kContentPaddingVertical, 0 },
                       .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
        CLAY({ .id = ClayUi::Id(prefix + "_Header"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(60) },
                           .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
            if (options.onBackButtonPressed) {
                if (Widgets::IconButton(ui, prefix + "_Back", Icons::ChevronLeft, 24, RetroColors::White)) {
                    options.onBackButtonPressed();
                }
            }
            CLAY({ .id = ClayUi::Id(prefix + "_Title"), .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
                Widgets::Text(ui, options.title, TextSize::HeadlineSmall, RetroColors::White, FONT_REGULAR);
                if (!options.subtitle.empty()) {
                    Widgets::Text(ui, options.subtitle, TextSize::BodyMedium,
                                  RetroColors::WithOpacity(RetroColors::White, 0.5f), FONT_REGULAR);
                }
            }
            if (options.topRightWidget) {
                CLAY({ .id = ClayUi::Id(prefix + "_TopRight"),
                       .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                                   .childAlignment = { CLAY_ALIGN_X_RIGHT, CLAY_ALIGN_Y_CENTER } } }) {
                    options.topRightWidget();
                }
            }
        }
        CLAY({ .id = ClayUi::Id(prefix + "_Content"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                           .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
            content();
        }
    }
}
