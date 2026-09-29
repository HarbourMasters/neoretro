#pragma once

#include "app/Screen.h"
#include "image/Image.h"
#include "otr/Texture.h"
#include "ui/PreviewTexture.h"

#include <optional>
#include <string>
#include <vector>

class DebugConvertTexturesScreen : public Screen {
  public:
    DebugConvertTexturesScreen();
    void Layout(AppContext& ctx) override;

  private:
    void ConvertTexture(AppContext& ctx);
    void LoadTlut(AppContext& ctx);
    void SaveAs(AppContext& ctx);
    void LoadTexture(AppContext& ctx);
    void UpdatePreview(AppContext& ctx);

    std::vector<std::string> mTypeNames;
    Otr::TextureType mSelectedType = Otr::TextureType::RGBA32bpp;
    std::optional<std::string> mTextureFile;
    std::optional<Otr::Texture> mTexture;
    std::optional<Otr::Texture> mTlut;
    bool mIsPalette = false;
    Image mPreview;
    std::vector<uint8_t> mPreviewPng;
    PreviewTexture mPreviewTexture;
};
