#pragma once

#include "Types.h"

#include <cstdint>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <vector>

namespace Otr {
constexpr size_t kHeaderSize = 0x40;

struct ResourceHeader {
    bool bigEndian = false;
    uint32_t type = 0;
    uint32_t version = 0;
    uint64_t id = 0;
};

std::optional<ResourceHeader> ReadHeader(const std::vector<uint8_t>& data);

class ResourceReader {
  public:
    ResourceReader(const std::vector<uint8_t>& data, bool bigEndian, size_t offset = kHeaderSize)
        : mData(data), mBigEndian(bigEndian), mOffset(offset) {
    }

    uint8_t ReadU8();
    uint16_t ReadU16();
    uint32_t ReadU32();
    int16_t ReadS16() {
        return (int16_t)ReadU16();
    }
    float ReadFloat();
    std::vector<uint8_t> ReadBytes(size_t length);
    size_t Offset() const {
        return mOffset;
    }
    size_t Remaining() const {
        return mOffset <= mData.size() ? mData.size() - mOffset : 0;
    }

  private:
    void Require(size_t length) const {
        if (mOffset + length > mData.size()) {
            throw std::out_of_range("Resource data ended early");
        }
    }

    const std::vector<uint8_t>& mData;
    bool mBigEndian;
    size_t mOffset;
};
} // namespace Otr
