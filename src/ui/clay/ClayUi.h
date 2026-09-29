#pragma once

#include <clay.h>
#include <imgui.h>

#include <deque>
#include <functional>
#include <string>
#include <string_view>

class ClayUi {
  public:
    using WidgetFn = std::function<void(ImVec2 min, ImVec2 max)>;

    bool Init();
    void Shutdown();

    void BeginLayout();
    void EndLayout();

    Clay_String Str(std::string_view text);
    static Clay_ElementId Id(std::string_view id, uint32_t index = 0);
    Clay_CustomElementConfig Widget(WidgetFn fn);

    bool Hovered() const;
    bool Clicked() const;
    bool Hovered(Clay_ElementId id) const;
    bool Clicked(Clay_ElementId id) const;

    void ToggleDebug();

  private:
    static Clay_Dimensions MeasureText(Clay_StringSlice text, Clay_TextElementConfig* config, void* userData);
    static void HandleError(Clay_ErrorData error);
    void Render(Clay_RenderCommandArray commands);

    void* mArenaMemory = nullptr;
    Clay_Context* mContext = nullptr;
    ImVec2 mOrigin = {};
    bool mHostHovered = false;

    std::deque<std::string> mFrameStrings;
    std::deque<WidgetFn> mFrameWidgets;
};
