#include "GhostshipTextScreen.h"

#include "app/AppContext.h"
#include "app/Router.h"
#include "l10n/Strings.h"
#include "otr/Archive.h"
#include "services/StageManager.h"
#include "ui/Toasts.h"
#include "ui/clay/ClayUi.h"
#include "ui/components/CustomScaffold.h"
#include "ui/components/Widgets.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"
#include "utils/FileDialog.h"
#include "utils/Jobs.h"

#include <algorithm>
#include <imgui.h>
#include <imgui_stdlib.h>

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

std::string ToLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return value;
}

std::string Basename(const std::string& path) {
    const auto slash = path.rfind('/');
    return slash == std::string::npos ? path : path.substr(slash + 1);
}

std::string Dirname(const std::string& path) {
    const auto slash = path.rfind('/');
    return slash == std::string::npos ? "." : path.substr(0, slash);
}

std::string Preview(const std::string& text) {
    std::string line = text.substr(0, text.find('\n'));
    if (line.size() > 48) {
        line = line.substr(0, 48) + "...";
    }
    return line;
}

ImU32 ToImColor(Clay_Color color) {
    return IM_COL32((int)color.r, (int)color.g, (int)color.b, (int)color.a);
}

void InputClamped(const char* label, int16_t* value, int min, int max) {
    int current = *value;
    ImGui::SetNextItemWidth(110);
    if (ImGui::InputInt(label, &current)) {
        *value = (int16_t)std::clamp(current, min, max);
    }
}
} // namespace

bool GhostshipTextScreen::Item::Modified() const {
    return text != original || dialog.linesPerBox != originalDialog.linesPerBox ||
           dialog.leftOffset != originalDialog.leftOffset || dialog.width != originalDialog.width;
}

void GhostshipTextScreen::Item::Validate() {
    error.clear();
    Sm64::EncodeText(text, &error);
}

std::vector<GhostshipTextScreen::Item> GhostshipTextScreen::BuildItems(GhostshipTextKind kind,
                                                                       const Sm64::TextAssets& assets) {
    std::vector<Item> items;
    const auto add = [&](const std::string& path, std::string name, const std::vector<uint8_t>& bytes,
                         const Sm64::Dialog& dialog) {
        Item item;
        item.path = path;
        item.name = std::move(name);
        item.original = Sm64::DecodeText(bytes);
        item.text = item.original;
        item.originalDialog = dialog;
        item.dialog = dialog;
        items.push_back(std::move(item));
    };
    if (kind == GhostshipTextKind::Dialogs) {
        for (const auto& [path, dialog] : assets.dialogs) {
            add(path, Basename(path), dialog.text, dialog);
        }
    } else if (kind == GhostshipTextKind::Texts) {
        for (const auto& [path, text] : assets.texts) {
            add(path, Basename(Dirname(path)) + "/" + Basename(path), text, {});
        }
    } else if (assets.strings) {
        for (const auto& [key, value] : *assets.strings) {
            add(key, key, value, {});
        }
    }
    return items;
}

void GhostshipTextScreen::ApplyMod(State& state, const Sm64::TextAssets& mod) {
    for (auto& item : state.items) {
        if (state.kind == GhostshipTextKind::Dialogs) {
            if (const auto it = mod.dialogs.find(item.path); it != mod.dialogs.end()) {
                item.dialog = it->second;
                item.text = Sm64::DecodeText(it->second.text);
            }
        } else if (state.kind == GhostshipTextKind::Texts) {
            if (const auto it = mod.texts.find(item.path); it != mod.texts.end()) {
                item.text = Sm64::DecodeText(it->second);
            }
        } else if (mod.strings) {
            if (const auto it = mod.strings->find(item.path); it != mod.strings->end()) {
                item.text = Sm64::DecodeText(it->second);
            }
        }
        item.Validate();
    }
}

void GhostshipTextScreen::Filter(State& state) {
    const std::string query = ToLower(state.query);
    state.filtered.clear();
    for (int i = 0; i < (int)state.items.size(); i++) {
        const auto& item = state.items[i];
        if (ToLower(item.name).find(query) != std::string::npos ||
            ToLower(item.text).find(query) != std::string::npos) {
            state.filtered.push_back(i);
        }
    }
}

void GhostshipTextScreen::OnSelectArchive() {
    const auto selected = FileDialog::PickFile(FileDialog::kArchiveFilters);
    if (!selected) {
        return;
    }
    mState->kind = mKind;
    mState->archivePath = *selected;
    mState->isProcessing = true;

    auto assets = std::make_shared<Sm64::TextAssets>();
    Jobs::Instance().Run(
        [path = *selected, assets] {
            if (auto archive = Otr::ArchiveReader::Open(path)) {
                *assets = Sm64::LoadTextAssets(*archive);
            }
        },
        [state = mState, path = *selected, assets] {
            if (state->archivePath == path) {
                state->strings = assets->strings;
                state->items = BuildItems(state->kind, *assets);
                state->modPath.reset();
                state->selected = state->items.empty() ? -1 : 0;
                state->loaded = true;
                Filter(*state);
            }
            state->isProcessing = false;
        });
}

void GhostshipTextScreen::OnLoadMod() {
    const auto selected = FileDialog::PickFile(FileDialog::kArchiveFilters);
    if (!selected) {
        return;
    }
    mState->modPath = *selected;
    mState->isProcessing = true;

    auto assets = std::make_shared<Sm64::TextAssets>();
    Jobs::Instance().Run(
        [path = *selected, assets] {
            if (auto archive = Otr::ArchiveReader::Open(path)) {
                *assets = Sm64::LoadTextAssets(*archive);
            }
        },
        [state = mState, path = *selected, assets] {
            if (state->modPath == path) {
                ApplyMod(*state, *assets);
                Filter(*state);
            }
            state->isProcessing = false;
        });
}

void GhostshipTextScreen::StageChanges(AppContext& ctx) {
    std::vector<const Item*> modified;
    for (const auto& item : mState->items) {
        if (!item.Modified()) {
            continue;
        }
        if (!item.error.empty()) {
            ctx.toasts.Error(Tr("ghostshipTextScreen_invalidText"));
            return;
        }
        modified.push_back(&item);
    }
    if (modified.empty()) {
        ctx.toasts.Error(Tr("ghostshipTextScreen_nothingChanged"));
        return;
    }

    if (mKind == GhostshipTextKind::Strings) {
        auto dictionary = mState->strings.value_or(Sm64::Dictionary{});
        for (const Item* item : modified) {
            dictionary[item->path] = *Sm64::EncodeText(item->text);
        }
        const std::string path = std::string(Sm64::kStringsDir) + "/" + std::string(Sm64::kStringsName);
        ctx.stage.AddResourceData(std::string(Sm64::kStringsDir), std::string(Sm64::kStringsName),
                                  Sm64::ExportDictionary(dictionary, path));
    } else {
        for (const Item* item : modified) {
            const auto bytes = *Sm64::EncodeText(item->text);
            std::vector<char> data;
            if (mKind == GhostshipTextKind::Dialogs) {
                Sm64::Dialog dialog = item->dialog;
                dialog.text = bytes;
                data = Sm64::ExportDialog(dialog, item->path);
            } else {
                data = Sm64::ExportText(bytes, item->path);
            }
            ctx.stage.AddResourceData(Dirname(item->path), Basename(item->path), std::move(data));
        }
    }
}

void GhostshipTextScreen::DrawEditor(float width, float height) {
    auto& state = *mState;
    const float listWidth = std::max(240.0f, width * 0.35f);

    ImGui::PushFont(Theme::GetFont(FONT_REGULAR), (float)TextSize::BodyMedium);
    ImGui::BeginChild("##GhostshipTextList", ImVec2(listWidth, height), ImGuiChildFlags_Borders);
    ImGuiListClipper clipper;
    clipper.Begin((int)state.filtered.size());
    while (clipper.Step()) {
        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
            const int index = state.filtered[row];
            const auto& item = state.items[index];
            const bool modified = item.Modified();
            const bool invalid = !item.error.empty();
            if (invalid || modified) {
                ImGui::PushStyleColor(ImGuiCol_Text,
                                      ToImColor(invalid ? RetroColors::WildWatermelon : RetroColors::Goldenrod));
            }
            ImGui::PushID(index);
            if (ImGui::Selectable(item.name.c_str(), state.selected == index, ImGuiSelectableFlags_AllowOverlap)) {
                state.selected = index;
            }
            ImGui::PopID();
            if (invalid || modified) {
                ImGui::PopStyleColor();
            }
            ImGui::SameLine(listWidth * 0.45f);
            ImGui::TextDisabled("%s", Preview(item.text).c_str());
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();

    ImGui::BeginChild("##GhostshipTextEditor", ImVec2(0, height));
    if (state.selected >= 0 && state.selected < (int)state.items.size()) {
        auto& item = state.items[state.selected];

        ImGui::PushFont(Theme::GetFont(FONT_MEDIUM), (float)TextSize::TitleLarge);
        ImGui::TextUnformatted(item.name.c_str());
        ImGui::PopFont();

        if (item.Modified()) {
            ImGui::SameLine();
            if (ImGui::SmallButton(Tr("ghostshipTextScreen_revert"))) {
                item.text = item.original;
                item.dialog = item.originalDialog;
                item.Validate();
            }
        }

        if (state.kind == GhostshipTextKind::Dialogs) {
            int lines = item.dialog.linesPerBox;
            ImGui::SetNextItemWidth(110);
            if (ImGui::InputInt(Tr("ghostshipTextScreen_linesPerBox"), &lines)) {
                item.dialog.linesPerBox = (int8_t)std::clamp(lines, 1, 8);
            }
            ImGui::SameLine();
            InputClamped(Tr("ghostshipTextScreen_width"), &item.dialog.width, 0, 320);
            ImGui::SameLine();
            InputClamped(Tr("ghostshipTextScreen_leftOffset"), &item.dialog.leftOffset, 0, 320);
        }

        const float lineHeight = ImGui::GetTextLineHeightWithSpacing();
        const float originalHeight = std::max(lineHeight * 3, ImGui::GetContentRegionAvail().y * 0.3f);
        const float editorHeight = ImGui::GetContentRegionAvail().y - originalHeight - lineHeight * 4;
        ImGui::PushFont(Theme::GetFont(FONT_MONO), (float)TextSize::BodyLarge);
        if (ImGui::InputTextMultiline("##GhostshipText", &item.text,
                                      ImVec2(-1, std::max(editorHeight, lineHeight * 3)))) {
            item.Validate();
        }
        ImGui::PopFont();

        if (!item.error.empty()) {
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(ToImColor(RetroColors::WildWatermelon)), "%s",
                               item.error.c_str());
        } else {
            ImGui::TextDisabled("%s", Tr("ghostshipTextScreen_hint"));
        }

        ImGui::TextDisabled("%s", Tr("ghostshipTextScreen_original"));
        ImGui::BeginChild("##GhostshipTextOriginal", ImVec2(0, 0), ImGuiChildFlags_Borders);
        ImGui::PushFont(Theme::GetFont(FONT_MONO), (float)TextSize::BodyLarge);
        ImGui::TextWrapped("%s", item.original.c_str());
        ImGui::PopFont();
        ImGui::EndChild();
    }
    ImGui::EndChild();
    ImGui::PopFont();
}

void GhostshipTextScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    ScaffoldOptions options;
    options.title = Tr(TitleKey(mKind));
    options.subtitle = Tr(SubtitleKey(mKind));
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };
    if (mState->loaded) {
        options.topRightWidget = [&] {
            Widgets::SearchField(ui, "GhostshipText_Search", &mState->query, Tr("ghostshipTextScreen_search"), 300,
                                 [state = mState] { Filter(*state); });
        };
    }

    const ClayUi::WidgetFn editor = [this](ImVec2 min, ImVec2 max) { DrawEditor(max.x - min.x, max.y - min.y); };
    const bool hasChanges =
        std::any_of(mState->items.begin(), mState->items.end(), [](const Item& item) { return item.Modified(); });

    CustomScaffold(ui, options, [&] {
        CLAY({ .id = CLAY_ID("GhostshipText_Body"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                           .padding = CLAY_PADDING_ALL(20),
                           .childGap = 10,
                           .childAlignment = { .x = CLAY_ALIGN_X_CENTER },
                           .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
            CLAY({ .id = CLAY_ID("GhostshipText_Picker"),
                   .layout = { .sizing = { CLAY_SIZING_GROW(0) },
                               .childGap = 12,
                               .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
                Widgets::OutlinedField(ui, "GhostshipText_Path",
                                       mState->archivePath.value_or(Tr("ghostshipTextScreen_archivePath")));
                if (Widgets::Button(ui, "GhostshipText_Select", Tr("ghostshipTextScreen_selectButton"),
                                    { CLAY_SIZING_FIT(100), CLAY_SIZING_FIXED(50) }, !mState->isProcessing)) {
                    OnSelectArchive();
                }
                if (Widgets::Button(ui, "GhostshipText_LoadMod", Tr("ghostshipTextScreen_loadModButton"),
                                    { CLAY_SIZING_FIT(100), CLAY_SIZING_FIXED(50) },
                                    mState->loaded && !mState->items.empty() && !mState->isProcessing,
                                    Widgets::OutlinedStyle())) {
                    OnLoadMod();
                }
            }
            if (mState->isProcessing) {
                Widgets::CenteredSpinner(ui, "GhostshipText_Spinner");
            } else if (mState->loaded && mState->items.empty()) {
                CLAY({ .id = CLAY_ID("GhostshipText_Empty"),
                       .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                                   .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } } }) {
                    Widgets::Text(ui, Tr("ghostshipTextScreen_noText"), TextSize::BodyLarge, RetroColors::Grey);
                }
            } else if (mState->loaded) {
                CLAY({ .id = CLAY_ID("GhostshipText_Editor"),
                       .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } },
                       .custom = ui.Widget(editor) }) {
                }
            } else {
                Widgets::Spacer();
            }
            const float width = ImGui::GetMainViewport()->WorkSize.x * 0.5f;
            if (Widgets::Button(ui, "GhostshipText_Stage", Tr("ghostshipTextScreen_stageChanges"),
                                { CLAY_SIZING_FIXED(width), CLAY_SIZING_FIXED(50) },
                                hasChanges && !mState->isProcessing)) {
                StageChanges(ctx);
            }
        }
    });
}
