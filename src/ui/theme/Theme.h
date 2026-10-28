#pragma once

#include <cstdint>
#include <filesystem>

struct ImFont;

enum FontId : uint16_t {
    FONT_REGULAR,
    FONT_MEDIUM,
    FONT_BOLD,
    FONT_MONO,
    FONT_ICONS,
    FONT_COUNT,
};

namespace TextSize {
constexpr uint16_t DisplaySmall = 48;
constexpr uint16_t HeadlineSmall = 24;
constexpr uint16_t TitleLarge = 20;
constexpr uint16_t TitleSmall = 14;
constexpr uint16_t BodyLarge = 16;
constexpr uint16_t BodyMedium = 14;
} // namespace TextSize

namespace Icons {
constexpr const char* AddCircle = "\xee\x85\x87";
constexpr const char* Visibility = "\xee\xa3\xb4";
constexpr const char* WarningRounded = "\xee\x80\x82";
constexpr const char* WarningAmber = "\xef\x82\x83";
constexpr const char* Favorite = "\xee\xa1\xbd";
constexpr const char* Gamepad = "\xee\x8c\x8f";
constexpr const char* ChevronLeft = "\xee\x97\x8b";
constexpr const char* Texture = "\xee\x90\xa1";
constexpr const char* VideogameAsset = "\xee\x8c\xb8";
constexpr const char* SettingsSuggest = "\xef\x81\x9e";
constexpr const char* DirectionsBoatFilled = "\xee\xbf\xb5";
constexpr const char* DarkMode = "\xee\x94\x9c";
constexpr const char* Extension = "\xee\xa1\xbb";
constexpr const char* MusicNote = "\xee\x90\x85";
constexpr const char* LibraryMusic = "\xee\x80\xb0";
constexpr const char* TextFields = "\xee\x89\xa2";
constexpr const char* Search = "\xee\xa2\xb6";
constexpr const char* Close = "\xee\x97\x8d";
constexpr const char* Delete = "\xee\xa1\xb2";
constexpr const char* Memory = "\xee\x8c\xa2";
constexpr const char* ArrowDropDown = "\xee\x97\x85";
constexpr const char* Star = "\xee\xa0\xb8";
constexpr const char* Chat = "\xee\x82\xb7";
constexpr const char* Title = "\xee\x89\xa4";
constexpr const char* Translate = "\xee\xa3\xa2";
} // namespace Icons

namespace Theme {
void Load(const std::filesystem::path& assetsDir);
void ApplyStyle();
ImFont* GetFont(uint16_t id);
} // namespace Theme
