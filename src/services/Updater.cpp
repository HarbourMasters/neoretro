#include "Updater.h"

#include "build.h"
#include "otr/Archive.h"
#include "utils/Hash.h"
#include "utils/Jobs.h"
#include "utils/Paths.h"
#include "utils/Platform.h"

#include <atomic>
#include <chrono>
#include <fstream>
#include <memory>
#include <optional>
#include <vector>
#include <cstdlib>
#include <nlohmann/json.hpp>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#ifdef NEORETRO_UPDATE_CHECK
#include <curl/curl.h>
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
extern char** environ;
#endif

namespace Updater {
namespace {
using Clock = std::chrono::steady_clock;

constexpr auto kFirstCheckDelay = std::chrono::seconds(3);
constexpr auto kCheckInterval = std::chrono::hours(24);
const char* kSettingsFile = "updater.json";

Status sStatus = Status::Idle;
bool sDialog = false;
bool sStarted = false;
bool sBusy = false;
bool sQuit = false;
bool sAutoCheck = true;
std::string sSkippedVersion;
std::string sMessage;
UpdateFeed::Release sPending;
std::atomic<float> sProgress = 0.0f;
Clock::time_point sNextCheck;

void LoadSettings() {
    const auto text = Paths::ReadText(kSettingsFile);
    if (!text) {
        return;
    }
    const auto json = nlohmann::json::parse(*text, nullptr, false);
    if (json.is_object()) {
        sAutoCheck = json.value("autoCheck", true);
        sSkippedVersion = json.value("skippedVersion", "");
    }
}

void SaveSettings() {
    const nlohmann::json json = { { "autoCheck", sAutoCheck }, { "skippedVersion", sSkippedVersion } };
    Paths::WriteText(kSettingsFile, json.dump(4));
}

void Finish(Status status, std::string message, bool dialog) {
    sStatus = status;
    sMessage = std::move(message);
    sDialog = dialog;
}

#ifdef NEORETRO_UPDATE_CHECK
struct Response {
    long status = 0;
    std::string body;
    std::string error;
};

size_t WriteString(char* data, size_t size, size_t count, void* user) {
    static_cast<std::string*>(user)->append(data, size * count);
    return size * count;
}

size_t WriteStream(char* data, size_t size, size_t count, void* user) {
    auto* stream = static_cast<std::ofstream*>(user);
    stream->write(data, (std::streamsize)(size * count));
    return *stream ? size * count : 0;
}

int OnProgress(void* user, curl_off_t total, curl_off_t now, curl_off_t, curl_off_t) {
    if (total > 0) {
        static_cast<std::atomic<float>*>(user)->store((float)now / (float)total);
    }
    return 0;
}

Response Fetch(const std::string& url, std::ofstream* file) {
    Response response;
    CURL* curl = curl_easy_init();
    if (curl == nullptr) {
        response.error = "Could not start the download.";
        return response;
    }
    const std::string agent = "neoretro/" + CurrentVersion();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, agent.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 20L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 60L);
    if (file != nullptr) {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteStream);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, OnProgress);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &sProgress);
    } else {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);
    }
    const CURLcode result = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status);
    curl_easy_cleanup(curl);
    if (result != CURLE_OK) {
        response.error = curl_easy_strerror(result);
    }
    return response;
}
#endif

[[maybe_unused]] bool IsWritableDir(const fs::path& dir) {
    const fs::path probe = dir / ".neoretro-update-probe";
    std::error_code ec;
    {
        std::ofstream stream(probe, std::ios::binary);
        if (!stream) {
            return false;
        }
    }
    fs::remove(probe, ec);
    return true;
}

#ifdef __APPLE__
fs::path BundlePath() {
    fs::path macos = Platform::ExecutableDir();
    if (!macos.has_filename()) {
        macos = macos.parent_path();
    }
    const fs::path bundle = macos.parent_path().parent_path();
    return macos.filename() == "MacOS" && bundle.extension() == ".app" ? bundle : fs::path();
}

std::string PlistString(const fs::path& plist, const std::string& key) {
    const auto text = Paths::ReadText(plist);
    if (!text) {
        return {};
    }
    const auto at = text->find("<key>" + key + "</key>");
    if (at == std::string::npos) {
        return {};
    }
    const auto open = text->find("<string>", at);
    const auto close = text->find("</string>", open);
    if (open == std::string::npos || close == std::string::npos) {
        return {};
    }
    return text->substr(open + 8, close - open - 8);
}
#endif

fs::path InstallTarget() {
#if defined(_WIN32)
    return Platform::ExecutableDir();
#elif defined(__APPLE__)
    return BundlePath();
#else
    const char* appImage = std::getenv("APPIMAGE");
    return appImage != nullptr ? fs::path(appImage) : fs::path();
#endif
}

#ifndef _WIN32
std::string ShellQuote(const std::string& text) {
    std::string quoted = "'";
    for (const char c : text) {
        quoted += c == '\'' ? std::string("'\\''") : std::string(1, c);
    }
    return quoted + "'";
}

int RunAndWait(const std::vector<std::string>& args) {
    std::vector<char*> argv;
    for (const auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);
    pid_t pid = 0;
    if (posix_spawn(&pid, argv[0], nullptr, nullptr, argv.data(), environ) != 0) {
        return -1;
    }
    int status = 0;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

bool SpawnDetached(const std::string& script) {
    posix_spawnattr_t attr;
    posix_spawnattr_init(&attr);
    posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSID);
    posix_spawn_file_actions_t actions;
    posix_spawn_file_actions_init(&actions);
    posix_spawn_file_actions_addopen(&actions, 0, "/dev/null", O_RDONLY, 0);
    posix_spawn_file_actions_addopen(&actions, 1, "/dev/null", O_WRONLY, 0);
    posix_spawn_file_actions_addopen(&actions, 2, "/dev/null", O_WRONLY, 0);
    const char* argv[] = { "/bin/sh", "-c", script.c_str(), nullptr };
    pid_t pid = 0;
    const int result = posix_spawn(&pid, "/bin/sh", &actions, &attr, const_cast<char**>(argv), environ);
    posix_spawn_file_actions_destroy(&actions);
    posix_spawnattr_destroy(&attr);
    return result == 0;
}
#else
bool SpawnDetached(const fs::path& script) {
    std::wstring command = L"cmd.exe /c \"\"" + script.wstring() + L"\"\"";
    STARTUPINFOW startup = { sizeof(startup) };
    PROCESS_INFORMATION process = {};
    if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE,
                        DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP | CREATE_NO_WINDOW, nullptr, nullptr, &startup,
                        &process)) {
        return false;
    }
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    return true;
}

std::optional<fs::path> ExtractZip(const fs::path& zip, const fs::path& into) {
    auto archive = Otr::ArchiveReader::Open(Paths::ToUtf8(zip));
    if (!archive) {
        return std::nullopt;
    }
    for (const auto& name : archive->List()) {
        const fs::path relative = Paths::FromUtf8(name).lexically_normal();
        if (relative.is_absolute() || relative.has_root_name() || (!relative.empty() && *relative.begin() == "..")) {
            return std::nullopt;
        }
        const auto data = archive->Read(name);
        if (!data || !Paths::WriteFile(into / relative, *data)) {
            return std::nullopt;
        }
    }
    if (fs::exists(into / "neoretro.exe")) {
        return into;
    }
    for (const auto& entry : fs::directory_iterator(into)) {
        if (entry.is_directory() && fs::exists(entry.path() / "neoretro.exe")) {
            return entry.path();
        }
    }
    return std::nullopt;
}
#endif

std::optional<std::string> Download(const UpdateFeed::Release& release, const fs::path& file) {
#ifdef NEORETRO_UPDATE_CHECK
    std::ofstream stream(file, std::ios::binary);
    if (!stream) {
        return "Could not write the download to " + Paths::ToUtf8(file) + ".";
    }
    const auto response = Fetch(release.url, &stream);
    stream.close();
    if (!response.error.empty()) {
        return "The download failed: " + response.error;
    }
    if (response.status < 200 || response.status >= 300) {
        return fmt::format("The download failed ({}).", response.status);
    }
    const auto data = Paths::ReadFile(file);
    if (!data || Sha256Hex(*data) != release.sha256) {
        return std::string("The download didn't match its checksum, so it was not installed.");
    }
    return std::nullopt;
#else
    return std::string("This build can't download updates.");
#endif
}

std::optional<std::string> InstallWork(const UpdateFeed::Release& release) {
    const fs::path target = InstallTarget();
    std::error_code ec;
    const fs::path work =
        fs::temp_directory_path(ec) /
        fmt::format("neoretro-update-{}", std::chrono::system_clock::now().time_since_epoch().count());
    fs::create_directories(work, ec);
    if (ec) {
        return "Could not create " + Paths::ToUtf8(work) + ".";
    }
    const auto cleanup = [&](std::string error) -> std::optional<std::string> {
        fs::remove_all(work, ec);
        SPDLOG_ERROR("Update failed: {}", error);
        return error;
    };

#if defined(_WIN32)
    const fs::path zip = work / "update.zip";
    if (auto error = Download(release, zip)) {
        return cleanup(*error);
    }
    const auto root = ExtractZip(zip, work / "new");
    if (!root) {
        return cleanup("The downloaded archive doesn't contain neoretro.");
    }
    const fs::path script = fs::temp_directory_path(ec) / "neoretro-update.cmd";
    const std::string text = fmt::format(
        "@echo off\r\n"
        "chcp 65001 >NUL\r\n"
        "set \"target={}\"\r\n"
        "set \"new={}\"\r\n"
        "set \"work={}\"\r\n"
        "set \"pid={}\"\r\n"
        "set /a tries=0\r\n"
        ":wait\r\n"
        "tasklist /FI \"PID eq %pid%\" 2>NUL | find \"%pid%\" >NUL\r\n"
        "if not errorlevel 1 if %tries% LSS 100 (set /a tries+=1 & ping -n 2 127.0.0.1 >NUL & goto wait)\r\n"
        "robocopy \"%new%\" \"%target%\" /E /R:10 /W:1 /NFL /NDL /NJH /NJS /NP >NUL\r\n"
        "start \"\" \"%target%\\neoretro.exe\"\r\n"
        "cd /d \"%TEMP%\"\r\n"
        "rmdir /S /Q \"%work%\"\r\n"
        "del \"%~f0\"\r\n",
        Paths::ToUtf8(target), Paths::ToUtf8(*root), Paths::ToUtf8(work), GetCurrentProcessId());
    if (!Paths::WriteText(script, text) || !SpawnDetached(script)) {
        return cleanup("Could not start the installer.");
    }
#elif defined(__APPLE__)
    const fs::path zip = work / "update.zip";
    if (auto error = Download(release, zip)) {
        return cleanup(*error);
    }
    const fs::path unpacked = work / "new";
    if (RunAndWait({ "/usr/bin/ditto", "-xk", Paths::ToUtf8(zip), Paths::ToUtf8(unpacked) }) != 0) {
        return cleanup("The downloaded archive could not be unpacked.");
    }
    fs::path app;
    for (const auto& entry : fs::directory_iterator(unpacked, ec)) {
        if (entry.path().extension() == ".app") {
            app = entry.path();
        }
    }
    if (app.empty()) {
        return cleanup("The downloaded archive doesn't contain an app.");
    }
    const fs::path plist = app / "Contents" / "Info.plist";
    if (PlistString(plist, "CFBundleIdentifier") !=
        PlistString(target / "Contents" / "Info.plist", "CFBundleIdentifier")) {
        return cleanup("The downloaded app isn't neoretro.");
    }
    if (!UpdateFeed::IsNewer(PlistString(plist, "CFBundleShortVersionString"), CurrentVersion())) {
        return cleanup("The downloaded app isn't newer than this one.");
    }
    if (RunAndWait({ "/usr/bin/codesign", "--verify", "--deep", "--strict", Paths::ToUtf8(app) }) != 0) {
        return cleanup("The downloaded app's signature is damaged.");
    }
    const std::string script = fmt::format(
        "target={0}; new={1}; pid={2}; work={3}\n"
        "for _ in $(seq 1 100); do kill -0 \"$pid\" 2>/dev/null || break; sleep 0.2; done\n"
        "if ditto \"$new\" \"$target.updating\"; then\n"
        "  rm -rf \"$target\" && mv \"$target.updating\" \"$target\"\n"
        "else\n"
        "  rm -rf \"$target.updating\"\n"
        "fi\n"
        "open \"$target\"\n"
        "rm -rf \"$work\"\n",
        ShellQuote(Paths::ToUtf8(target)), ShellQuote(Paths::ToUtf8(app)), getpid(), ShellQuote(Paths::ToUtf8(work)));
    if (!SpawnDetached(script)) {
        return cleanup("Could not start the installer.");
    }
#else
    const fs::path staged = fs::path(target).concat(".updating");
    if (auto error = Download(release, staged)) {
        fs::remove(staged, ec);
        return cleanup(*error);
    }
    fs::permissions(staged,
                    fs::perms::owner_all | fs::perms::group_read | fs::perms::group_exec | fs::perms::others_read |
                        fs::perms::others_exec,
                    ec);
    const std::string script =
        fmt::format("target={0}; staged={1}; pid={2}; work={3}\n"
                    "for _ in $(seq 1 100); do kill -0 \"$pid\" 2>/dev/null || break; sleep 0.2; done\n"
                    "mv -f \"$staged\" \"$target\" || rm -f \"$staged\"\n"
                    "rm -rf \"$work\"\n"
                    "exec \"$target\"\n",
                    ShellQuote(Paths::ToUtf8(target)), ShellQuote(Paths::ToUtf8(staged)), getpid(),
                    ShellQuote(Paths::ToUtf8(work)));
    if (!SpawnDetached(script)) {
        fs::remove(staged, ec);
        return cleanup("Could not start the installer.");
    }
#endif
    SPDLOG_INFO("Installing neoretro {}, relaunching", release.version);
    return std::nullopt;
}
} // namespace

void Start() {
    if (sStarted) {
        return;
    }
    sStarted = true;
    LoadSettings();
    sNextCheck = Clock::now() + kFirstCheckDelay;
}

void Tick() {
    if (sStarted && Clock::now() >= sNextCheck) {
        sNextCheck = Clock::now() + kCheckInterval;
        Check(true);
    }
}

void Check(bool silent) {
    if (sBusy || (silent && !sAutoCheck)) {
        return;
    }
    if (gUpdateFeedUrl[0] == '\0') {
        if (!silent) {
            Finish(Status::Failed, "Updates are turned off in this build.", true);
        }
        return;
    }
#ifdef NEORETRO_UPDATE_CHECK
    sBusy = true;
    sStatus = Status::Checking;
    sDialog = !silent;
    auto response = std::make_shared<Response>();
    Jobs::Instance().Run([response] { *response = Fetch(gUpdateFeedUrl, nullptr); },
                         [response, silent] {
                             sBusy = false;
                             if (response->status == 404) {
                                 Finish(silent ? Status::Idle : Status::UpToDate, "No release has been published yet.",
                                        !silent);
                                 return;
                             }
                             std::string error = response->error;
                             std::optional<UpdateFeed::Release> release;
                             if (error.empty() && (response->status < 200 || response->status >= 300)) {
                                 error = fmt::format("The update server answered {}.", response->status);
                             }
                             if (error.empty()) {
                                 release = UpdateFeed::Parse(response->body, UpdateFeed::PlatformKey(), &error);
                             }
                             if (!release) {
                                 SPDLOG_WARN("Update check failed: {}", error);
                                 Finish(silent ? Status::Idle : Status::Failed, error, !silent);
                                 return;
                             }
                             if (!UpdateFeed::IsNewer(release->version, CurrentVersion())) {
                                 Finish(silent ? Status::Idle : Status::UpToDate,
                                        fmt::format("neoretro {} is the latest version.", CurrentVersion()), !silent);
                                 return;
                             }
                             sPending = *release;
                             const bool skipped = silent && sSkippedVersion == release->version;
                             Finish(Status::Available, "", !skipped);
                         });
#else
    if (!silent) {
        Finish(Status::Failed, "This build can't check for updates.", true);
    }
#endif
}

void Install() {
    if (sBusy || sStatus != Status::Available) {
        return;
    }
    if (!CanInstall()) {
        OpenReleasesPage();
        return;
    }
    sBusy = true;
    sProgress = 0.0f;
    Finish(Status::Downloading, "", true);
    auto error = std::make_shared<std::optional<std::string>>();
    Jobs::Instance().Run([error, release = sPending] { *error = InstallWork(release); },
                         [error] {
                             sBusy = false;
                             if (*error) {
                                 Finish(Status::Failed, **error, true);
                             } else {
                                 Finish(Status::Installing, "", true);
                                 sQuit = true;
                             }
                         });
}

void Later() {
    sDialog = false;
}

void Skip() {
    sSkippedVersion = sPending.version;
    SaveSettings();
    Finish(Status::Idle, "", false);
}

void Close() {
    if (sStatus == Status::Failed && !sPending.version.empty()) {
        Finish(Status::Available, "", false);
    } else if (sStatus != Status::Available) {
        Finish(Status::Idle, "", false);
    } else {
        sDialog = false;
    }
}

void ShowDialog() {
    if (sStatus == Status::Available) {
        sDialog = true;
    }
}

void OpenReleasesPage() {
    Platform::OpenUrl(gReleasesUrl);
}

Status GetStatus() {
    return sStatus;
}

bool DialogVisible() {
    return sDialog;
}

bool UpdateAvailable() {
    return sStatus == Status::Available;
}

bool CanInstall() {
#if defined(NEORETRO_SELF_UPDATE) && defined(NEORETRO_UPDATE_CHECK)
    const fs::path target = InstallTarget();
    if (target.empty()) {
        return false;
    }
#ifdef _WIN32
    return IsWritableDir(target);
#else
    return IsWritableDir(target.parent_path());
#endif
#else
    return false;
#endif
}

bool QuitRequested() {
    return sQuit;
}

float Progress() {
    return sProgress.load();
}

const UpdateFeed::Release& Pending() {
    return sPending;
}

const std::string& Message() {
    return sMessage;
}

std::string CurrentVersion() {
    return fmt::format("{}.{}.{}", gBuildVersionMajor, gBuildVersionMinor, gBuildVersionPatch);
}

bool AutoCheck() {
    return sAutoCheck;
}

void SetAutoCheck(bool enabled) {
    sAutoCheck = enabled;
    SaveSettings();
}
} // namespace Updater
