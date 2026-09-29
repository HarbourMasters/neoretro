#include "DebugConvertTexturesScreen.h"

#include "app/AppContext.h"
#include "app/Router.h"
#include "otr/Resource.h"
#include "ui/Toasts.h"
#include "ui/clay/ClayUi.h"
#include "ui/components/CustomScaffold.h"
#include "ui/components/Widgets.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"
#include "utils/FileDialog.h"
#include "utils/Paths.h"

namespace {
constexpr Clay_Sizing kButtonSizing = { CLAY_SIZING_FIT(64, 250), CLAY_SIZING_FIXED(36) };

std::string FileLabel(const std::optional<std::string>& file) {
    return file ? Paths::Basename(*file) : "Select Texture File";
}
} // namespace

DebugConvertTexturesScreen::DebugConvertTexturesScreen() {
    for (int i = 1; i < Otr::kTextureTypeCount; i++) {
        mTypeNames.emplace_back(Otr::TextureTypeName((Otr::TextureType)i));
    }
}

void DebugConvertTexturesScreen::UpdatePreview(AppContext& ctx) {
    if (!mTexture) {
        return;
    }
    const auto image = Otr::TextureCodec::ToImage(*mTexture, mTlut ? &*mTlut : nullptr);
    if (!image) {
        ctx.toasts.Error("Could not decode a " + std::string(Otr::TextureTypeName(mTexture->type)) + " texture");
        return;
    }
    mPreview = *image;
    mPreviewPng = ImageIO::EncodePng(mPreview);
    mPreviewTexture.Upload(mPreview.rgba.data(), mPreview.width, mPreview.height);
}

void DebugConvertTexturesScreen::ConvertTexture(AppContext& ctx) {
    if (!mTextureFile) {
        return;
    }
    const auto bytes = Paths::ReadFile(Paths::FromUtf8(*mTextureFile));
    const auto image = bytes ? ImageIO::Decode(*bytes) : std::nullopt;
    if (!image) {
        ctx.toasts.Error("Could not decode " + Paths::Basename(*mTextureFile));
        return;
    }
    Otr::Texture texture;
    texture.type = mSelectedType;
    if (!Otr::TextureCodec::FromImage(texture, *image)) {
        ctx.toasts.Error("Cannot convert to " + std::string(Otr::TextureTypeName(mSelectedType)));
        return;
    }
    mTexture = std::move(texture);
    mTlut.reset();
    mIsPalette = Otr::IsPalette(mSelectedType);
    UpdatePreview(ctx);
}

void DebugConvertTexturesScreen::LoadTlut(AppContext& ctx) {
    const auto file = FileDialog::PickFile(FileDialog::kImageFilters);
    if (!file) {
        return;
    }
    const auto bytes = Paths::ReadFile(Paths::FromUtf8(*file));
    const auto image = bytes ? ImageIO::Decode(*bytes) : std::nullopt;
    if (!image) {
        ctx.toasts.Error("Could not decode " + Paths::Basename(*file));
        return;
    }
    Otr::Texture tlut;
    tlut.type = Otr::TextureType::RGBA32bpp;
    Otr::TextureCodec::FromImage(tlut, *image);
    mTlut = std::move(tlut);
    UpdatePreview(ctx);
}

void DebugConvertTexturesScreen::SaveAs(AppContext& ctx) {
    const auto out = FileDialog::SaveFile("converted.png", FileDialog::kPngFilters);
    if (out && !Paths::WriteFile(Paths::FromUtf8(*out), mPreviewPng)) {
        ctx.toasts.Error("Could not write " + *out);
    }
}

void DebugConvertTexturesScreen::LoadTexture(AppContext& ctx) {
    if (!mTextureFile) {
        return;
    }
    const auto bytes = Paths::ReadFile(Paths::FromUtf8(*mTextureFile));
    const auto header = bytes ? Otr::ReadHeader(*bytes) : std::nullopt;
    if (!header) {
        ctx.toasts.Error("Not a resource file");
        return;
    }
    mTlut.reset();
    mIsPalette = false;
    if (header->type == (uint32_t)Otr::ResourceType::Texture) {
        mTexture = Otr::Texture::Read(*bytes);
        if (!mTexture) {
            ctx.toasts.Error("Malformed texture");
            return;
        }
        UpdatePreview(ctx);
        return;
    }
    const auto background = Otr::Background::Read(*bytes);
    const auto image = background ? ImageIO::DecodeJpeg(background->data) : std::nullopt;
    if (!image) {
        ctx.toasts.Error("Not a texture or background resource");
        return;
    }
    mTexture = Otr::Texture{};
    mPreview = *image;
    mPreviewPng = ImageIO::EncodePng(mPreview);
    mPreviewTexture.Upload(mPreview.rgba.data(), mPreview.width, mPreview.height);
}

void DebugConvertTexturesScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    ScaffoldOptions options;
    options.title = "Debug Convert Textures";
    options.subtitle = "Convert and preview textures";
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };

    CustomScaffold(ui, options, [&] {
        CLAY({ .id = CLAY_ID("Convert_Body"), .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } } }) {
            CLAY({ .id = CLAY_ID("Convert_Controls"),
                   .layout = {
                       .padding = CLAY_PADDING_ALL(20), .childGap = 20, .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
                Widgets::Text(ui, "PNG to N64 Viewer", TextSize::BodyMedium, RetroColors::White, FONT_BOLD);
                Widgets::Dropdown(ui, "Convert_Type", &mTypeNames, (int)mSelectedType - 1, 220,
                                  [this](int index) { mSelectedType = (Otr::TextureType)(index + 1); });
                if (Widgets::Button(ui, "Convert_SelectPng", FileLabel(mTextureFile), kButtonSizing)) {
                    if (auto file = FileDialog::PickFile(FileDialog::kImageFilters)) {
                        mTextureFile = file;
                    }
                }
                if (Widgets::Button(ui, "Convert_Convert", "Convert Texture", kButtonSizing)) {
                    ConvertTexture(ctx);
                }
                if (mTexture && mIsPalette && Widgets::Button(ui, "Convert_LoadTlut", "Load TLUT", kButtonSizing)) {
                    LoadTlut(ctx);
                }
                if (mTexture && mTlut && Widgets::Button(ui, "Convert_Save", "Save As", kButtonSizing)) {
                    SaveAs(ctx);
                }
                Widgets::Text(ui, "OTR N64 Viewer", TextSize::BodyMedium, RetroColors::White, FONT_BOLD);
                if (Widgets::Button(ui, "Convert_SelectResource", FileLabel(mTextureFile), kButtonSizing)) {
                    if (auto file = FileDialog::PickFile()) {
                        mTextureFile = file;
                    }
                }
                if (Widgets::Button(ui, "Convert_Load", "Load Texture", kButtonSizing)) {
                    LoadTexture(ctx);
                }
            }
            CLAY({ .id = CLAY_ID("Convert_PreviewPad"),
                   .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) }, .padding = { 0, 20, 0, 0 } },
                   .cornerRadius = CLAY_CORNER_RADIUS(10) }) {
                Widgets::ImagePreview(ui, "Convert_Preview", mTexture ? &mPreviewTexture : nullptr);
            }
        }
    });
}
