#include "Archive.h"

#include <algorithm>
#include <archive/SWrapper.h>
#include <archive/ZWrapper.h>
#include <filesystem>
#include <spdlog/spdlog.h>

#include <cstring>
#include <fstream>
#include <unordered_map>
#include <zlib.h>

namespace Otr {
namespace {
bool IsOtrPath(const std::string& path) {
    auto ext = std::filesystem::path(path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".otr";
}

bool IsReservedMpqName(const std::string& name) {
    return name == "(signature)" || name == "(listfile)" || name == "(attributes)";
}

class ZipReader : public ArchiveReader {
  public:
    static std::unique_ptr<ZipReader> Open(const std::string& path) {
        std::ifstream stream(std::filesystem::path(std::u8string(path.begin(), path.end())), std::ios::binary);
        if (!stream) {
            return nullptr;
        }
        auto reader = std::unique_ptr<ZipReader>(new ZipReader());
        reader->mData.assign(std::istreambuf_iterator<char>(stream), {});
        if (!reader->ReadCentralDirectory()) {
            SPDLOG_ERROR("{} is not a valid O2R (zip) archive", path);
            return nullptr;
        }
        return reader;
    }

    std::vector<std::string> List() override {
        std::vector<std::string> names;
        names.reserve(mEntries.size());
        for (const auto& entry : mEntries) {
            if (!entry.name.empty() && entry.name.back() != '/') {
                names.push_back(entry.name);
            }
        }
        return names;
    }

    std::optional<std::vector<uint8_t>> Read(const std::string& name) override {
        const auto found = mIndex.find(name);
        if (found == mIndex.end()) {
            return std::nullopt;
        }
        const Entry& entry = mEntries[found->second];
        const uint64_t local = entry.localHeaderOffset;
        if (local + 30 > mData.size() || U32(local) != 0x04034b50) {
            SPDLOG_WARN("Skipping file {}: bad local header", name);
            return std::nullopt;
        }
        const uint64_t start = local + 30 + U16(local + 26) + U16(local + 28);
        if (start + entry.compressedSize > mData.size()) {
            SPDLOG_WARN("Skipping file {}: truncated", name);
            return std::nullopt;
        }
        const uint8_t* src = mData.data() + start;
        std::vector<uint8_t> out(entry.size);

        if (entry.method == 0) {
            std::memcpy(out.data(), src, entry.size);
        } else if (entry.method == 8) {
            z_stream z = {};
            if (inflateInit2(&z, -MAX_WBITS) != Z_OK) {
                return std::nullopt;
            }
            z.next_in = const_cast<Bytef*>(src);
            z.avail_in = (uInt)entry.compressedSize;
            z.next_out = out.data();
            z.avail_out = (uInt)out.size();
            const int result = inflate(&z, Z_FINISH);
            inflateEnd(&z);
            if (result != Z_STREAM_END || z.total_out != entry.size) {
                SPDLOG_WARN("Skipping file {}: inflate failed", name);
                return std::nullopt;
            }
        } else {
            SPDLOG_WARN("Skipping file {}: unsupported compression {}", name, entry.method);
            return std::nullopt;
        }
        if (crc32(0, out.data(), (uInt)out.size()) != entry.crc) {
            SPDLOG_WARN("Skipping file {}: CRC mismatch", name);
            return std::nullopt;
        }
        return out;
    }

  private:
    struct Entry {
        std::string name;
        uint16_t method = 0;
        uint32_t crc = 0;
        uint64_t compressedSize = 0;
        uint64_t size = 0;
        uint64_t localHeaderOffset = 0;
    };

    ZipReader() = default;

    uint16_t U16(uint64_t at) const {
        return (uint16_t)(mData[at] | mData[at + 1] << 8);
    }
    uint32_t U32(uint64_t at) const {
        return (uint32_t)U16(at) | (uint32_t)U16(at + 2) << 16;
    }
    uint64_t U64(uint64_t at) const {
        return (uint64_t)U32(at) | (uint64_t)U32(at + 4) << 32;
    }

    bool ReadCentralDirectory() {
        if (mData.size() < 22) {
            return false;
        }
        int64_t eocd = -1;
        const int64_t lowest = std::max<int64_t>(0, (int64_t)mData.size() - 22 - 0xFFFF);
        for (int64_t at = (int64_t)mData.size() - 22; at >= lowest; at--) {
            if (U32(at) == 0x06054b50) {
                eocd = at;
                break;
            }
        }
        if (eocd < 0) {
            return false;
        }
        uint64_t count = U16(eocd + 10);
        uint64_t offset = U32(eocd + 16);
        if ((count == 0xFFFF || offset == 0xFFFFFFFF) && eocd >= 20 && U32(eocd - 20) == 0x07064b50) {
            const uint64_t zip64 = U64(eocd - 20 + 8);
            if (zip64 + 56 <= mData.size() && U32(zip64) == 0x06064b50) {
                count = U64(zip64 + 32);
                offset = U64(zip64 + 48);
            }
        }

        uint64_t at = offset;
        for (uint64_t i = 0; i < count; i++) {
            if (at + 46 > mData.size() || U32(at) != 0x02014b50) {
                return false;
            }
            Entry entry;
            entry.method = U16(at + 10);
            entry.crc = U32(at + 16);
            entry.compressedSize = U32(at + 20);
            entry.size = U32(at + 24);
            const uint16_t nameLength = U16(at + 28);
            const uint16_t extraLength = U16(at + 30);
            const uint16_t commentLength = U16(at + 32);
            entry.localHeaderOffset = U32(at + 42);
            if (at + 46 + nameLength + extraLength > mData.size()) {
                return false;
            }
            entry.name.assign(reinterpret_cast<const char*>(&mData[at + 46]), nameLength);

            uint64_t extra = at + 46 + nameLength;
            const uint64_t extraEnd = extra + extraLength;
            while (extra + 4 <= extraEnd) {
                const uint16_t id = U16(extra);
                const uint16_t length = U16(extra + 2);
                uint64_t field = extra + 4;
                if (id == 0x0001) {
                    if (entry.size == 0xFFFFFFFF && field + 8 <= extraEnd) {
                        entry.size = U64(field);
                        field += 8;
                    }
                    if (entry.compressedSize == 0xFFFFFFFF && field + 8 <= extraEnd) {
                        entry.compressedSize = U64(field);
                        field += 8;
                    }
                    if (entry.localHeaderOffset == 0xFFFFFFFF && field + 8 <= extraEnd) {
                        entry.localHeaderOffset = U64(field);
                    }
                }
                extra += 4 + length;
            }

            mIndex.emplace(entry.name, mEntries.size());
            mEntries.push_back(std::move(entry));
            at += 46 + nameLength + extraLength + commentLength;
        }
        return true;
    }

    std::vector<uint8_t> mData;
    std::vector<Entry> mEntries;
    std::unordered_map<std::string, size_t> mIndex;
};

class MpqReader : public ArchiveReader {
  public:
    explicit MpqReader(HANDLE handle) : mHandle(handle) {
    }
    ~MpqReader() override {
        SFileCloseArchive(mHandle);
    }

    std::vector<std::string> List() override {
        std::vector<std::string> names;
        SFILE_FIND_DATA data;
        HANDLE find = SFileFindFirstFile(mHandle, "*", &data, nullptr);
        if (find == nullptr) {
            return names;
        }
        do {
            const std::string name = data.cFileName;
            if (!IsReservedMpqName(name)) {
                names.push_back(name);
            }
        } while (SFileFindNextFile(find, &data));
        SFileFindClose(find);
        return names;
    }

    std::optional<std::vector<uint8_t>> Read(const std::string& name) override {
        std::lock_guard lock(mMutex);
        HANDLE file = nullptr;
        if (!SFileOpenFileEx(mHandle, name.c_str(), 0, &file)) {
            SPDLOG_WARN("Skipping file {}: cannot open", name);
            return std::nullopt;
        }
        const DWORD size = SFileGetFileSize(file, nullptr);
        std::vector<uint8_t> data(size);
        DWORD read = 0;
        const bool ok = size == 0 || SFileReadFile(file, data.data(), size, &read, nullptr);
        SFileCloseFile(file);
        if (!ok || read != size) {
            SPDLOG_WARN("Skipping file {}: read failed", name);
            return std::nullopt;
        }
        return data;
    }

  private:
    HANDLE mHandle;
    std::mutex mMutex;
};
} // namespace

std::unique_ptr<ArchiveReader> ArchiveReader::Open(const std::string& path) {
    if (!std::filesystem::exists(std::filesystem::path(std::u8string(path.begin(), path.end())))) {
        SPDLOG_ERROR("Archive not found: {}", path);
        return nullptr;
    }
    try {
        if (IsOtrPath(path)) {
            HANDLE handle = nullptr;
            if (!SFileOpenArchive(path.c_str(), 0, MPQ_OPEN_READ_ONLY, &handle)) {
                SPDLOG_ERROR("Failed to open OTR {} (error {})", path, GetLastError());
                return nullptr;
            }
            return std::make_unique<MpqReader>(handle);
        }
        return ZipReader::Open(path);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to open {}: {}", path, e.what());
        return nullptr;
    }
}

void ArchiveReader::ForEach(
    const std::function<void(const std::string& name, const std::vector<uint8_t>& data)>& onFile) {
    for (const auto& name : List()) {
        if (auto data = Read(name)) {
            onFile(name, *data);
        }
    }
}

ArchiveWriter::ArchiveWriter(const std::string& path) {
    std::error_code ec;
    std::filesystem::remove(std::filesystem::path(std::u8string(path.begin(), path.end())), ec);

    mZip = !IsOtrPath(path);
    std::unique_ptr<BinaryWrapper> wrapper;
    if (mZip) {
        wrapper = std::make_unique<ZWrapper>(path);
    } else {
        wrapper = std::make_unique<SWrapper>(path);
    }
    try {
        if (wrapper->CreateArchive() != 0) {
            SPDLOG_ERROR("Failed to create archive {}", path);
            return;
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to create archive {}: {}", path, e.what());
        return;
    }
    mWrapper = std::move(wrapper);
}

ArchiveWriter::~ArchiveWriter() {
    Close();
}

bool ArchiveWriter::Add(const std::string& path, std::vector<char> data) {
    std::lock_guard lock(mMutex);
    if (!mWrapper || mClosed) {
        return false;
    }
    try {
        return mWrapper->AddFile(path, std::move(data));
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to add {}: {}", path, e.what());
        return false;
    }
}

bool ArchiveWriter::Close() {
    std::lock_guard lock(mMutex);
    if (!mWrapper || mClosed) {
        return false;
    }
    mClosed = true;
    try {
        return mWrapper->Close() == 0;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to close archive: {}", e.what());
        return false;
    }
}
} // namespace Otr
