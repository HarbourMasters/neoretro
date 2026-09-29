#pragma once

#include "image/Image.h"

#include <filesystem>
#include <optional>
#include <vector>

class FontGenerator {
  public:
    struct Settings {
        float offset = 0.0f;
        float scale = 1.0f;
        int glyphWidth = 32;
        int glyphHeight = 32;
        int perRow = 18;
        int rows = 32;
        bool drawGrid = true;
    };

    bool Load(const std::filesystem::path& ttf);
    bool IsLoaded() const {
        return !mFontData.empty();
    }
    const std::string& FontName() const {
        return mFontName;
    }

    Image Render(const Settings& settings, int width, int height) const;
    Image RenderTable(const Settings& settings) const;

  private:
    std::vector<uint8_t> mFontData;
    std::string mFontName;
};
