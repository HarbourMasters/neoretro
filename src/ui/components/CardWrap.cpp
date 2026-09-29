#include "CardWrap.h"

#include "ui/clay/ClayUi.h"

#include <string>

void CenteredCards(ClayUi& ui, std::string_view id, uint16_t spacing, const std::function<void()>& cards) {
    CLAY({ .id = ClayUi::Id(id),
           .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                       .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } } }) {
        CLAY({ .id = ClayUi::Id(std::string(id) + "_Row"), .layout = { .childGap = spacing } }) {
            cards();
        }
    }
}
