#include "PaperboatTextScreen.h"

#include "games/pm64/Pm64Font.h"
#include "games/pm64/Pm64Text.h"
#include "l10n/Strings.h"
#include "services/StageManager.h"
#include "ui/PreviewTexture.h"
#include "utils/Paths.h"

#include <algorithm>
#include <imgui.h>

namespace {
class PaperboatText : public TextEditorGame {
  public:
    Apply LoadArchive(Otr::ArchiveReader& archive) override {
        auto messages = std::make_shared<Pm64::Messages>(Pm64::LoadMessages(archive));
        auto font = std::make_shared<Pm64::Font>(Pm64::LoadFont(archive));
        return [this, messages, font](TextEntryList& list) {
            mFont = std::move(*font);
            mPreviewText.reset();
            mCharsets.clear();
            list.entries.reserve(messages->size());
            for (const auto& [path, bytes] : *messages) {
                const auto charset = Pm64::CharsetFor(path);
                TextEntry entry;
                entry.path = path;
                entry.name = Paths::Basename(path);
                entry.original = Pm64::DecodeText(bytes, charset);
                entry.text = entry.original;
                list.entries.push_back(std::move(entry));
                mCharsets.push_back(charset);
            }
        };
    }

    Apply LoadMod(Otr::ArchiveReader& archive) override {
        auto messages = std::make_shared<Pm64::Messages>(Pm64::LoadMessages(archive));
        return [this, messages](TextEntryList& list) {
            for (int i = 0; i < (int)list.entries.size(); i++) {
                auto& entry = list.entries[i];
                if (const auto it = messages->find(entry.path); it != messages->end()) {
                    entry.text = Pm64::DecodeText(it->second, mCharsets[i]);
                }
            }
        };
    }

    void Validate(TextEntry& entry, int index) override {
        entry.error.clear();
        Pm64::EncodeText(entry.text, mCharsets[index], &entry.error);
    }

    void DrawFields(TextEntry& entry, int index) override {
        if (mCharsets[index] == Pm64::Charset::Credits) {
            ImGui::TextDisabled("%s", Tr("paperboatTextScreen_creditsCharset"));
        } else if (!mFont.Empty()) {
            DrawFontPreview(entry, index);
        }
    }

    std::string Preview(const std::string& text) const override {
        std::string line;
        size_t at = 0;
        while (at < text.size() && line.size() < 48) {
            if (text.compare(at, 2, "\\[") == 0) {
                line += '[';
                at += 2;
            } else if (text[at] == '[') {
                const auto close = text.find(']', at);
                at = close == std::string::npos ? text.size() : close + 1;
            } else if (text[at] == '\n') {
                if (!line.empty() && line.back() != ' ') {
                    line += ' ';
                }
                at++;
            } else {
                do {
                    line += text[at++];
                } while (at < text.size() && ((uint8_t)text[at] & 0xC0) == 0x80);
            }
        }
        return at < text.size() ? line + "..." : line;
    }

    void Stage(StageManager& stage, const TextEntryList& list, const std::vector<int>& modified) override {
        for (const int i : modified) {
            const auto& entry = list.entries[i];
            const auto bytes = *Pm64::EncodeText(entry.text, mCharsets[i]);
            stage.AddResourceData(Paths::Dirname(entry.path), Paths::Basename(entry.path),
                                  Pm64::ExportMessage(bytes, entry.path));
        }
    }

  private:
    void DrawFontPreview(const TextEntry& entry, int index) {
        if (mPreviewText != entry.text) {
            mPreviewText = entry.text;
            const auto bytes = Pm64::EncodeText(entry.text, mCharsets[index]);
            if (bytes) {
                const Image image = Pm64::RenderText(mFont, *bytes);
                mPreview.Upload(image.rgba.data(), image.width, image.height);
            }
        }
        if (!mPreview.Valid()) {
            return;
        }
        constexpr float kScale = 2.0f;
        constexpr float kPadding = 8.0f;
        const float maxHeight = std::max(ImGui::GetContentRegionAvail().y * 0.3f, 64.0f);
        const ImVec2 size(mPreview.Width() * kScale, mPreview.Height() * kScale);
        ImGui::TextDisabled("%s", Tr("paperboatTextScreen_fontPreview"));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(244, 236, 214, 255));
        ImGui::BeginChild("##PaperboatFontPreview", ImVec2(0, std::min(size.y + kPadding * 2, maxHeight)),
                          ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::SetCursorPos(ImVec2(kPadding, kPadding));
        ImGui::Image(ImTextureRef((ImTextureID)mPreview.Id()), size);
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    std::vector<Pm64::Charset> mCharsets;
    Pm64::Font mFont;
    PreviewTexture mPreview;
    std::optional<std::string> mPreviewText;
};
} // namespace

PaperboatTextScreen::PaperboatTextScreen()
    : TextEditorScreen({ .id = "PaperboatText",
                         .titleKey = "paperboatTextScreen_strings",
                         .subtitleKey = "paperboatTextScreen_stringsSubtitle",
                         .archivePathKey = "paperboatTextScreen_archivePath",
                         .noTextKey = "paperboatTextScreen_noText",
                         .hintKey = "paperboatTextScreen_hint" },
                       std::make_shared<PaperboatText>()) {
}
