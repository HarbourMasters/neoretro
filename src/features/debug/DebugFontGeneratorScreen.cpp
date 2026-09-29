#include "DebugFontGeneratorScreen.h"

#include "app/AppContext.h"
#include "app/Router.h"
#include "l10n/Strings.h"
#include "services/StageManager.h"
#include "services/TextureReplacement.h"
#include "ui/Toasts.h"
#include "ui/clay/ClayUi.h"
#include "ui/components/CustomScaffold.h"
#include "ui/components/Widgets.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"
#include "utils/FileDialog.h"
#include "utils/Paths.h"

#include <cmath>
#include <imgui.h>
#include <spdlog/fmt/fmt.h>

namespace {
std::string DartNumber(double value, bool isDouble) {
    if (!isDouble) {
        return std::to_string((long long)value);
    }
    std::string text = fmt::format("{}", value);
    if (text.find_first_of(".en") == std::string::npos) {
        text += ".0";
    }
    return text;
}

double TextNumpad(ClayUi& ui, std::string_view id, std::string_view title, double input, double step, bool isDouble) {
    double value = input;
    const std::string prefix(id);
    CLAY({ .id = ClayUi::Id(prefix), .layout = { .childGap = 10, .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
        Widgets::Text(ui, title, 11, RetroColors::White, FONT_BOLD);
        CLAY({ .id = ClayUi::Id(prefix + "_Row"),
               .layout = { .childGap = 10, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
            if (Widgets::Button(ui, prefix + "_Minus", "-", { CLAY_SIZING_FIT(64), CLAY_SIZING_FIXED(36) })) {
                value = input - step;
            }
            Widgets::Text(ui, DartNumber(input, isDouble), TextSize::BodyMedium, RetroColors::White, FONT_BOLD);
            if (Widgets::Button(ui, prefix + "_Plus", "+", { CLAY_SIZING_FIT(64), CLAY_SIZING_FIXED(36) })) {
                value = input + step;
            }
        }
    }
    return value;
}
} // namespace

void DebugFontGeneratorScreen::RefreshPreview() {
    if (!mGenerator.IsLoaded()) {
        return;
    }
    const auto key = std::make_tuple(mSettings.offset, mSettings.scale, mSettings.glyphWidth, mSettings.glyphHeight,
                                     mSettings.perRow);
    if (key == mPreviewKey && mPreview.Valid()) {
        return;
    }
    mPreviewKey = key;
    const int width = std::max(1, mSettings.perRow * mSettings.glyphWidth);
    const int height = std::max(1, mSettings.rows * mSettings.glyphHeight);
    const Image preview = mGenerator.Render(mSettings, width, height);
    mPreview.Upload(preview.rgba.data(), preview.width, preview.height);
}

void DebugFontGeneratorScreen::WriteFontTable(AppContext& ctx) {
    const Image table = mGenerator.RenderTable(mSettings);
    const auto file = std::filesystem::temp_directory_path() / "sGfxPrintFontData.png";
    if (!Paths::WriteFile(file, ImageIO::EncodePng(table))) {
        ctx.toasts.Error("Could not write " + Paths::ToUtf8(file));
        return;
    }
    ctx.stage.AddFile(file, TextureReplacement::kFontTextureName);
    ctx.router.PopUntil("/create_selection");
}

void DebugFontGeneratorScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    RefreshPreview();

    ScaffoldOptions options;
    options.title = Tr("sohCreateDebugFontScreen_title");
    options.subtitle = Tr("sohCreateDebugFontScreen_subtitle");
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };

    CustomScaffold(ui, options, [&] {
        CLAY({ .id = CLAY_ID("Font_Body"), .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } } }) {
            CLAY({ .id = CLAY_ID("Font_Controls"),
                   .layout = {
                       .padding = CLAY_PADDING_ALL(20), .childGap = 10, .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
                mSettings.offset = (float)TextNumpad(ui, "Font_Offset", "Text Offset:", mSettings.offset, 0.5, true);
                mSettings.scale = (float)TextNumpad(ui, "Font_Scale", "Text Scale:", mSettings.scale, 0.1, true);
                mSettings.glyphWidth =
                    (int)TextNumpad(ui, "Font_GlyphWidth", "Glyph Width:", mSettings.glyphWidth, 1, false);
                mSettings.glyphHeight =
                    (int)TextNumpad(ui, "Font_GlyphHeight", "Glyph Height:", mSettings.glyphHeight, 1, false);
                mSettings.perRow = (int)TextNumpad(ui, "Font_Rows", "Rows:", mSettings.perRow, 1, false);
                if (Widgets::Button(ui, "Font_Load", "Load Font", { CLAY_SIZING_FIT(64), CLAY_SIZING_FIXED(36) })) {
                    if (const auto file = FileDialog::PickFile(FileDialog::kFontFilters)) {
                        if (mGenerator.Load(Paths::FromUtf8(*file))) {
                            mPreview.Reset();
                        } else {
                            ctx.toasts.Error("Could not load " + Paths::Basename(*file));
                        }
                    }
                }
                if (mGenerator.IsLoaded() && Widgets::Button(ui, "Font_Convert", "Convert Texture",
                                                             { CLAY_SIZING_FIT(64), CLAY_SIZING_FIXED(36) })) {
                    WriteFontTable(ctx);
                }
            }
            CLAY({ .id = CLAY_ID("Font_Preview"),
                   .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) }, .padding = { 0, 20, 20, 0 } },
                   .clip = { .horizontal = true, .vertical = true } }) {
                if (mGenerator.IsLoaded() && mPreview.Valid()) {
                    const PreviewTexture* preview = &mPreview;
                    CLAY({ .id = CLAY_ID("Font_PreviewImage"),
                           .layout = { .sizing = { CLAY_SIZING_FIXED((float)preview->Width()),
                                                   CLAY_SIZING_FIXED((float)preview->Height()) } },
                           .image = { .imageData = (void*)(uintptr_t)preview->Id() } }) {
                    }
                }
            }
        }
    });
}
