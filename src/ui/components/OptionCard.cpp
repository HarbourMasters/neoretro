#include "OptionCard.h"
#include "ui/clay/ClayUi.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"

#include <imgui.h>
#include <string>

bool OptionCard(ClayUi& ui, std::string_view id, std::string_view text, const char* icon, bool* hovered,
                const std::function<void()>& overlay) {
    const Clay_ElementId cardId = ClayUi::Id(std::string(id) + "_Card");
    const bool isHovered = ui.Hovered(cardId);
    if (isHovered) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    CLAY({ .id = ClayUi::Id(id),
           .layout = {
               .childGap = 10, .childAlignment = { CLAY_ALIGN_X_CENTER }, .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
        CLAY({ .id = cardId,
               .layout = { .padding = CLAY_PADDING_ALL(80) },
               .backgroundColor = isHovered ? RetroColors::White24 : RetroColors::White12,
               .cornerRadius = CLAY_CORNER_RADIUS(20) }) {
            CLAY_TEXT(ui.Str(icon),
                      CLAY_TEXT_CONFIG({ .textColor = RetroColors::White, .fontId = FONT_ICONS, .fontSize = 50 }));
        }
        CLAY_TEXT(ui.Str(text), CLAY_TEXT_CONFIG({ .textColor = RetroColors::WithOpacity(RetroColors::White, 0.5f),
                                                   .fontId = FONT_REGULAR,
                                                   .fontSize = TextSize::BodyLarge,
                                                   .textAlignment = CLAY_TEXT_ALIGN_CENTER }));
        if (overlay) {
            CLAY({ .id = ClayUi::Id(std::string(id) + "_Overlay"),
                   .floating = { .offset = { 10, 10 },
                                 .attachPoints = { CLAY_ATTACH_POINT_LEFT_TOP, CLAY_ATTACH_POINT_LEFT_TOP },
                                 .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_PASSTHROUGH,
                                 .attachTo = CLAY_ATTACH_TO_PARENT } }) {
                overlay();
            }
        }
    }

    if (hovered != nullptr) {
        *hovered = isHovered;
    }
    return ui.Clicked(cardId);
}
