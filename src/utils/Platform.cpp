#include "Platform.h"

#include <raylib.h>

namespace Platform {
bool OpenUrl(const char* url) {
    if (url == nullptr || url[0] == '\0') {
        return false;
    }
    OpenURL(url);
    return true;
}

std::filesystem::path ExecutableDir() {
    const char* dir = GetApplicationDirectory();
    return std::filesystem::path(std::u8string(dir, dir + std::char_traits<char>::length(dir)));
}
} // namespace Platform
