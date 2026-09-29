#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace L10n {
struct Locale {
    const char* code;
    std::unordered_map<std::string, const char*> strings;
};

namespace Generated {
extern const std::vector<Locale> kLocales;
}

void Init();
void SetLocale(std::string_view code);
std::string_view CurrentLocale();

const char* Tr(std::string_view key);
} // namespace L10n

using L10n::Tr;
