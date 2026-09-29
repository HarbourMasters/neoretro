#pragma once

#include "Texture.h"

#include <factories/BaseFactory.h>
#include <memory>
#include <vector>

namespace Otr {
class TextureResourceData : public IParsedData {
  public:
    explicit TextureResourceData(Texture texture) : mTexture(std::move(texture)) {
    }
    Texture mTexture;
};

class SequenceResourceData : public IParsedData {
  public:
    explicit SequenceResourceData(Sequence sequence) : mSequence(std::move(sequence)) {
    }
    Sequence mSequence;
};

class TextureBinaryExporter : public BaseExporter {
  public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName,
                        YAML::Node& node, std::string* replacement) override;
};

class SequenceBinaryExporter : public BaseExporter {
  public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName,
                        YAML::Node& node, std::string* replacement) override;
};

std::vector<char> ExportResource(BaseExporter& exporter, std::shared_ptr<IParsedData> data, std::string entryName);

std::vector<char> ExportTexture(const Texture& texture, const std::string& entryName);
std::vector<char> ExportSequence(const Sequence& sequence);
} // namespace Otr
