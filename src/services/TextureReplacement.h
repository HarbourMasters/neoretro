#pragma once

#include "models/StageEntry.h"

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace TextureReplacement {
inline constexpr const char* kFontTextureName = "textures/font/sGfxPrintFontData";

using ProcessedFolder = std::map<std::string, std::vector<TexturePair>>;
using ExtractedTextures = std::map<std::string, TextureManifestEntry>;

std::optional<ProcessedFolder> ProcessFolder(const std::filesystem::path& folder);

std::optional<ExtractedTextures> ProcessOtr(const std::vector<std::string>& otrPaths,
                                            const std::filesystem::path& outputDir);

std::optional<TextureManifestEntry> DumpFont(const std::filesystem::path& outputPath,
                                             const std::filesystem::path& assetsDir);

bool WriteManifest(const std::filesystem::path& path, const ExtractedTextures& entries);

std::string OutputFolderName(const std::string& otrPath);

std::vector<std::filesystem::path> ListInGlidenOrder(const std::filesystem::path& folder);

std::optional<int> BandIndexOf(const std::string& target);
std::optional<TextureManifestEntry> ResolveTextureEntry(const nlohmann::json& manifest, const std::string& target);
} // namespace TextureReplacement
