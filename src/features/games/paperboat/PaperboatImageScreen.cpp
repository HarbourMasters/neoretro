#include "PaperboatImageScreen.h"

#include "l10n/Strings.h"
#include "otr/Exporters.h"
#include "services/StageManager.h"
#include "utils/Paths.h"

#include <spdlog/fmt/fmt.h>

namespace {
class PaperboatImages : public ImageEditorGame {
  public:
    explicit PaperboatImages(Pm64::ImageKind kind) : mKind(kind) {
    }

    Apply LoadArchive(Otr::ArchiveReader& archive) override {
        auto assets = std::make_shared<std::vector<Pm64::ImageAsset>>(Pm64::LoadImages(archive, mKind));
        auto entries = std::make_shared<std::vector<ImageEntry>>();
        std::vector<Pm64::ImageAsset> decodable;
        for (auto& asset : *assets) {
            auto image = Pm64::DecodeImage(asset);
            if (!image) {
                continue;
            }
            ImageEntry entry;
            entry.path = asset.path;
            entry.group = asset.group;
            entry.name = asset.name;
            entry.info = Pm64::DescribeImage(asset);
            entry.original = ImageOps::Convert(*image, 4);
            entries->push_back(std::move(entry));
            decodable.push_back(std::move(asset));
        }
        *assets = std::move(decodable);
        return [this, assets, entries](ImageEntryList& list) {
            mAssets = std::move(*assets);
            list.entries = std::move(*entries);
        };
    }

    std::optional<std::string> Import(ImageEntry& entry, int index, const Image& image) override {
        const auto& asset = mAssets[index];
        if (image.width != asset.texture.width || image.height != asset.texture.height) {
            return fmt::format(fmt::runtime(Tr("imageEditorScreen_wrongSize")), asset.texture.width,
                               asset.texture.height, image.width, image.height);
        }
        std::string error;
        auto encoded = Pm64::EncodeImage(asset, image, &error);
        if (!encoded) {
            return error;
        }
        entry.edited = std::move(encoded->preview);
        entry.note.clear();
        if (encoded->reducedColors) {
            entry.note = fmt::format(fmt::runtime(Tr("imageEditorScreen_reducedColors")),
                                     asset.texture.type == Otr::TextureType::Palette4bpp ? 16 : 256);
        } else if (encoded->mappedToPalette) {
            entry.note = Tr("imageEditorScreen_mappedToPalette");
        }
        return std::nullopt;
    }

    void Stage(StageManager& stage, const ImageEntryList& list, const std::vector<int>& modified) override {
        for (const int i : modified) {
            const auto encoded = Pm64::EncodeImage(mAssets[i], *list.entries[i].edited);
            if (!encoded) {
                continue;
            }
            for (const auto& [path, texture] : encoded->textures) {
                const std::string name = Paths::Basename(path);
                stage.AddResourceData(Paths::Dirname(path), name, Otr::ExportTexture(texture, name));
            }
        }
    }

  private:
    Pm64::ImageKind mKind;
    std::vector<Pm64::ImageAsset> mAssets;
};

ImageEditorScreenOptions OptionsFor(Pm64::ImageKind kind) {
    switch (kind) {
        case Pm64::ImageKind::Sprites:
            return { "PaperboatSprites", "paperboatImageScreen_sprites", "paperboatImageScreen_spritesSubtitle",
                     "paperboatTextScreen_archivePath", "paperboatImageScreen_noImages" };
        case Pm64::ImageKind::MapTextures:
            return { "PaperboatMapTextures", "paperboatImageScreen_mapTextures",
                     "paperboatImageScreen_mapTexturesSubtitle", "paperboatTextScreen_archivePath",
                     "paperboatImageScreen_noImages" };
        case Pm64::ImageKind::Pictures:
        default:
            return { "PaperboatPictures", "paperboatImageScreen_pictures", "paperboatImageScreen_picturesSubtitle",
                     "paperboatTextScreen_archivePath", "paperboatImageScreen_noImages" };
    }
}
} // namespace

PaperboatImageScreen::PaperboatImageScreen(Pm64::ImageKind kind)
    : ImageEditorScreen(OptionsFor(kind), std::make_shared<PaperboatImages>(kind)) {
}
