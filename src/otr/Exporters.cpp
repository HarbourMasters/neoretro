#include "Exporters.h"

#include <sstream>

namespace Otr {
ExportResult TextureBinaryExporter::Export(std::ostream& write, std::shared_ptr<IParsedData> raw,
                                           std::string& entryName, YAML::Node& node, std::string* replacement) {
    const auto& texture = std::static_pointer_cast<TextureResourceData>(raw)->mTexture;
    auto writer = LUS::BinaryWriter();

    WriteHeader(writer, Torch::ResourceType::Texture, (int32_t)texture.version);
    writer.Write((uint32_t)texture.type);
    writer.Write((uint32_t)texture.width);
    writer.Write((uint32_t)texture.height);
    if (texture.version == Version::Roy) {
        writer.Write((uint32_t)texture.flags);
        writer.Write(texture.hByteScale);
        writer.Write(texture.vPixelScale);
    }
    writer.Write((uint32_t)texture.data.size());
    writer.Write((char*)texture.data.data(), texture.data.size());
    writer.Finish(write);
    return std::nullopt;
}

ExportResult SequenceBinaryExporter::Export(std::ostream& write, std::shared_ptr<IParsedData> raw,
                                            std::string& entryName, YAML::Node& node, std::string* replacement) {
    const auto& sequence = std::static_pointer_cast<SequenceResourceData>(raw)->mSequence;
    auto writer = LUS::BinaryWriter();

    WriteHeader(writer, Torch::ResourceType::OoTAudioSequence, (int32_t)Version::Rachael);
    writer.Write((uint32_t)sequence.rawBinary.size());
    writer.Write((char*)sequence.rawBinary.data(), sequence.rawBinary.size());
    writer.Write(sequence.sequenceNum);
    writer.Write(sequence.medium);
    writer.Write(sequence.cachePolicy);
    writer.Write((uint32_t)sequence.fontIndices.size());
    for (const uint8_t font : sequence.fontIndices) {
        writer.Write(font);
    }
    writer.Finish(write);
    return std::nullopt;
}

std::vector<char> ExportResource(BaseExporter& exporter, std::shared_ptr<IParsedData> data, std::string entryName) {
    std::stringstream stream;
    YAML::Node node;
    std::string replacement = entryName;
    exporter.Export(stream, std::move(data), entryName, node, &replacement);
    const std::string bytes = stream.str();
    return std::vector<char>(bytes.begin(), bytes.end());
}

std::vector<char> ExportTexture(const Texture& texture, const std::string& entryName) {
    TextureBinaryExporter exporter;
    return ExportResource(exporter, std::make_shared<TextureResourceData>(texture), entryName);
}

std::vector<char> ExportSequence(const Sequence& sequence) {
    SequenceBinaryExporter exporter;
    return ExportResource(exporter, std::make_shared<SequenceResourceData>(sequence), sequence.path);
}
} // namespace Otr
