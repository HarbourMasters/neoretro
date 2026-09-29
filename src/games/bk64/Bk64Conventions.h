#pragma once

#include "games/GameTextureConventions.h"

#include <cstdint>
#include <map>
#include <optional>
#include <utility>
#include <vector>

class Bk64TextureConventions : public GameTextureConventions {
  public:
    std::optional<TextureManifestEntry> ResolveAdditiveEntry(const nlohmann::json& manifest,
                                                             const std::string& target) const override;
    void RecordExtractionMetadata(const std::vector<std::string>& otrPaths,
                                  std::map<std::string, TextureManifestEntry>& entries) const override;
};

std::optional<std::map<std::pair<int, int>, std::pair<int, int>>>
ParseSpriteTilePositions(const std::vector<uint8_t>& data);
