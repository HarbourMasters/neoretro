#include "app/Application.h"
#include "build.h"
#include "utils/Log.h"

#include <cstdlib>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <string>

namespace {
void EnterShipHome() {
    const char* home = std::getenv("SHIP_HOME");
    if (home == nullptr || home[0] == '\0') {
        return;
    }
    std::string path = home;
    if (path[0] == '~') {
        const char* user = std::getenv("HOME");
        path = std::string(user != nullptr ? user : "") + path.substr(1);
    }
    std::error_code ec;
    std::filesystem::create_directories(path, ec);
    std::filesystem::current_path(path, ec);
}
} // namespace

int main(int argc, char* argv[]) {
    EnterShipHome();
    Log::Init();
    SPDLOG_INFO("neoretro {} [{}@{}]", gBuildVersion, gGitBranch, gGitCommitHash);

    std::string initialRoute;
    for (int i = 1; i + 1 < argc; i++) {
        if (std::string(argv[i]) == "--route") {
            initialRoute = argv[i + 1];
        }
    }

    Application app;
    const bool ok = app.Init(initialRoute);
    if (ok) {
        app.Run();
    }
    app.Shutdown();
    return ok ? 0 : 1;
}
