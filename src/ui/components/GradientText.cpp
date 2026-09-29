#include "GradientText.h"
#include "ui/clay/ClayUi.h"
#include "ui/theme/Theme.h"

#include <cfloat>
#include <cmath>
#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <vector>

namespace {
ImU32 SampleGradient(std::span<const Clay_Color> colors, float t) {
    const float scaled = t * (float)colors.size();
    const size_t index = (size_t)scaled % colors.size();
    const Clay_Color& a = colors[index];
    const Clay_Color& b = colors[(index + 1) % colors.size()];
    const float f = scaled - std::floor(scaled);
    return IM_COL32((int)(a.r + (b.r - a.r) * f), (int)(a.g + (b.g - a.g) * f), (int)(a.b + (b.b - a.b) * f),
                    (int)(a.a + (b.a - a.a) * f));
}
} // namespace

void GradientText(ClayUi& ui, std::string_view id, std::string_view text, uint16_t fontId, uint16_t fontSize,
                  std::span<const Clay_Color> colors, float durationSeconds) {
    ImFont* font = Theme::GetFont(fontId);
    const float size = (float)fontSize;
    const ImVec2 extent = font->CalcTextSizeA(size, FLT_MAX, 0.0f, text.data(), text.data() + text.size());

    CLAY({ .id = CLAY_SID(ui.Str(id)),
           .layout = { .sizing = { CLAY_SIZING_FIXED(extent.x), CLAY_SIZING_FIXED(extent.y) } },
           .custom = ui.Widget([font, size, text = std::string(text),
                                colors = std::vector<Clay_Color>(colors.begin(), colors.end()),
                                durationSeconds](ImVec2 min, ImVec2 max) {
               if (colors.empty()) {
                   return;
               }
               ImDrawList* dl = ImGui::GetWindowDrawList();
               const float phase = std::fmod((float)ImGui::GetTime() / durationSeconds, 1.0f);
               const float width = max.x - min.x;
               const char* begin = text.data();
               const char* end = begin + text.size();
               ImVec2 pos = min;
               while (begin < end) {
                   unsigned int c;
                   const char* next = begin + ImTextCharFromUtf8(&c, begin, end);
                   const float t = std::fmod(phase + (pos.x - min.x) / width, 1.0f);
                   dl->AddText(font, size, pos, SampleGradient(colors, t), begin, next);
                   pos.x += font->CalcTextSizeA(size, FLT_MAX, 0.0f, begin, next).x;
                   begin = next;
               }
           }) }) {
    }
}
