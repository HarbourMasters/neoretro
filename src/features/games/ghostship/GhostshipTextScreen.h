#pragma once

#include "features/games/TextEditorScreen.h"

enum class GhostshipTextKind { Dialogs, Texts, Strings };

class GhostshipTextScreen : public TextEditorScreen {
  public:
    explicit GhostshipTextScreen(GhostshipTextKind kind);
};
