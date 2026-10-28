#pragma once

#include <functional>
#include <string_view>
#include <vector>
#include <cstdint>

class ClayUi;

void CenteredCards(ClayUi& ui, std::string_view id, uint16_t spacing, const std::function<void()>& cards);
void CenteredCardRows(ClayUi& ui, std::string_view id, uint16_t spacing,
                      const std::vector<std::function<void()>>& rows);
