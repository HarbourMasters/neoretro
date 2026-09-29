#include "ClayUi.h"
#include "ui/theme/Theme.h"

#include <algorithm>
#include <cfloat>
#include <cstdlib>
#include <imgui_internal.h>
#include <spdlog/spdlog.h>

namespace {
constexpr float SCROLL_SPEED = 40.0f;

ImU32 ToImColor(Clay_Color color) {
    return IM_COL32((int)color.r, (int)color.g, (int)color.b, (int)color.a);
}

void ToImRounding(const Clay_CornerRadius& radius, float& rounding, ImDrawFlags& flags) {
    rounding = std::max({ radius.topLeft, radius.topRight, radius.bottomLeft, radius.bottomRight });
    flags = ImDrawFlags_None;
    if (rounding <= 0.0f) {
        flags = ImDrawFlags_RoundCornersNone;
        return;
    }
    if (radius.topLeft > 0.0f) {
        flags |= ImDrawFlags_RoundCornersTopLeft;
    }
    if (radius.topRight > 0.0f) {
        flags |= ImDrawFlags_RoundCornersTopRight;
    }
    if (radius.bottomLeft > 0.0f) {
        flags |= ImDrawFlags_RoundCornersBottomLeft;
    }
    if (radius.bottomRight > 0.0f) {
        flags |= ImDrawFlags_RoundCornersBottomRight;
    }
}
} // namespace

bool ClayUi::Init() {
    const uint32_t size = Clay_MinMemorySize();
    mArenaMemory = std::malloc(size);
    if (mArenaMemory == nullptr) {
        SPDLOG_ERROR("Failed to allocate {} bytes for the Clay arena", size);
        return false;
    }

    const Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(size, mArenaMemory);
    const ImVec2 display = ImGui::GetIO().DisplaySize;
    mContext = Clay_Initialize(arena, { display.x, display.y }, { HandleError, this });
    Clay_SetMeasureTextFunction(MeasureText, this);
    return true;
}

void ClayUi::Shutdown() {
    mContext = nullptr;
    std::free(mArenaMemory);
    mArenaMemory = nullptr;
}

void ClayUi::BeginLayout() {
    Clay_SetCurrentContext(mContext);
    mFrameStrings.clear();
    mFrameWidgets.clear();

    const ImGuiIO& io = ImGui::GetIO();
    mOrigin = ImGui::GetWindowPos();
    const ImVec2 size = ImGui::GetWindowSize();
    mHostHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);

    const ImVec2 mouse = io.MousePos - mOrigin;
    Clay_SetLayoutDimensions({ size.x, size.y });
    Clay_SetPointerState({ mouse.x, mouse.y }, mHostHovered && io.MouseDown[ImGuiMouseButton_Left]);

    const Clay_Vector2 scroll = mHostHovered
                                    ? Clay_Vector2{ io.MouseWheelH * SCROLL_SPEED, io.MouseWheel * SCROLL_SPEED }
                                    : Clay_Vector2{ 0.0f, 0.0f };
    Clay_UpdateScrollContainers(false, scroll, io.DeltaTime);

    Clay_BeginLayout();
}

void ClayUi::EndLayout() {
    Render(Clay_EndLayout());
}

Clay_String ClayUi::Str(std::string_view text) {
    const std::string& stored = mFrameStrings.emplace_back(text);
    return { false, (int32_t)stored.size(), stored.c_str() };
}

Clay_ElementId ClayUi::Id(std::string_view id, uint32_t index) {
    return Clay__HashString({ false, (int32_t)id.size(), id.data() }, index, 0);
}

Clay_CustomElementConfig ClayUi::Widget(WidgetFn fn) {
    return { &mFrameWidgets.emplace_back(std::move(fn)) };
}

bool ClayUi::Hovered() const {
    return mHostHovered && Clay_Hovered() && !ImGui::IsAnyItemHovered();
}

bool ClayUi::Clicked() const {
    return Hovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
}

bool ClayUi::Hovered(Clay_ElementId id) const {
    return mHostHovered && Clay_PointerOver(id) && !ImGui::IsAnyItemHovered();
}

bool ClayUi::Clicked(Clay_ElementId id) const {
    return Hovered(id) && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
}

void ClayUi::ToggleDebug() {
    Clay_SetCurrentContext(mContext);
    Clay_SetDebugModeEnabled(!Clay_IsDebugModeEnabled());
}

Clay_Dimensions ClayUi::MeasureText(Clay_StringSlice text, Clay_TextElementConfig* config, void* userData) {
    ImFont* font = Theme::GetFont(config->fontId);
    const ImVec2 size =
        font->CalcTextSizeA((float)config->fontSize, FLT_MAX, 0.0f, text.chars, text.chars + text.length);
    const float spacing = (float)config->letterSpacing * (float)std::max(text.length - 1, 0);
    const float height = config->lineHeight > 0 ? (float)config->lineHeight : size.y;
    return { size.x + spacing, height };
}

void ClayUi::HandleError(Clay_ErrorData error) {
    SPDLOG_ERROR("Clay: {}", std::string_view(error.errorText.chars, error.errorText.length));
}

void ClayUi::Render(Clay_RenderCommandArray commands) {
    ImDrawList* dl = ImGui::GetWindowDrawList();

    for (int32_t i = 0; i < commands.length; i++) {
        const Clay_RenderCommand* cmd = Clay_RenderCommandArray_Get(&commands, i);
        const Clay_BoundingBox& box = cmd->boundingBox;
        const ImVec2 min = mOrigin + ImVec2(box.x, box.y);
        const ImVec2 max = min + ImVec2(box.width, box.height);

        switch (cmd->commandType) {
            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
                const auto& rect = cmd->renderData.rectangle;
                float rounding;
                ImDrawFlags flags;
                ToImRounding(rect.cornerRadius, rounding, flags);
                dl->AddRectFilled(min, max, ToImColor(rect.backgroundColor), rounding, flags);
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_BORDER: {
                const auto& border = cmd->renderData.border;
                const ImU32 color = ToImColor(border.color);
                const auto& w = border.width;
                if (w.left == w.right && w.left == w.top && w.left == w.bottom) {
                    if (w.left > 0) {
                        float rounding;
                        ImDrawFlags flags;
                        ToImRounding(border.cornerRadius, rounding, flags);
                        const float half = w.left * 0.5f;
                        dl->AddRect(min + ImVec2(half, half), max - ImVec2(half, half), color, rounding, flags,
                                    (float)w.left);
                    }
                } else {
                    dl->AddRectFilled(min, ImVec2(min.x + w.left, max.y), color);
                    dl->AddRectFilled(ImVec2(max.x - w.right, min.y), max, color);
                    dl->AddRectFilled(min, ImVec2(max.x, min.y + w.top), color);
                    dl->AddRectFilled(ImVec2(min.x, max.y - w.bottom), max, color);
                }
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_TEXT: {
                const auto& text = cmd->renderData.text;
                ImFont* font = Theme::GetFont(text.fontId);
                const char* begin = text.stringContents.chars;
                const char* end = begin + text.stringContents.length;
                const float size = (float)text.fontSize;
                const ImU32 color = ToImColor(text.textColor);

                if (text.letterSpacing == 0) {
                    dl->AddText(font, size, min, color, begin, end);
                    break;
                }

                ImVec2 pos = min;
                while (begin < end) {
                    unsigned int c;
                    const char* next = begin + ImTextCharFromUtf8(&c, begin, end);
                    dl->AddText(font, size, pos, color, begin, next);
                    pos.x += font->CalcTextSizeA(size, FLT_MAX, 0.0f, begin, next).x + text.letterSpacing;
                    begin = next;
                }
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
                const auto& image = cmd->renderData.image;
                const Clay_Color tint = image.backgroundColor;
                const bool untinted = tint.r == 0 && tint.g == 0 && tint.b == 0 && tint.a == 0;
                float rounding;
                ImDrawFlags flags;
                ToImRounding(image.cornerRadius, rounding, flags);
                dl->AddImageRounded(ImTextureRef((ImTextureID)(intptr_t)image.imageData), min, max, ImVec2(0, 0),
                                    ImVec2(1, 1), untinted ? IM_COL32_WHITE : ToImColor(tint), rounding, flags);
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START:
                dl->PushClipRect(min, max, true);
                break;
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END:
                dl->PopClipRect();
                break;
            case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {
                const auto* fn = static_cast<const WidgetFn*>(cmd->renderData.custom.customData);
                if (fn != nullptr && *fn) {
                    ImGui::SetCursorScreenPos(min);
                    ImGui::PushID((int)cmd->id);
                    (*fn)(min, max);
                    ImGui::PopID();
                    ImGui::SetCursorScreenPos(min);
                    ImGui::Dummy(ImVec2(0.0f, 0.0f));
                }
                break;
            }
            default:
                break;
        }
    }
}
