#include "Strings.h"

#include "utils/Platform.h"

#include <spdlog/spdlog.h>

namespace L10n {
namespace {
const Locale* sCurrent = nullptr;
const Locale* sFallback = nullptr;

const Locale* Find(std::string_view code) {
    for (const auto& locale : Generated::kLocales) {
        if (code == locale.code) {
            return &locale;
        }
    }
    return nullptr;
}
} // namespace

void Init() {
    sFallback = Find("en");
    sCurrent = sFallback;

    for (const auto& language : Platform::PreferredLanguages()) {
        if (const Locale* match = Find(language)) {
            sCurrent = match;
            break;
        }
    }
    SPDLOG_INFO("Locale: {}", sCurrent != nullptr ? sCurrent->code : "none");
}

void SetLocale(std::string_view code) {
    if (const Locale* match = Find(code)) {
        sCurrent = match;
    }
}

std::string_view CurrentLocale() {
    return sCurrent != nullptr ? sCurrent->code : "en";
}

const char* Tr(std::string_view key) {
    const std::string k(key);
    for (const Locale* locale : { sCurrent, sFallback }) {
        if (locale == nullptr) {
            continue;
        }
        if (auto it = locale->strings.find(k); it != locale->strings.end()) {
            return it->second;
        }
    }
    SPDLOG_WARN("Missing translation: {}", k);
    static std::unordered_map<std::string, std::string> sMissing;
    return sMissing.emplace(k, k).first->second.c_str();
}
} // namespace L10n
