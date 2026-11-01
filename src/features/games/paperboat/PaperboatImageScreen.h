#pragma once

#include "features/games/ImageEditorScreen.h"
#include "games/pm64/Pm64Images.h"

class PaperboatImageScreen : public ImageEditorScreen {
  public:
    explicit PaperboatImageScreen(Pm64::ImageKind kind);
};
