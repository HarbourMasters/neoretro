#pragma once

#include <cstdint>

class PreviewTexture {
  public:
    PreviewTexture() = default;
    ~PreviewTexture();
    PreviewTexture(const PreviewTexture&) = delete;
    PreviewTexture& operator=(const PreviewTexture&) = delete;

    void Upload(const uint8_t* rgba, int width, int height);
    void Reset();

    bool Valid() const {
        return mId != 0;
    }
    uint64_t Id() const {
        return mId;
    }
    int Width() const {
        return mWidth;
    }
    int Height() const {
        return mHeight;
    }

  private:
    unsigned int mId = 0;
    int mWidth = 0;
    int mHeight = 0;
};
