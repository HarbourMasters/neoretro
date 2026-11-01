#pragma once

#include "app/Screen.h"
#include "ui/components/ImageEntryEditor.h"

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Otr {
class ArchiveReader;
}

class StageManager;

class ImageEditorGame {
  public:
    using Apply = std::function<void(ImageEntryList& list)>;

    virtual ~ImageEditorGame() = default;

    virtual Apply LoadArchive(Otr::ArchiveReader& archive) = 0;
    virtual std::optional<std::string> Import(ImageEntry& entry, int index, const Image& image) = 0;
    virtual void Stage(StageManager& stage, const ImageEntryList& list, const std::vector<int>& modified) = 0;
};

struct ImageEditorScreenOptions {
    std::string id;
    const char* titleKey;
    const char* subtitleKey;
    const char* archivePathKey;
    const char* noImagesKey;
};

class ImageEditorScreen : public Screen {
  public:
    ImageEditorScreen(ImageEditorScreenOptions options, std::shared_ptr<ImageEditorGame> game);

    void Layout(AppContext& ctx) override;

  private:
    struct State {
        std::shared_ptr<ImageEditorGame> game;
        ImageEntryList list;
        ImageEntryEditorState editor;
        std::optional<std::string> archivePath;
        bool isProcessing = false;
        bool loaded = false;
    };

    void OnSelectArchive();
    void OnExportAll(AppContext& ctx);
    void OnImportFolder(AppContext& ctx);
    void ExportOne(AppContext& ctx, int index);
    void ImportOne(AppContext& ctx, int index);
    void StageChanges(AppContext& ctx);

    ImageEditorScreenOptions mOptions;
    std::shared_ptr<State> mState = std::make_shared<State>();
};
