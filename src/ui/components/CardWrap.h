#pragma once

#include <functional>
#include <string_view>
#include <cstdint>

class ClayUi;

void CenteredCards(ClayUi& ui, std::string_view id, uint16_t spacing, const std::function<void()>& cards);
