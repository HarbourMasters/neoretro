#pragma once

struct AppContext;

class Screen {
  public:
    virtual ~Screen() = default;

    virtual void OnEnter(AppContext& ctx) {
    }
    virtual void OnExit(AppContext& ctx) {
    }

    virtual void Layout(AppContext& ctx) = 0;
    virtual void DrawImGui(AppContext& ctx) {
    }
};
