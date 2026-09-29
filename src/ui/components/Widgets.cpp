#include "Widgets.h"

#include "ui/clay/ClayUi.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"

#include <cmath>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <string>

namespace Widgets {
namespace {
const Clay_Color kDisabledBackground = RetroColors::WithOpacity(RetroColors::White, 0.12f);
const Clay_Color kDisabledText = RetroColors::WithOpacity(RetroColors::White, 0.38f);
const Clay_Color kOutline = RetroColors::WithOpacity(RetroColors::White, 0.38f);

Clay_Color Lighten(Clay_Color color, float amount) {
    return { color.r + (255 - color.r) * amount, color.g + (255 - color.g) * amount, color.b + (255 - color.b) * amount,
             color.a };
}
} // namespace

ButtonStyle ElevatedStyle() {
    return { RetroColors::TelegramBlue, RetroColors::White, false, TextSize::BodyMedium };
}

ButtonStyle OutlinedStyle() {
    return { RetroColors::Transparent, RetroColors::TelegramBlue, true, TextSize::BodyMedium };
}

bool Button(ClayUi& ui, std::string_view id, std::string_view label, Clay_Sizing sizing, bool enabled,
            const ButtonStyle& style, const char* leadingIcon) {
    const Clay_ElementId elementId = ClayUi::Id(id);
    const bool hovered = enabled && ui.Hovered(elementId);
    if (hovered) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    Clay_Color background = style.background;
    if (!enabled) {
        background = style.outlined ? RetroColors::Transparent : kDisabledBackground;
    } else if (hovered) {
        background = style.outlined ? RetroColors::WithOpacity(style.text, 0.08f) : Lighten(background, 0.12f);
    }
    const Clay_Color text = enabled ? style.text : kDisabledText;
    const uint16_t borderWidth = style.outlined ? 1 : 0;

    CLAY({ .id = elementId,
           .layout = { .sizing = sizing,
                       .padding = { 16, 16, 8, 8 },
                       .childGap = 8,
                       .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } },
           .backgroundColor = background,
           .cornerRadius = CLAY_CORNER_RADIUS(4),
           .border = { .color = style.outlined ? (enabled ? kOutline : kDisabledBackground) : RetroColors::Transparent,
                       .width = CLAY_BORDER_OUTSIDE(borderWidth) } }) {
        if (leadingIcon != nullptr) {
            CLAY_TEXT(ui.Str(leadingIcon),
                      CLAY_TEXT_CONFIG({ .textColor = text, .fontId = FONT_ICONS, .fontSize = style.fontSize }));
        }
        CLAY_TEXT(ui.Str(label), CLAY_TEXT_CONFIG({ .textColor = text,
                                                    .fontId = FONT_MEDIUM,
                                                    .fontSize = style.fontSize,
                                                    .wrapMode = CLAY_TEXT_WRAP_NONE }));
    }
    return enabled && ui.Clicked(elementId);
}

bool IconButton(ClayUi& ui, std::string_view id, const char* icon, uint16_t size, Clay_Color color, float padding) {
    const Clay_ElementId elementId = ClayUi::Id(id);
    const bool hovered = ui.Hovered(elementId);
    if (hovered) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    const uint16_t pad = (uint16_t)padding;
    CLAY({ .id = elementId,
           .layout = { .padding = CLAY_PADDING_ALL(pad),
                       .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } },
           .backgroundColor = hovered ? RetroColors::WithOpacity(RetroColors::White, 0.1f) : RetroColors::Transparent,
           .cornerRadius = CLAY_CORNER_RADIUS((size + padding * 2) / 2) }) {
        CLAY_TEXT(ui.Str(icon), CLAY_TEXT_CONFIG({ .textColor = color, .fontId = FONT_ICONS, .fontSize = size }));
    }
    return ui.Clicked(elementId);
}

bool Switch(ClayUi& ui, std::string_view id, bool value) {
    const Clay_ElementId elementId = ClayUi::Id(id);
    const bool hovered = ui.Hovered(elementId);
    if (hovered) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    const Clay_Color track =
        value ? RetroColors::WithOpacity(RetroColors::Blue, 0.5f) : RetroColors::WithOpacity(RetroColors::White, 0.3f);
    const Clay_Color thumb = value ? RetroColors::Blue : Clay_Color{ 0xBD, 0xBD, 0xBD, 0xFF };

    CLAY({ .id = elementId,
           .layout = { .sizing = { CLAY_SIZING_FIXED(44), CLAY_SIZING_FIXED(40) },
                       .childAlignment = { value ? CLAY_ALIGN_X_RIGHT : CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER } } }) {
        CLAY({ .layout = { .sizing = { CLAY_SIZING_FIXED(34), CLAY_SIZING_FIXED(14) } },
               .backgroundColor = track,
               .cornerRadius = CLAY_CORNER_RADIUS(7),
               .floating = { .offset = { 5, 13 },
                             .attachPoints = { CLAY_ATTACH_POINT_LEFT_TOP, CLAY_ATTACH_POINT_LEFT_TOP },
                             .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_PASSTHROUGH,
                             .attachTo = CLAY_ATTACH_TO_PARENT } }) {
        }
        CLAY({ .layout = { .sizing = { CLAY_SIZING_FIXED(20), CLAY_SIZING_FIXED(20) } },
               .backgroundColor = thumb,
               .cornerRadius = CLAY_CORNER_RADIUS(10) }) {
        }
    }
    return ui.Clicked(elementId);
}

void OutlinedField(ClayUi& ui, std::string_view id, std::string_view label) {
    CLAY({ .id = ClayUi::Id(id),
           .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(50) },
                       .padding = { 12, 12, 0, 0 },
                       .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } },
           .cornerRadius = CLAY_CORNER_RADIUS(4),
           .clip = { .horizontal = true },
           .border = { .color = RetroColors::WithOpacity(RetroColors::White, 0.24f),
                       .width = CLAY_BORDER_OUTSIDE(1) } }) {
        CLAY_TEXT(ui.Str(label), CLAY_TEXT_CONFIG({ .textColor = kDisabledText,
                                                    .fontId = FONT_REGULAR,
                                                    .fontSize = TextSize::BodyLarge,
                                                    .wrapMode = CLAY_TEXT_WRAP_NONE }));
    }
}

void Spinner(ClayUi& ui, std::string_view id, float size, Clay_Color color, float thickness) {
    CLAY({ .id = ClayUi::Id(id),
           .layout = { .sizing = { CLAY_SIZING_FIXED(size), CLAY_SIZING_FIXED(size) } },
           .custom = ui.Widget([color, thickness](ImVec2 min, ImVec2 max) {
               ImDrawList* dl = ImGui::GetWindowDrawList();
               const ImVec2 center = (min + max) * 0.5f;
               const float radius = (max.x - min.x) * 0.5f - thickness * 0.5f;
               const float t = (float)ImGui::GetTime();
               const float start = t * 4.7f;
               const float sweep = 0.4f + 2.6f * (0.5f + 0.5f * std::sin(t * 2.5f));
               dl->PathArcTo(center, radius, start, start + sweep, 32);
               dl->PathStroke(IM_COL32((int)color.r, (int)color.g, (int)color.b, (int)color.a), ImDrawFlags_None,
                              thickness);
           }) }) {
    }
}

void CenteredSpinner(ClayUi& ui, std::string_view id) {
    CLAY({ .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                       .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } } }) {
        Spinner(ui, id, 36, RetroColors::TelegramBlue);
    }
}

void TextList(ClayUi& ui, std::string_view id, const std::vector<std::string>* items, float rowHeight) {
    const std::string childId = "##" + std::string(id);
    CLAY({ .id = ClayUi::Id(id),
           .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } },
           .custom = ui.Widget([items, rowHeight, childId](ImVec2 min, ImVec2 max) {
               ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
               ImGui::BeginChild(childId.c_str(), max - min, ImGuiChildFlags_None);
               ImGui::PushFont(Theme::GetFont(FONT_REGULAR), (float)TextSize::BodyMedium);
               ImGuiListClipper clipper;
               clipper.Begin((int)items->size(), rowHeight);
               while (clipper.Step()) {
                   for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
                       const ImVec2 start = ImGui::GetCursorScreenPos();
                       ImGui::TextUnformatted((*items)[i].c_str());
                       ImGui::SetCursorScreenPos(ImVec2(start.x, start.y + rowHeight));
                       ImGui::Dummy(ImVec2(0, 0));
                   }
               }
               ImGui::PopFont();
               ImGui::EndChild();
               ImGui::PopStyleColor();
           }) }) {
    }
}

void SearchField(ClayUi& ui, std::string_view id, std::string* text, const char* hint, float width,
                 std::function<void()> onChange) {
    const std::string inputId = "##" + std::string(id);
    CLAY({ .id = ClayUi::Id(id),
           .layout = { .sizing = { CLAY_SIZING_FIXED(width), CLAY_SIZING_FIXED(50) },
                       .padding = { 12, 12, 0, 0 },
                       .childGap = 8,
                       .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } },
           .cornerRadius = CLAY_CORNER_RADIUS(4),
           .border = { .color = RetroColors::WithOpacity(RetroColors::White, 0.38f),
                       .width = CLAY_BORDER_OUTSIDE(1) } }) {
        CLAY_TEXT(ui.Str(Icons::Search),
                  CLAY_TEXT_CONFIG({ .textColor = RetroColors::WithOpacity(RetroColors::White, 0.7f),
                                     .fontId = FONT_ICONS,
                                     .fontSize = 24 }));
        CLAY({ .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(30) } },
               .custom = ui.Widget([text, hint, inputId, onChange = std::move(onChange)](ImVec2 min, ImVec2 max) {
                   ImGui::PushFont(Theme::GetFont(FONT_REGULAR), (float)TextSize::BodyLarge);
                   ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
                   ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(0, 0, 0, 0));
                   ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(0, 0, 0, 0));
                   const float frameHeight = ImGui::GetFrameHeight();
                   ImGui::SetCursorScreenPos(ImVec2(min.x, min.y + ((max.y - min.y) - frameHeight) * 0.5f));
                   ImGui::SetNextItemWidth(max.x - min.x);
                   if (ImGui::InputTextWithHint(inputId.c_str(), hint, text) && onChange) {
                       onChange();
                   }
                   ImGui::PopStyleColor(3);
                   ImGui::PopFont();
               }) }) {
        }
    }
}

void Dropdown(ClayUi& ui, std::string_view id, const std::vector<std::string>* items, int selected, float width,
              std::function<void(int)> onSelect) {
    const std::string comboId = "##" + std::string(id);
    CLAY({ .id = ClayUi::Id(id),
           .layout = { .sizing = { CLAY_SIZING_FIXED(width), CLAY_SIZING_FIXED(40) } },
           .custom = ui.Widget([items, selected, comboId, onSelect = std::move(onSelect)](ImVec2 min, ImVec2 max) {
               ImGui::PushFont(Theme::GetFont(FONT_BOLD), (float)TextSize::BodyMedium);
               ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0x40, 0xAA, 0xE8, 0xFF));
               ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(0x55, 0xB6, 0xEE, 0xFF));
               ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0x40, 0xAA, 0xE8, 0xFF));
               ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(0x40, 0xAA, 0xE8, 0xFF));
               ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                                   ImVec2(17.0f, ((max.y - min.y) - ImGui::GetFontSize()) * 0.5f));
               ImGui::SetNextItemWidth(max.x - min.x);
               const char* preview = selected >= 0 && selected < (int)items->size() ? (*items)[selected].c_str() : "";
               if (ImGui::BeginCombo(comboId.c_str(), preview)) {
                   for (int i = 0; i < (int)items->size(); i++) {
                       if (ImGui::Selectable((*items)[i].c_str(), i == selected) && onSelect) {
                           onSelect(i);
                       }
                   }
                   ImGui::EndCombo();
               }
               ImGui::PopStyleVar();
               ImGui::PopStyleColor(4);
               ImGui::PopFont();
           }) }) {
    }
}

void Text(ClayUi& ui, std::string_view text, uint16_t fontSize, Clay_Color color, uint16_t fontId,
          Clay_TextAlignment alignment) {
    CLAY_TEXT(
        ui.Str(text),
        CLAY_TEXT_CONFIG({ .textColor = color, .fontId = fontId, .fontSize = fontSize, .textAlignment = alignment }));
}

void Gap(float width, float height) {
    CLAY({ .layout = { .sizing = { CLAY_SIZING_FIXED(width), CLAY_SIZING_FIXED(height) } } }) {
    }
}

void Spacer() {
    CLAY({ .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } } }) {
    }
}
} // namespace Widgets

#include "ui/PreviewTexture.h"

namespace Widgets {
void ImagePreview(ClayUi& ui, std::string_view id, const PreviewTexture* texture) {
    CLAY({ .id = ClayUi::Id(id),
           .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } },
           .custom = ui.Widget([texture](ImVec2 min, ImVec2 max) {
               if (texture == nullptr || !texture->Valid()) {
                   return;
               }
               const ImVec2 area = max - min;
               const float scale = std::min(area.x / texture->Width(), area.y / texture->Height());
               const ImVec2 size(texture->Width() * scale, texture->Height() * scale);
               const ImVec2 pos = min + (area - size) * 0.5f;
               ImGui::GetWindowDrawList()->AddImage(ImTextureRef((ImTextureID)texture->Id()), pos, pos + size);
           }) }) {
    }
}
} // namespace Widgets
