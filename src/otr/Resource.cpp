#include "Resource.h"

namespace Otr {
std::optional<ResourceHeader> ReadHeader(const std::vector<uint8_t>& data) {
    if (data.size() < kHeaderSize) {
        return std::nullopt;
    }
    if (data[0] > 1) {
        return std::nullopt;
    }
    ResourceHeader header;
    header.bigEndian = data[0] == 1;
    ResourceReader reader(data, header.bigEndian, 0x04);
    header.type = reader.ReadU32();
    header.version = reader.ReadU32();
    const uint64_t lo = reader.ReadU32();
    const uint64_t hi = reader.ReadU32();
    header.id = header.bigEndian ? (lo << 32) | hi : (hi << 32) | lo;
    return header;
}

uint8_t ResourceReader::ReadU8() {
    Require(1);
    return mData[mOffset++];
}

uint16_t ResourceReader::ReadU16() {
    Require(2);
    const uint8_t* p = &mData[mOffset];
    mOffset += 2;
    return mBigEndian ? (uint16_t)(p[0] << 8 | p[1]) : (uint16_t)(p[1] << 8 | p[0]);
}

uint32_t ResourceReader::ReadU32() {
    Require(4);
    const uint8_t* p = &mData[mOffset];
    mOffset += 4;
    if (mBigEndian) {
        return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
    }
    return (uint32_t)p[3] << 24 | (uint32_t)p[2] << 16 | (uint32_t)p[1] << 8 | p[0];
}

float ResourceReader::ReadFloat() {
    const uint32_t bits = ReadU32();
    float value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

std::vector<uint8_t> ResourceReader::ReadBytes(size_t length) {
    Require(length);
    std::vector<uint8_t> out(mData.begin() + (std::ptrdiff_t)mOffset,
                             mData.begin() + (std::ptrdiff_t)(mOffset + length));
    mOffset += length;
    return out;
}
} // namespace Otr
