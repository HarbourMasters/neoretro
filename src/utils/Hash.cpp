#include "Hash.h"

#include <picosha2.h>

std::string Sha256Hex(const std::vector<uint8_t>& data) {
    return picosha2::hash256_hex_string(data.begin(), data.end());
}
