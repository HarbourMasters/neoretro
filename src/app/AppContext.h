#pragma once

#include <filesystem>

class ClayUi;
class Router;
class StageManager;
class Toasts;

struct AppContext {
    ClayUi& ui;
    Router& router;
    StageManager& stage;
    Toasts& toasts;
    std::filesystem::path assetsDir;
};
