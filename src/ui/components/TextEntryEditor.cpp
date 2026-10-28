#include "TextEntryEditor.h"

#include "l10n/Strings.h"
#include "ui/clay/ClayUi.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"

#include <algorithm>
#include <imgui.h>
#include <imgui_stdlib.h>

namespace {
std::string ToLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return value;
}

ImU32 ToImColor(Clay_Color color) {
    return IM_COL32((int)color.r, (int)color.g, (int)color.b, (int)color.a);
}

std::string ChildId(std::string_view id, std::string_view suffix) {
    return "##" + std::string(id) + std::string(suffix);
}
} // namespace

void TextEntryList::Filter() {
    const std::string lowered = ToLower(query);
    filtered.clear();
    for (int i = 0; i < (int)entries.size(); i++) {
        const auto& entry = entries[i];
        if (ToLower(entry.name).find(lowered) != std::string::npos ||
            ToLower(entry.text).find(lowered) != std::string::npos) {
            filtered.push_back(i);
        }
    }
}

bool TextEntryList::HasChanges() const {
    return std::any_of(entries.begin(), entries.end(), [](const TextEntry& entry) { return entry.Modified(); });
}

std::string TextEntryPreview(const std::string& text) {
    const size_t end = std::min(text.find('\n'), text.size());
    size_t cut = std::min<size_t>(end, 48);
    while (cut > 0 && cut < end && ((uint8_t)text[cut] & 0xC0) == 0x80) {
        cut--;
    }
    return cut < end ? text.substr(0, cut) + "..." : text.substr(0, cut);
}

void TextEntryEditor(TextEntryList& list, const TextEntryEditorOptions& options, float width, float height) {
    const float listWidth = std::max(240.0f, width * 0.35f);
    const auto preview = options.preview ? options.preview : TextEntryPreview;

    ImGui::PushFont(Theme::GetFont(FONT_REGULAR), (float)TextSize::BodyMedium);
    ImGui::BeginChild(ChildId(options.id, "List").c_str(), ImVec2(listWidth, height), ImGuiChildFlags_Borders);
    ImGuiListClipper clipper;
    clipper.Begin((int)list.filtered.size());
    while (clipper.Step()) {
        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
            const int index = list.filtered[row];
            const auto& entry = list.entries[index];
            const bool modified = entry.Modified();
            const bool invalid = !entry.error.empty();
            if (invalid || modified) {
                ImGui::PushStyleColor(ImGuiCol_Text,
                                      ToImColor(invalid ? RetroColors::WildWatermelon : RetroColors::Goldenrod));
            }
            ImGui::PushID(index);
            if (ImGui::Selectable(entry.name.c_str(), list.selected == index, ImGuiSelectableFlags_AllowOverlap)) {
                list.selected = index;
            }
            ImGui::PopID();
            if (invalid || modified) {
                ImGui::PopStyleColor();
            }
            ImGui::SameLine(listWidth * 0.5f);
            ImGui::TextDisabled("%s", preview(entry.text).c_str());
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();

    ImGui::BeginChild(ChildId(options.id, "Editor").c_str(), ImVec2(0, height));
    if (list.selected >= 0 && list.selected < (int)list.entries.size()) {
        const int index = list.selected;
        auto& entry = list.entries[index];

        ImGui::PushFont(Theme::GetFont(FONT_MEDIUM), (float)TextSize::TitleLarge);
        ImGui::TextUnformatted(entry.name.c_str());
        ImGui::PopFont();

        if (entry.Modified()) {
            ImGui::SameLine();
            if (ImGui::SmallButton(Tr("textEditorScreen_revert"))) {
                entry.text = entry.original;
                entry.fieldsModified = false;
                if (options.revert) {
                    options.revert(entry, index);
                }
                options.validate(entry, index);
            }
        }

        if (options.drawFields) {
            options.drawFields(entry, index);
        }

        const float lineHeight = ImGui::GetTextLineHeightWithSpacing();
        const float originalHeight = std::max(lineHeight * 3, ImGui::GetContentRegionAvail().y * 0.3f);
        const float editorHeight = ImGui::GetContentRegionAvail().y - originalHeight - lineHeight * 4;
        ImGui::PushFont(Theme::GetFont(FONT_MONO), (float)TextSize::BodyLarge);
        if (ImGui::InputTextMultiline(ChildId(options.id, "Text").c_str(), &entry.text,
                                      ImVec2(-1, std::max(editorHeight, lineHeight * 3)))) {
            options.validate(entry, index);
        }
        ImGui::PopFont();

        if (!entry.error.empty()) {
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(ToImColor(RetroColors::WildWatermelon)), "%s",
                               entry.error.c_str());
        } else {
            ImGui::TextDisabled("%s", options.hint);
        }

        ImGui::TextDisabled("%s", Tr("textEditorScreen_original"));
        ImGui::BeginChild(ChildId(options.id, "Original").c_str(), ImVec2(0, 0), ImGuiChildFlags_Borders);
        ImGui::PushFont(Theme::GetFont(FONT_MONO), (float)TextSize::BodyLarge);
        ImGui::TextWrapped("%s", entry.original.c_str());
        ImGui::PopFont();
        ImGui::EndChild();
    }
    ImGui::EndChild();
    ImGui::PopFont();
}
