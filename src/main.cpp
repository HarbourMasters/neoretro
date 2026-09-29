#include "app/Application.h"
#include "build.h"
#include "utils/Log.h"

#include <spdlog/spdlog.h>
#include <string>

int main(int argc, char* argv[]) {
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
