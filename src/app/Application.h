#pragma once

#include "AppContext.h"
#include "Router.h"
#include "services/StageManager.h"
#include "ui/Toasts.h"
#include "ui/clay/ClayUi.h"
#include "ui/components/EphemeralBar.h"

#include <filesystem>
#include <memory>
#include <string>

class Application {
  public:
    Application();
    bool Init(const std::string& initialRoute = {});
    void Run();
    void Shutdown();

  private:
    void HandleShortcuts();
    void DrawFrame();

    bool mInitialized = false;

    ClayUi mUi;
    Router mRouter;
    Toasts mToasts;
    StageManager mStage;
    EphemeralBar mEphemeralBar;
    std::unique_ptr<AppContext> mContext;
};
