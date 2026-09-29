#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

class BinaryWrapper;

namespace Otr {
class ArchiveReader {
  public:
    virtual ~ArchiveReader() = default;

    static std::unique_ptr<ArchiveReader> Open(const std::string& path);

    virtual std::vector<std::string> List() = 0;
    virtual std::optional<std::vector<uint8_t>> Read(const std::string& name) = 0;

    void ForEach(const std::function<void(const std::string& name, const std::vector<uint8_t>& data)>& onFile);
};

class ArchiveWriter {
  public:
    explicit ArchiveWriter(const std::string& path);
    ~ArchiveWriter();

    bool IsOpen() const {
        return mWrapper != nullptr;
    }
    bool IsZip() const {
        return mZip;
    }
    bool Add(const std::string& path, std::vector<char> data);
    bool Close();

  private:
    std::unique_ptr<BinaryWrapper> mWrapper;
    std::mutex mMutex;
    bool mZip = false;
    bool mClosed = false;
};
} // namespace Otr
