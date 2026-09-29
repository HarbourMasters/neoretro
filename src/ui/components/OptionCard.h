#pragma once

#include <functional>
#include <string_view>

class ClayUi;

bool OptionCard(ClayUi& ui, std::string_view id, std::string_view text, const char* icon, bool* hovered = nullptr,
                const std::function<void()>& overlay = {});
