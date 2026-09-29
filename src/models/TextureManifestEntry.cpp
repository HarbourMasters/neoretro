#include "TextureManifestEntry.h"

#include <spdlog/spdlog.h>

namespace {
std::optional<int> OptionalInt(const nlohmann::json& json, const char* key) {
    if (json.contains(key) && json[key].is_number_integer()) {
        return json[key].get<int>();
    }
    return std::nullopt;
}
} // namespace

TextureStrip TextureStrip::FromJson(const nlohmann::json& json) {
    TextureStrip strip;
    strip.y = json.at("y").get<int>();
    strip.rows = json.at("rows").get<int>();
    strip.hashY = OptionalInt(json, "hashY").value_or(strip.y);
    strip.hashRows = OptionalInt(json, "hashRows").value_or(strip.rows);
    strip.x = OptionalInt(json, "x").value_or(0);
    strip.cols = OptionalInt(json, "cols");
    return strip;
}

nlohmann::ordered_json TextureStrip::ToJson() const {
    nlohmann::ordered_json json;
    json["y"] = y;
    json["rows"] = rows;
    if (cols) {
        json["x"] = x;
        json["cols"] = *cols;
    }
    if (hashY != y) {
        json["hashY"] = hashY;
    }
    if (hashRows != rows) {
        json["hashRows"] = hashRows;
    }
    return json;
}

std::optional<TextureManifestEntry> TextureManifestEntry::FromJson(const nlohmann::json& json) {
    try {
        TextureManifestEntry entry;
        entry.hash = json.at("hash").get<std::string>();
        const int type = json.at("textureType").get<int>();
        if (type < 0 || type >= Otr::kTextureTypeCount) {
            return std::nullopt;
        }
        entry.textureType = (Otr::TextureType)type;
        entry.textureWidth = json.at("textureWidth").get<int>();
        entry.textureHeight = json.at("textureHeight").get<int>();
        entry.tileWidth = OptionalInt(json, "tileWidth");
        entry.tileHeight = OptionalInt(json, "tileHeight");
        if (json.contains("strips") && json["strips"].is_array()) {
            std::vector<TextureStrip> strips;
            for (const auto& strip : json["strips"]) {
                strips.push_back(TextureStrip::FromJson(strip));
            }
            entry.strips = std::move(strips);
        }
        return entry;
    } catch (const std::exception& e) {
        SPDLOG_WARN("Invalid manifest entry: {}", e.what());
        return std::nullopt;
    }
}

nlohmann::ordered_json TextureManifestEntry::ToJson() const {
    nlohmann::ordered_json json;
    json["hash"] = hash;
    json["textureType"] = (int)textureType;
    json["textureWidth"] = textureWidth;
    json["textureHeight"] = textureHeight;
    if (tileWidth) {
        json["tileWidth"] = *tileWidth;
    }
    if (tileHeight) {
        json["tileHeight"] = *tileHeight;
    }
    if (strips) {
        auto array = nlohmann::ordered_json::array();
        for (const auto& strip : *strips) {
            array.push_back(strip.ToJson());
        }
        json["strips"] = array;
    }
    return json;
}
