#include "Toasts.h"

#include "ui/clay/ClayUi.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"

#include <imgui.h>

void Toasts::Post(Toast toast) {
    std::lock_guard lock(mMutex);
    mQueue.push_back(std::move(toast));
}

void Toasts::Error(std::string text) {
    Post({ std::move(text), RetroColors::Red, RetroColors::White });
}

void Toasts::Layout(ClayUi& ui) {
    std::lock_guard lock(mMutex);
    if (mQueue.empty()) {
        return;
    }
    Toast& toast = mQueue.front();
    const double now = ImGui::GetTime();
    if (toast.shownAt < 0.0) {
        toast.shownAt = now;
    }
    if (now - toast.shownAt > toast.duration) {
        mQueue.pop_front();
        return;
    }

    CLAY({ .id = CLAY_ID("Toast"),
           .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT(48) },
                       .padding = { 16, 16, 14, 14 },
                       .childGap = 5,
                       .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } },
           .backgroundColor = toast.background,
           .floating = { .offset = { 0, -24 },
                         .zIndex = 100,
                         .attachPoints = { CLAY_ATTACH_POINT_LEFT_BOTTOM, CLAY_ATTACH_POINT_LEFT_BOTTOM },
                         .attachTo = CLAY_ATTACH_TO_ROOT } }) {
        if (toast.icon != nullptr) {
            CLAY_TEXT(ui.Str(toast.icon),
                      CLAY_TEXT_CONFIG({ .textColor = toast.iconColor, .fontId = FONT_ICONS, .fontSize = 24 }));
        }
        CLAY_TEXT(ui.Str(toast.text),
                  CLAY_TEXT_CONFIG(
                      { .textColor = toast.textColor, .fontId = FONT_REGULAR, .fontSize = TextSize::BodyLarge }));
    }
}
