#include "GhostshipTextScreen.h"

#include "games/sm64/Sm64Text.h"
#include "l10n/Strings.h"
#include "services/StageManager.h"
#include "utils/Paths.h"

#include <algorithm>
#include <imgui.h>

namespace {
const char* TitleKey(GhostshipTextKind kind) {
    switch (kind) {
        case GhostshipTextKind::Dialogs:
            return "ghostshipTextScreen_dialogs";
        case GhostshipTextKind::Texts:
            return "ghostshipTextScreen_texts";
        default:
            return "ghostshipTextScreen_strings";
    }
}

const char* SubtitleKey(GhostshipTextKind kind) {
    switch (kind) {
        case GhostshipTextKind::Dialogs:
            return "ghostshipTextScreen_dialogsSubtitle";
        case GhostshipTextKind::Texts:
            return "ghostshipTextScreen_textsSubtitle";
        default:
            return "ghostshipTextScreen_stringsSubtitle";
    }
}

void InputClamped(const char* label, int16_t* value, int min, int max) {
    int current = *value;
    ImGui::SetNextItemWidth(110);
    if (ImGui::InputInt(label, &current)) {
        *value = (int16_t)std::clamp(current, min, max);
    }
}

bool SameFields(const Sm64::Dialog& a, const Sm64::Dialog& b) {
    return a.linesPerBox == b.linesPerBox && a.leftOffset == b.leftOffset && a.width == b.width;
}

class GhostshipText : public TextEditorGame {
  public:
    explicit GhostshipText(GhostshipTextKind kind) : mKind(kind) {
    }

    Apply LoadArchive(Otr::ArchiveReader& archive) override {
        auto assets = std::make_shared<Sm64::TextAssets>(Sm64::LoadTextAssets(archive));
        return [this, assets](TextEntryList& list) {
            mStrings = assets->strings;
            mDialogs.clear();
            const auto add = [&](const std::string& path, std::string name, const std::vector<uint8_t>& bytes) {
                TextEntry entry;
                entry.path = path;
                entry.name = std::move(name);
                entry.original = Sm64::DecodeText(bytes);
                entry.text = entry.original;
                list.entries.push_back(std::move(entry));
            };
            if (mKind == GhostshipTextKind::Dialogs) {
                for (const auto& [path, dialog] : assets->dialogs) {
                    add(path, Paths::Basename(path), dialog.text);
                    mDialogs.push_back(dialog);
                }
                mOriginalDialogs = mDialogs;
            } else if (mKind == GhostshipTextKind::Texts) {
                for (const auto& [path, text] : assets->texts) {
                    add(path, Paths::Basename(Paths::Dirname(path)) + "/" + Paths::Basename(path), text);
                }
            } else if (assets->strings) {
                for (const auto& [key, value] : *assets->strings) {
                    add(key, key, value);
                }
            }
        };
    }

    Apply LoadMod(Otr::ArchiveReader& archive) override {
        auto assets = std::make_shared<Sm64::TextAssets>(Sm64::LoadTextAssets(archive));
        return [this, assets](TextEntryList& list) {
            for (int i = 0; i < (int)list.entries.size(); i++) {
                auto& entry = list.entries[i];
                if (mKind == GhostshipTextKind::Dialogs) {
                    if (const auto it = assets->dialogs.find(entry.path); it != assets->dialogs.end()) {
                        mDialogs[i] = it->second;
                        entry.fieldsModified = !SameFields(mDialogs[i], mOriginalDialogs[i]);
                        entry.text = Sm64::DecodeText(it->second.text);
                    }
                } else if (mKind == GhostshipTextKind::Texts) {
                    if (const auto it = assets->texts.find(entry.path); it != assets->texts.end()) {
                        entry.text = Sm64::DecodeText(it->second);
                    }
                } else if (assets->strings) {
                    if (const auto it = assets->strings->find(entry.path); it != assets->strings->end()) {
                        entry.text = Sm64::DecodeText(it->second);
                    }
                }
            }
        };
    }

    void Validate(TextEntry& entry, int index) override {
        entry.error.clear();
        Sm64::EncodeText(entry.text, &entry.error);
    }

    void Revert(TextEntry& entry, int index) override {
        if (mKind == GhostshipTextKind::Dialogs) {
            mDialogs[index] = mOriginalDialogs[index];
        }
    }

    void DrawFields(TextEntry& entry, int index) override {
        if (mKind != GhostshipTextKind::Dialogs) {
            return;
        }
        auto& dialog = mDialogs[index];
        int lines = dialog.linesPerBox;
        ImGui::SetNextItemWidth(110);
        if (ImGui::InputInt(Tr("ghostshipTextScreen_linesPerBox"), &lines)) {
            dialog.linesPerBox = (int8_t)std::clamp(lines, 1, 8);
        }
        ImGui::SameLine();
        InputClamped(Tr("ghostshipTextScreen_width"), &dialog.width, 0, 320);
        ImGui::SameLine();
        InputClamped(Tr("ghostshipTextScreen_leftOffset"), &dialog.leftOffset, 0, 320);
        entry.fieldsModified = !SameFields(dialog, mOriginalDialogs[index]);
    }

    void Stage(StageManager& stage, const TextEntryList& list, const std::vector<int>& modified) override {
        if (mKind == GhostshipTextKind::Strings) {
            auto dictionary = mStrings.value_or(Sm64::Dictionary{});
            for (const int i : modified) {
                dictionary[list.entries[i].path] = *Sm64::EncodeText(list.entries[i].text);
            }
            const std::string path = std::string(Sm64::kStringsDir) + "/" + std::string(Sm64::kStringsName);
            stage.AddResourceData(std::string(Sm64::kStringsDir), std::string(Sm64::kStringsName),
                                  Sm64::ExportDictionary(dictionary, path));
            return;
        }
        for (const int i : modified) {
            const auto& entry = list.entries[i];
            const auto bytes = *Sm64::EncodeText(entry.text);
            std::vector<char> data;
            if (mKind == GhostshipTextKind::Dialogs) {
                Sm64::Dialog dialog = mDialogs[i];
                dialog.text = bytes;
                data = Sm64::ExportDialog(dialog, entry.path);
            } else {
                data = Sm64::ExportText(bytes, entry.path);
            }
            stage.AddResourceData(Paths::Dirname(entry.path), Paths::Basename(entry.path), std::move(data));
        }
    }

  private:
    GhostshipTextKind mKind;
    std::optional<Sm64::Dictionary> mStrings;
    std::vector<Sm64::Dialog> mDialogs;
    std::vector<Sm64::Dialog> mOriginalDialogs;
};
} // namespace

GhostshipTextScreen::GhostshipTextScreen(GhostshipTextKind kind)
    : TextEditorScreen({ .id = "GhostshipText",
                         .titleKey = TitleKey(kind),
                         .subtitleKey = SubtitleKey(kind),
                         .archivePathKey = "ghostshipTextScreen_archivePath",
                         .noTextKey = "ghostshipTextScreen_noText",
                         .hintKey = "ghostshipTextScreen_hint" },
                       std::make_shared<GhostshipText>(kind)) {
}
