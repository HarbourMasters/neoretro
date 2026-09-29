#pragma once

#include <functional>
#include <string_view>

class ClayUi;

struct ScaffoldOptions {
    std::string_view id = "Scaffold";
    std::string_view title;
    std::string_view subtitle;
    std::function<void()> onBackButtonPressed;
    std::function<void()> topRightWidget;
};

void CustomScaffold(ClayUi& ui, const ScaffoldOptions& options, const std::function<void()>& content);
