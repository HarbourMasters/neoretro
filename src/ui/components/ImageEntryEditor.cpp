#include "ImageEntryEditor.h"

#include "l10n/Strings.h"
#include "ui/clay/ClayUi.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"

#include <algorithm>
#include <cmath>
#include <imgui.h>

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

struct Row {
    int index;
    int count;
    bool header;
    bool modified;
};

std::vector<Row> BuildRows(const ImageEntryList& list) {
    const bool showAll = !list.query.empty();
    std::vector<Row> rows;
    int header = -1;
    for (const int index : list.filtered) {
        const auto& entry = list.entries[index];
        if (!entry.group.empty() && (header < 0 || list.entries[rows[header].index].group != entry.group)) {
            header = (int)rows.size();
            rows.push_back({ index, 0, true, false });
        }
        if (header >= 0) {
            rows[header].count++;
            rows[header].modified |= entry.Modified();
        }
        if (header < 0 || showAll || list.expanded.count(entry.group)) {
            rows.push_back({ index, 0, false, false });
        }
    }
    return rows;
}

void DrawCheckerboard(ImDrawList* drawList, ImVec2 min, ImVec2 max, float cell) {
    drawList->AddRectFilled(min, max, IM_COL32(60, 60, 60, 255));
    for (float y = min.y; y < max.y; y += cell) {
        for (float x = min.x + (std::fmod((y - min.y) / cell, 2.0f) >= 1.0f ? cell : 0.0f); x < max.x; x += cell * 2) {
            drawList->AddRectFilled(ImVec2(x, y), ImVec2(std::min(x + cell, max.x), std::min(y + cell, max.y)),
                                    IM_COL32(85, 85, 85, 255));
        }
    }
}
} // namespace

void ImageEntryList::Filter() {
    const std::string lowered = ToLower(query);
    filtered.clear();
    for (int i = 0; i < (int)entries.size(); i++) {
        const auto& entry = entries[i];
        if (ToLower(entry.path).find(lowered) != std::string::npos ||
            ToLower(entry.group).find(lowered) != std::string::npos ||
            ToLower(entry.name).find(lowered) != std::string::npos) {
            filtered.push_back(i);
        }
    }
}

bool ImageEntryList::HasChanges() const {
    return std::any_of(entries.begin(), entries.end(), [](const ImageEntry& entry) { return entry.Modified(); });
}

void ImageEntryEditor(ImageEntryList& list, ImageEntryEditorState& state, const ImageEntryEditorOptions& options,
                      float width, float height) {
    const float listWidth = std::max(240.0f, width * 0.35f);

    ImGui::PushFont(Theme::GetFont(FONT_REGULAR), (float)TextSize::BodyMedium);
    ImGui::BeginChild(ChildId(options.id, "List").c_str(), ImVec2(listWidth, height), ImGuiChildFlags_Borders);
    const auto rows = BuildRows(list);
    const float indent = ImGui::GetStyle().IndentSpacing;
    ImGuiListClipper clipper;
    clipper.Begin((int)rows.size());
    while (clipper.Step()) {
        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
            const int index = rows[row].index;
            const auto& entry = list.entries[index];
            if (rows[row].header) {
                const bool open = !list.query.empty() || list.expanded.count(entry.group);
                if (rows[row].modified) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ToImColor(RetroColors::Goldenrod));
                }
                ImGui::PushID(entry.group.c_str());
                ImGui::SetNextItemOpen(open);
                const bool nowOpen = ImGui::TreeNodeEx(
                    "##Group", ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth, "%s (%d)",
                    entry.group.c_str(), rows[row].count);
                ImGui::PopID();
                if (rows[row].modified) {
                    ImGui::PopStyleColor();
                }
                if (nowOpen != open && list.query.empty()) {
                    if (nowOpen) {
                        list.expanded.insert(entry.group);
                    } else {
                        list.expanded.erase(entry.group);
                    }
                }
                continue;
            }
            if (!entry.group.empty()) {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            }
            if (entry.Modified()) {
                ImGui::PushStyleColor(ImGuiCol_Text, ToImColor(RetroColors::Goldenrod));
            }
            ImGui::PushID(index);
            if (ImGui::Selectable(entry.name.c_str(), list.selected == index, ImGuiSelectableFlags_AllowOverlap)) {
                list.selected = index;
                state.error.clear();
            }
            ImGui::PopID();
            if (entry.Modified()) {
                ImGui::PopStyleColor();
            }
            ImGui::SameLine(listWidth * 0.6f);
            ImGui::TextDisabled("%dx%d", entry.original.width, entry.original.height);
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
        ImGui::TextDisabled("%s", entry.info.c_str());

        if (ImGui::Button(Tr("imageEditorScreen_exportPng"))) {
            options.exportPng(index);
        }
        ImGui::SameLine();
        if (ImGui::Button(Tr("imageEditorScreen_importPng"))) {
            options.importPng(index);
        }
        if (entry.Modified()) {
            ImGui::SameLine();
            if (ImGui::Button(Tr("imageEditorScreen_revert"))) {
                options.revert(index);
            }
            ImGui::SameLine();
            ImGui::Checkbox(Tr("imageEditorScreen_showOriginal"), &state.showOriginal);
        }

        if (!state.error.empty()) {
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(ToImColor(RetroColors::WildWatermelon)), "%s",
                               state.error.c_str());
        } else if (entry.Modified() && !entry.note.empty()) {
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(ToImColor(RetroColors::Goldenrod)), "%s",
                               entry.note.c_str());
        } else {
            ImGui::TextDisabled("%s", Tr("imageEditorScreen_hint"));
        }

        const bool showEdited = entry.Modified() && !state.showOriginal;
        if (state.shownIndex != index || state.shownEdited != showEdited || !state.texture.Valid()) {
            const Image& image = showEdited ? *entry.edited : entry.original;
            state.texture.Upload(image.rgba.data(), image.width, image.height);
            state.shownIndex = index;
            state.shownEdited = showEdited;
        }

        const ImVec2 area = ImGui::GetContentRegionAvail();
        if (state.texture.Valid() && area.x > 0 && area.y > 0) {
            const float fit = std::min(area.x / state.texture.Width(), area.y / state.texture.Height());
            const float scale = fit >= 1.0f ? std::floor(std::min(fit, 8.0f)) : fit;
            const ImVec2 size(state.texture.Width() * scale, state.texture.Height() * scale);
            const ImVec2 pos = ImGui::GetCursorScreenPos() + ImVec2((area.x - size.x) * 0.5f, 0);
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            DrawCheckerboard(drawList, pos, pos + size, std::max(4.0f, scale * 4));
            drawList->AddImage(ImTextureRef((ImTextureID)state.texture.Id()), pos, pos + size);
        }
    }
    ImGui::EndChild();
    ImGui::PopFont();
}
