#pragma once

#include "app/Screen.h"
#include "services/FontGenerator.h"
#include "ui/PreviewTexture.h"

#include <tuple>

class DebugFontGeneratorScreen : public Screen {
  public:
    void Layout(AppContext& ctx) override;

  private:
    void RefreshPreview();
    void WriteFontTable(AppContext& ctx);

    FontGenerator mGenerator;
    FontGenerator::Settings mSettings;
    PreviewTexture mPreview;
    std::tuple<float, float, int, int, int> mPreviewKey = { -1.0f, -1.0f, -1, -1, -1 };
};
