#include "OtrGenerator.h"

#include "otr/Archive.h"
#include "otr/Exporters.h"
#include "services/TextureBuilder.h"
#include "utils/Paths.h"
#include "utils/ThreadPool.h"

#include <algorithm>
#include <map>
#include <spdlog/spdlog.h>

namespace OtrGenerator {
namespace {
std::string EntryFilePath(const std::string& key, const std::string& name) {
    return key == "." ? name : key + "/" + name;
}

void AddTextures(Otr::ArchiveWriter& archive, const std::string& key, std::vector<TexturePair> pairs, bool prependAlt,
                 std::atomic<int>& filesProcessed, const ErrorFn& onError) {
    std::vector<TextureBuilder::TextureGroup> work;
    std::map<std::string, TextureBuilder::TextureGroup> bands;
    for (auto& pair : pairs) {
        if (!pair.second.stripIndex) {
            work.push_back({ pair });
        } else {
            bands[pair.second.targetName.value_or(Paths::ToUtf8(pair.first))].push_back(pair);
        }
    }
    for (auto& [name, group] : bands) {
        work.push_back(std::move(group));
    }

    std::map<std::string, size_t> targets;
    for (size_t i = 0; i < work.size(); i++) {
        targets[work[i].front().second.targetName.value_or("")] = i;
    }
    std::map<std::string, std::optional<std::pair<int, int>>> baseSizes;
    for (auto& group : work) {
        const std::string target = group.front().second.targetName.value_or("");
        const auto at = target.find('@');
        if (at == std::string::npos) {
            continue;
        }
        const auto base = targets.find(target.substr(0, at));
        if (base == targets.end()) {
            continue;
        }
        const auto& baseGroup = work[base->second];
        const std::string baseName = baseGroup.front().second.targetName.value_or("");
        if (!baseSizes.count(baseName)) {
            baseSizes[baseName] = TextureBuilder::PackedSize(baseGroup);
        }
        const auto& size = baseSizes[baseName];
        for (auto& pair : group) {
            pair.second.fitWidth = size ? std::optional<int>(size->first) : std::nullopt;
            pair.second.fitHeight = size ? std::optional<int>(size->second) : std::nullopt;
        }
    }

    ThreadPool::Instance().ParallelFor(work.size(), [&](size_t index) {
        auto texture = TextureBuilder::Process(key, work[index], prependAlt);
        if (!texture.data) {
            onError("Failed to process texture " + texture.archivePath);
        } else {
            archive.Add(texture.archivePath, std::move(*texture.data));
        }
        filesProcessed++;
    });
    if (pairs.size() > work.size()) {
        filesProcessed += (int)(pairs.size() - work.size());
    }
}
} // namespace

bool Generate(const std::map<std::string, StageEntry>& entries, const std::string& outputFile, bool prependAlt,
              std::atomic<int>& filesProcessed, const ErrorFn& onError) {
    Otr::ArchiveWriter archive(outputFile);
    if (!archive.IsOpen()) {
        onError("Failed to create " + outputFile);
        return false;
    }

    for (const auto& [key, entry] : entries) {
        if (const auto* custom = std::get_if<CustomStageEntry>(&entry)) {
            for (const auto& file : custom->files) {
                const auto data = Paths::ReadFile(file);
                if (!data) {
                    onError("Failed to read " + Paths::ToUtf8(file));
                } else {
                    archive.Add(EntryFilePath(key, Paths::Basename(Paths::ToUtf8(file))),
                                std::vector<char>(data->begin(), data->end()));
                }
                filesProcessed++;
            }
        } else if (const auto* sequences = std::get_if<CustomSequencesEntry>(&entry)) {
            for (const auto& [seqFile, metaFile] : sequences->pairs) {
                const auto seq = Paths::ReadFile(seqFile);
                const auto meta = Paths::ReadText(metaFile);
                const auto sequence = seq && meta ? Otr::Sequence::FromSeqFile(*seq, *meta) : std::nullopt;
                if (!sequence) {
                    onError("Failed to process sequence " + Paths::ToUtf8(seqFile));
                } else {
                    archive.Add(EntryFilePath(key, sequence->path), Otr::ExportSequence(*sequence));
                }
                filesProcessed++;
            }
        } else if (const auto* textures = std::get_if<CustomTexturesEntry>(&entry)) {
            AddTextures(archive, key, textures->pairs, prependAlt, filesProcessed, onError);
        } else if (const auto* resources = std::get_if<ResourceDataEntry>(&entry)) {
            for (const auto& [name, data] : resources->resources) {
                archive.Add(EntryFilePath(key, name), data);
                filesProcessed++;
            }
        }
    }

    const bool ok = archive.Close();
    if (!ok) {
        onError("Failed to write " + outputFile);
    }
    SPDLOG_INFO("Generated {} ({} files)", outputFile, filesProcessed.load());
    return ok;
}
} // namespace OtrGenerator
