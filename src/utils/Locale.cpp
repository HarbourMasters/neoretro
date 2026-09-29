#include "Platform.h"

#include <algorithm>
#include <cstdlib>
#include <sstream>

#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace Platform {
namespace {
std::string LanguageOf(std::string tag) {
    const auto end = tag.find_first_of("-_.@");
    tag = tag.substr(0, end);
    std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
    return tag;
}

void Add(std::vector<std::string>& languages, const std::string& tag) {
    const std::string language = LanguageOf(tag);
    if (!language.empty() && language != "c" && language != "posix" &&
        std::find(languages.begin(), languages.end(), language) == languages.end()) {
        languages.push_back(language);
    }
}
} // namespace

std::vector<std::string> PreferredLanguages() {
    std::vector<std::string> languages;
#if defined(__APPLE__)
    if (CFArrayRef preferred = CFLocaleCopyPreferredLanguages()) {
        for (CFIndex i = 0; i < CFArrayGetCount(preferred); i++) {
            auto tag = (CFStringRef)CFArrayGetValueAtIndex(preferred, i);
            char buffer[64];
            if (CFStringGetCString(tag, buffer, sizeof(buffer), kCFStringEncodingUTF8)) {
                Add(languages, buffer);
            }
        }
        CFRelease(preferred);
    }
#elif defined(_WIN32)
    ULONG count = 0;
    ULONG size = 0;
    if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &count, nullptr, &size) && size > 0) {
        std::wstring buffer(size, L'\0');
        if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &count, buffer.data(), &size)) {
            for (const wchar_t* tag = buffer.c_str(); *tag != L'\0'; tag += wcslen(tag) + 1) {
                std::string narrow;
                for (const wchar_t* c = tag; *c != L'\0'; c++) {
                    narrow += (char)*c;
                }
                Add(languages, narrow);
            }
        }
    }
#else
    if (const char* list = std::getenv("LANGUAGE")) {
        std::stringstream stream(list);
        for (std::string tag; std::getline(stream, tag, ':');) {
            Add(languages, tag);
        }
    }
    for (const char* var : { "LC_ALL", "LC_MESSAGES", "LANG" }) {
        if (const char* value = std::getenv(var)) {
            Add(languages, value);
        }
    }
#endif
    return languages;
}
} // namespace Platform
