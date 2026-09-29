#pragma once

#include <clay.h>

#include <functional>
#include <string>
#include <string_view>
#include <vector>

class ClayUi;

namespace Widgets {
struct ButtonStyle {
    Clay_Color background;
    Clay_Color text;
    bool outlined = false;
    uint16_t fontSize;
};
ButtonStyle ElevatedStyle();
ButtonStyle OutlinedStyle();

bool Button(ClayUi& ui, std::string_view id, std::string_view label, Clay_Sizing sizing, bool enabled = true,
            const ButtonStyle& style = ElevatedStyle(), const char* leadingIcon = nullptr);
bool IconButton(ClayUi& ui, std::string_view id, const char* icon, uint16_t size, Clay_Color color, float padding = 8);
bool Switch(ClayUi& ui, std::string_view id, bool value);
void OutlinedField(ClayUi& ui, std::string_view id, std::string_view label);
void Spinner(ClayUi& ui, std::string_view id, float size, Clay_Color color, float thickness = 4);
void CenteredSpinner(ClayUi& ui, std::string_view id);
void TextList(ClayUi& ui, std::string_view id, const std::vector<std::string>* items, float rowHeight = 20);
void SearchField(ClayUi& ui, std::string_view id, std::string* text, const char* hint, float width,
                 std::function<void()> onChange);
void Dropdown(ClayUi& ui, std::string_view id, const std::vector<std::string>* items, int selected, float width,
              std::function<void(int)> onSelect);

void Text(ClayUi& ui, std::string_view text, uint16_t fontSize, Clay_Color color, uint16_t fontId = 0,
          Clay_TextAlignment alignment = CLAY_TEXT_ALIGN_LEFT);
void Gap(float width, float height);
void Spacer();
} // namespace Widgets

class PreviewTexture;

namespace Widgets {
void ImagePreview(ClayUi& ui, std::string_view id, const PreviewTexture* texture);
} // namespace Widgets
