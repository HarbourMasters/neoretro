#pragma once

#include "models/TextureManifestEntry.h"

#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

class GameTextureConventions {
  public:
    virtual ~GameTextureConventions() = default;

    virtual std::optional<TextureManifestEntry> ResolveAdditiveEntry(const nlohmann::json& manifest,
                                                                     const std::string& target) const = 0;
    virtual void RecordExtractionMetadata(const std::vector<std::string>& otrPaths,
                                          std::map<std::string, TextureManifestEntry>& entries) const = 0;
};

const std::vector<std::unique_ptr<GameTextureConventions>>& AllGameTextureConventions();
