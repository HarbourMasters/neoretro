#pragma once

#include <clay.h>
#include <span>
#include <string_view>

class ClayUi;

void GradientText(ClayUi& ui, std::string_view id, std::string_view text, uint16_t fontId, uint16_t fontSize,
                  std::span<const Clay_Color> colors, float durationSeconds);
