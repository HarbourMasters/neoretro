#include "PreviewTexture.h"

#include <raylib.h>

namespace {
Texture2D ToRaylib(unsigned int id, int width, int height) {
    return { id, width, height, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
}
} // namespace

PreviewTexture::~PreviewTexture() {
    Reset();
}

void PreviewTexture::Upload(const uint8_t* rgba, int width, int height) {
    if (rgba == nullptr || width <= 0 || height <= 0) {
        Reset();
        return;
    }
    if (mId != 0 && width == mWidth && height == mHeight) {
        UpdateTexture(ToRaylib(mId, mWidth, mHeight), rgba);
        return;
    }
    Reset();
    const Image image = { const_cast<uint8_t*>(rgba), width, height, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
    const Texture2D texture = LoadTextureFromImage(image);
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    SetTextureWrap(texture, TEXTURE_WRAP_CLAMP);
    mId = texture.id;
    mWidth = texture.width;
    mHeight = texture.height;
}

void PreviewTexture::Reset() {
    if (mId != 0) {
        UnloadTexture(ToRaylib(mId, mWidth, mHeight));
    }
    mId = 0;
    mWidth = 0;
    mHeight = 0;
}
