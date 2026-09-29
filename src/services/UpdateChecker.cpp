#include "UpdateChecker.h"

#include "build.h"
#include "utils/Jobs.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>

#ifdef NEORETRO_UPDATE_CHECK
#include <curl/curl.h>
#endif

namespace UpdateChecker {
namespace {
std::atomic<int> sRemoteVersion = -1;
bool sStarted = false;

#ifdef NEORETRO_UPDATE_CHECK
size_t WriteBody(char* data, size_t size, size_t count, void* user) {
    static_cast<std::string*>(user)->append(data, size * count);
    return size * count;
}

void Fetch() {
    CURL* curl = curl_easy_init();
    if (curl == nullptr) {
        return;
    }
    std::string body;
    curl_easy_setopt(curl, CURLOPT_URL, gReleaseJsonUrl);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteBody);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    const CURLcode result = curl_easy_perform(curl);
    long status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK || status != 200) {
        SPDLOG_WARN("Error fetching version code: {}", result != CURLE_OK ? curl_easy_strerror(result) : "HTTP error");
        return;
    }
    try {
        sRemoteVersion = nlohmann::json::parse(body).at("version").get<int>();
    } catch (const std::exception& e) { SPDLOG_WARN("Error fetching version code: {}", e.what()); }
}
#endif
} // namespace

void Start() {
    if (sStarted || gReleaseJsonUrl[0] == '\0') {
        return;
    }
    sStarted = true;
#ifdef NEORETRO_UPDATE_CHECK
    Jobs::Instance().Run(Fetch);
#endif
}

bool UpdateAvailable() {
    const int remote = sRemoteVersion.load();
    return remote != -1 && (int)gVersionCode < remote;
}
} // namespace UpdateChecker
