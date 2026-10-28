#include "PaperboatTextScreen.h"

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
#include "utils/Paths.h"

#include <algorithm>
#include <imgui.h>
#include <imgui_stdlib.h>

namespace {
std::string ToLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return value;
}

std::string Preview(const std::string& text) {
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

ImU32 ToImColor(Clay_Color color) {
    return IM_COL32((int)color.r, (int)color.g, (int)color.b, (int)color.a);
}
} // namespace

bool PaperboatTextScreen::Item::Modified() const {
    return text != original;
}

void PaperboatTextScreen::Item::Validate() {
    error.clear();
    Pm64::EncodeText(text, charset, &error);
}

std::vector<PaperboatTextScreen::Item> PaperboatTextScreen::BuildItems(const Pm64::Messages& messages) {
    std::vector<Item> items;
    items.reserve(messages.size());
    for (const auto& [path, bytes] : messages) {
        Item item;
        item.path = path;
        item.name = Paths::Basename(path);
        item.charset = Pm64::CharsetFor(path);
        item.original = Pm64::DecodeText(bytes, item.charset);
        item.text = item.original;
        items.push_back(std::move(item));
    }
    return items;
}

void PaperboatTextScreen::ApplyMod(State& state, const Pm64::Messages& mod) {
    for (auto& item : state.items) {
        if (const auto it = mod.find(item.path); it != mod.end()) {
            item.text = Pm64::DecodeText(it->second, item.charset);
        }
        item.Validate();
    }
}

void PaperboatTextScreen::Filter(State& state) {
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

void PaperboatTextScreen::OnSelectArchive() {
    const auto selected = FileDialog::PickFile(FileDialog::kArchiveFilters);
    if (!selected) {
        return;
    }
    mState->archivePath = *selected;
    mState->isProcessing = true;

    auto messages = std::make_shared<Pm64::Messages>();
    Jobs::Instance().Run(
        [path = *selected, messages] {
            if (auto archive = Otr::ArchiveReader::Open(path)) {
                *messages = Pm64::LoadMessages(*archive);
            }
        },
        [state = mState, path = *selected, messages] {
            if (state->archivePath == path) {
                state->items = BuildItems(*messages);
                state->modPath.reset();
                state->selected = state->items.empty() ? -1 : 0;
                state->loaded = true;
                Filter(*state);
            }
            state->isProcessing = false;
        });
}

void PaperboatTextScreen::OnLoadMod() {
    const auto selected = FileDialog::PickFile(FileDialog::kArchiveFilters);
    if (!selected) {
        return;
    }
    mState->modPath = *selected;
    mState->isProcessing = true;

    auto messages = std::make_shared<Pm64::Messages>();
    Jobs::Instance().Run(
        [path = *selected, messages] {
            if (auto archive = Otr::ArchiveReader::Open(path)) {
                *messages = Pm64::LoadMessages(*archive);
            }
        },
        [state = mState, path = *selected, messages] {
            if (state->modPath == path) {
                ApplyMod(*state, *messages);
                Filter(*state);
            }
            state->isProcessing = false;
        });
}

void PaperboatTextScreen::StageChanges(AppContext& ctx) {
    std::vector<const Item*> modified;
    for (const auto& item : mState->items) {
        if (!item.Modified()) {
            continue;
        }
        if (!item.error.empty()) {
            ctx.toasts.Error(Tr("paperboatTextScreen_invalidText"));
            return;
        }
        modified.push_back(&item);
    }
    if (modified.empty()) {
        ctx.toasts.Error(Tr("paperboatTextScreen_nothingChanged"));
        return;
    }

    for (const Item* item : modified) {
        const auto bytes = *Pm64::EncodeText(item->text, item->charset);
        ctx.stage.AddResourceData(Paths::Dirname(item->path), Paths::Basename(item->path),
                                  Pm64::ExportMessage(bytes, item->path));
    }
}

void PaperboatTextScreen::DrawEditor(float width, float height) {
    auto& state = *mState;
    const float listWidth = std::max(240.0f, width * 0.35f);

    ImGui::PushFont(Theme::GetFont(FONT_REGULAR), (float)TextSize::BodyMedium);
    ImGui::BeginChild("##PaperboatTextList", ImVec2(listWidth, height), ImGuiChildFlags_Borders);
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
            ImGui::SameLine(listWidth * 0.5f);
            ImGui::TextDisabled("%s", Preview(item.text).c_str());
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();

    ImGui::BeginChild("##PaperboatTextEditor", ImVec2(0, height));
    if (state.selected >= 0 && state.selected < (int)state.items.size()) {
        auto& item = state.items[state.selected];

        ImGui::PushFont(Theme::GetFont(FONT_MEDIUM), (float)TextSize::TitleLarge);
        ImGui::TextUnformatted(item.name.c_str());
        ImGui::PopFont();

        if (item.Modified()) {
            ImGui::SameLine();
            if (ImGui::SmallButton(Tr("paperboatTextScreen_revert"))) {
                item.text = item.original;
                item.Validate();
            }
        }
        if (item.charset == Pm64::Charset::Credits) {
            ImGui::TextDisabled("%s", Tr("paperboatTextScreen_creditsCharset"));
        }

        const float lineHeight = ImGui::GetTextLineHeightWithSpacing();
        const float originalHeight = std::max(lineHeight * 3, ImGui::GetContentRegionAvail().y * 0.3f);
        const float editorHeight = ImGui::GetContentRegionAvail().y - originalHeight - lineHeight * 4;
        ImGui::PushFont(Theme::GetFont(FONT_MONO), (float)TextSize::BodyLarge);
        if (ImGui::InputTextMultiline("##PaperboatText", &item.text,
                                      ImVec2(-1, std::max(editorHeight, lineHeight * 3)))) {
            item.Validate();
        }
        ImGui::PopFont();

        if (!item.error.empty()) {
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(ToImColor(RetroColors::WildWatermelon)), "%s",
                               item.error.c_str());
        } else {
            ImGui::TextDisabled("%s", Tr("paperboatTextScreen_hint"));
        }

        ImGui::TextDisabled("%s", Tr("paperboatTextScreen_original"));
        ImGui::BeginChild("##PaperboatTextOriginal", ImVec2(0, 0), ImGuiChildFlags_Borders);
        ImGui::PushFont(Theme::GetFont(FONT_MONO), (float)TextSize::BodyLarge);
        ImGui::TextWrapped("%s", item.original.c_str());
        ImGui::PopFont();
        ImGui::EndChild();
    }
    ImGui::EndChild();
    ImGui::PopFont();
}

void PaperboatTextScreen::Layout(AppContext& ctx) {
    auto& ui = ctx.ui;
    ScaffoldOptions options;
    options.title = Tr("paperboatTextScreen_strings");
    options.subtitle = Tr("paperboatTextScreen_stringsSubtitle");
    options.onBackButtonPressed = [&] { ctx.router.Pop(); };
    if (mState->loaded) {
        options.topRightWidget = [&] {
            Widgets::SearchField(ui, "PaperboatText_Search", &mState->query, Tr("paperboatTextScreen_search"), 300,
                                 [state = mState] { Filter(*state); });
        };
    }

    const ClayUi::WidgetFn editor = [this](ImVec2 min, ImVec2 max) { DrawEditor(max.x - min.x, max.y - min.y); };
    const bool hasChanges =
        std::any_of(mState->items.begin(), mState->items.end(), [](const Item& item) { return item.Modified(); });

    CustomScaffold(ui, options, [&] {
        CLAY({ .id = CLAY_ID("PaperboatText_Body"),
               .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                           .padding = CLAY_PADDING_ALL(20),
                           .childGap = 10,
                           .childAlignment = { .x = CLAY_ALIGN_X_CENTER },
                           .layoutDirection = CLAY_TOP_TO_BOTTOM } }) {
            CLAY({ .id = CLAY_ID("PaperboatText_Picker"),
                   .layout = { .sizing = { CLAY_SIZING_GROW(0) },
                               .childGap = 12,
                               .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
                Widgets::OutlinedField(ui, "PaperboatText_Path",
                                       mState->archivePath.value_or(Tr("paperboatTextScreen_archivePath")));
                if (Widgets::Button(ui, "PaperboatText_Select", Tr("paperboatTextScreen_selectButton"),
                                    { CLAY_SIZING_FIT(100), CLAY_SIZING_FIXED(50) }, !mState->isProcessing)) {
                    OnSelectArchive();
                }
                if (Widgets::Button(ui, "PaperboatText_LoadMod", Tr("paperboatTextScreen_loadModButton"),
                                    { CLAY_SIZING_FIT(100), CLAY_SIZING_FIXED(50) },
                                    mState->loaded && !mState->items.empty() && !mState->isProcessing,
                                    Widgets::OutlinedStyle())) {
                    OnLoadMod();
                }
            }
            if (mState->isProcessing) {
                Widgets::CenteredSpinner(ui, "PaperboatText_Spinner");
            } else if (mState->loaded && mState->items.empty()) {
                CLAY({ .id = CLAY_ID("PaperboatText_Empty"),
                       .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                                   .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } } }) {
                    Widgets::Text(ui, Tr("paperboatTextScreen_noText"), TextSize::BodyLarge, RetroColors::Grey);
                }
            } else if (mState->loaded) {
                CLAY({ .id = CLAY_ID("PaperboatText_Editor"),
                       .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } },
                       .custom = ui.Widget(editor) }) {
                }
            } else {
                Widgets::Spacer();
            }
            const float width = ImGui::GetMainViewport()->WorkSize.x * 0.5f;
            if (Widgets::Button(ui, "PaperboatText_Stage", Tr("paperboatTextScreen_stageChanges"),
                                { CLAY_SIZING_FIXED(width), CLAY_SIZING_FIXED(50) },
                                hasChanges && !mState->isProcessing)) {
                StageChanges(ctx);
            }
        }
    });
}
