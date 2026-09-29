#include "utils/Paths.h"
#include "Theme.h"
#include "Colors.h"

#include <array>
#include <imgui.h>
#include <spdlog/spdlog.h>

namespace {
std::array<ImFont*, FONT_COUNT> sFonts = {};

ImVec4 ToImVec4(Clay_Color color) {
    return { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
}

ImFont* LoadFont(const std::filesystem::path& path) {
    ImGuiIO& io = ImGui::GetIO();
    if (!std::filesystem::exists(path)) {
        SPDLOG_WARN("Font not found: {}, falling back to the default font", Paths::ToUtf8(path));
        return nullptr;
    }
    return io.Fonts->AddFontFromFileTTF(Paths::ToUtf8(path).c_str(), (float)TextSize::BodyLarge);
}
} // namespace

namespace Theme {
void ApplyStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark(&style);

    style.WindowRounding = 8.0f;
    style.FrameRounding = 5.0f;
    style.PopupRounding = 5.0f;
    style.GrabRounding = 5.0f;
    style.TabRounding = 5.0f;
    style.WindowBorderSize = 0.0f;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ToImVec4(RetroColors::BigStone);
    colors[ImGuiCol_ChildBg] = ToImVec4(RetroColors::BigStone);
    colors[ImGuiCol_PopupBg] = ToImVec4(RetroColors::Elephant);
    colors[ImGuiCol_FrameBg] = ToImVec4(RetroColors::Elephant);
    colors[ImGuiCol_FrameBgHovered] = ToImVec4(RetroColors::WithOpacity(RetroColors::TelegramBlue, 0.4f));
    colors[ImGuiCol_FrameBgActive] = ToImVec4(RetroColors::WithOpacity(RetroColors::TelegramBlue, 0.6f));
    colors[ImGuiCol_TitleBg] = ToImVec4(RetroColors::Elephant);
    colors[ImGuiCol_TitleBgActive] = ToImVec4(RetroColors::Elephant);
    colors[ImGuiCol_Button] = ToImVec4(RetroColors::White12);
    colors[ImGuiCol_ButtonHovered] = ToImVec4(RetroColors::White24);
    colors[ImGuiCol_ButtonActive] = ToImVec4(RetroColors::TelegramBlue);
    colors[ImGuiCol_Header] = ToImVec4(RetroColors::WithOpacity(RetroColors::TelegramBlue, 0.3f));
    colors[ImGuiCol_HeaderHovered] = ToImVec4(RetroColors::WithOpacity(RetroColors::TelegramBlue, 0.5f));
    colors[ImGuiCol_HeaderActive] = ToImVec4(RetroColors::TelegramBlue);
    colors[ImGuiCol_CheckMark] = ToImVec4(RetroColors::TelegramBlue);
    colors[ImGuiCol_SliderGrab] = ToImVec4(RetroColors::TelegramBlue);
    colors[ImGuiCol_SliderGrabActive] = ToImVec4(RetroColors::JordyBlue);
}

void Load(const std::filesystem::path& assetsDir) {
    const auto fonts = assetsDir / "fonts";
    sFonts[FONT_REGULAR] = LoadFont(fonts / "GoogleSans-Regular.ttf");
    sFonts[FONT_MEDIUM] = LoadFont(fonts / "GoogleSans-Medium.ttf");
    sFonts[FONT_BOLD] = LoadFont(fonts / "GoogleSans-Bold.ttf");
    sFonts[FONT_MONO] = LoadFont(fonts / "FiraCode-Regular.ttf");
    sFonts[FONT_ICONS] = LoadFont(fonts / "MaterialIcons-Regular.ttf");

    if (sFonts[FONT_REGULAR] == nullptr) {
        sFonts[FONT_REGULAR] = ImGui::GetIO().Fonts->AddFontDefault();
    }
    for (auto& font : sFonts) {
        if (font == nullptr) {
            font = sFonts[FONT_REGULAR];
        }
    }
    ImGui::GetIO().FontDefault = sFonts[FONT_REGULAR];

    ApplyStyle();
}

ImFont* GetFont(uint16_t id) {
    return id < FONT_COUNT ? sFonts[id] : sFonts[FONT_REGULAR];
}
} // namespace Theme
