#pragma once

#include "otr/Types.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

enum class TextureEntryKind {
    Replacement,
    Additive,
    AdditiveFontGlyph,
};

struct TextureStrip {
    int y = 0;
    int rows = 0;
    int hashY = 0;
    int hashRows = 0;
    int x = 0;
    std::optional<int> cols;

    static TextureStrip FromJson(const nlohmann::json& json);
    nlohmann::ordered_json ToJson() const;
};

struct TextureManifestEntry {
    std::string hash;
    Otr::TextureType textureType = Otr::TextureType::Error;
    int textureWidth = 0;
    int textureHeight = 0;
    std::optional<int> tileWidth;
    std::optional<int> tileHeight;
    std::optional<std::vector<TextureStrip>> strips;
    TextureEntryKind kind = TextureEntryKind::Replacement;
    std::optional<std::string> targetName;
    std::optional<int> stripIndex;
    std::optional<int> fitWidth;
    std::optional<int> fitHeight;

    static std::optional<TextureManifestEntry> FromJson(const nlohmann::json& json);
    nlohmann::ordered_json ToJson() const;
};
