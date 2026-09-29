#pragma once

#include "app/Screen.h"

class GameSelectionScreen : public Screen {
  public:
    void Layout(AppContext& ctx) override;
};

class SohGameScreen : public Screen {
  public:
    void Layout(AppContext& ctx) override;
};

class Ship2GameScreen : public Screen {
  public:
    void Layout(AppContext& ctx) override;
};
