#pragma once

namespace Pm64 {
struct SpriteSheetName {
    const char* group;
    const char* sheet;
    const char* const* rasters;
    int rasterCount;
};

extern const SpriteSheetName kNpcSpriteNames[];
extern const int kNpcSpriteNameCount;
extern const SpriteSheetName kPlayerSpriteNames[];
extern const int kPlayerSpriteNameCount;
} // namespace Pm64
