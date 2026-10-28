#include "Application.h"

#include "l10n/Strings.h"
#include "services/Updater.h"
#include "ui/components/UpdateDialog.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"
#include "utils/FileDialog.h"
#include "utils/Jobs.h"
#include "utils/Paths.h"
#include "utils/Platform.h"

#include <cstdarg>
#include <cstdio>
#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>
#include <spdlog/spdlog.h>

namespace {
constexpr int MIN_WINDOW_SIZE = 700;

std::filesystem::path sAssetsDir;

std::filesystem::path FindAssetsDir() {
    const auto base = Platform::ExecutableDir();
    for (const auto& candidate : { base / "assets", base / ".." / "Resources" / "assets" }) {
        if (std::filesystem::exists(candidate)) {
            return candidate;
        }
    }
    return "assets";
}

void RaylibLog(int level, const char* text, va_list args) {
    char message[1024];
    std::vsnprintf(message, sizeof(message), text, args);
    switch (level) {
        case LOG_ERROR:
        case LOG_FATAL:
            SPDLOG_ERROR("raylib: {}", message);
            break;
        case LOG_WARNING:
            SPDLOG_WARN("raylib: {}", message);
            break;
        case LOG_INFO:
            SPDLOG_DEBUG("raylib: {}", message);
            break;
        default:
            SPDLOG_TRACE("raylib: {}", message);
            break;
    }
}
} // namespace

Application::Application() : mStage(mToasts) {
}

bool Application::Init(const std::string& initialRoute) {
    SetTraceLogCallback(RaylibLog);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(1024, 768, "neoretro");
    if (!IsWindowReady()) {
        SPDLOG_CRITICAL("Failed to create the window");
        return false;
    }
    SetWindowMinSize(MIN_WINDOW_SIZE, MIN_WINDOW_SIZE);
    SetExitKey(KEY_NULL);

    sAssetsDir = FindAssetsDir();
    SPDLOG_INFO("Assets directory: {}", Paths::ToUtf8(sAssetsDir));
    rlImGuiSetLoadFontsCallback([] { Theme::Load(sAssetsDir); });
    rlImGuiSetup(true);
    Theme::ApplyStyle();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = "imgui.ini";
    mInitialized = true;

    L10n::Init();
    FileDialog::Init();

    if (!mUi.Init()) {
        return false;
    }

    mContext = std::make_unique<AppContext>(AppContext{ mUi, mRouter, mStage, mToasts, sAssetsDir });
    RegisterRoutes(mRouter);
    mRouter.Push("/");
    if (!initialRoute.empty() && initialRoute != "/") {
        mRouter.Push(initialRoute);
    }
    mRouter.Apply(*mContext);
    return true;
}

void Application::Run() {
    Updater::Start();
    while (!WindowShouldClose() && !Updater::QuitRequested()) {
        Jobs::Instance().Update();
        Updater::Tick();
        DrawFrame();
        mRouter.Apply(*mContext);
    }
}

void Application::HandleShortcuts() {
    if (ImGui::IsKeyPressed(ImGuiKey_F12, false)) {
        mUi.ToggleDebug();
    }
    if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) && !ImGui::GetIO().WantTextInput) {
        if (mStage.isEphemeralBarExpanded) {
            mStage.ToggleEphemeralBar();
        } else {
            mRouter.Pop();
        }
    }
}

void Application::DrawFrame() {
    BeginDrawing();
    const Clay_Color& bg = RetroColors::BigStone;
    ClearBackground({ (unsigned char)bg.r, (unsigned char)bg.g, (unsigned char)bg.b, 255 });
    rlImGuiBegin();
    HandleShortcuts();

    Screen* screen = mRouter.Current();

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    constexpr ImGuiWindowFlags hostFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                           ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                           ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::Begin("##neoretro_root", nullptr, hostFlags);
    ImGui::PopStyleVar(2);

    mUi.BeginLayout();
    CLAY({ .id = CLAY_ID("Root"),
           .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) }, .layoutDirection = CLAY_TOP_TO_BOTTOM },
           .backgroundColor = RetroColors::BigStone }) {
        CLAY({ .id = CLAY_ID("ScreenArea"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } },
               .clip = { .horizontal = true, .vertical = true } }) {
            if (screen != nullptr) {
                screen->Layout(*mContext);
            }
        }
        CLAY({ .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(EphemeralBar::kHeight) } } }) {
        }
        mEphemeralBar.Layout(*mContext, viewport->WorkSize.y);
        mToasts.Layout(mUi);
        UpdateDialog::Layout(mUi);
    }
    mUi.EndLayout();

    ImGui::End();

    if (screen != nullptr) {
        screen->DrawImGui(*mContext);
    }
    UpdateDialog::BlockInputBehind();

    rlImGuiEnd();
    EndDrawing();
}

void Application::Shutdown() {
    Jobs::Instance().WaitAll();
    FileDialog::Shutdown();
    mUi.Shutdown();

    if (mInitialized) {
        rlImGuiShutdown();
    }
    if (IsWindowReady()) {
        CloseWindow();
    }
}
