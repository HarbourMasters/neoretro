#pragma once

#include "Screen.h"

struct AppContext;

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class Router {
  public:
    using Factory = std::function<std::unique_ptr<Screen>()>;

    void Register(const std::string& route, Factory factory);

    void Push(const std::string& route);
    void Pop();
    void PopUntil(const std::string& route);
    bool CanPop() const;

    Screen* Current() const;
    const std::string& CurrentRoute() const;

    void Apply(AppContext& ctx);

  private:
    struct Entry {
        std::string route;
        std::unique_ptr<Screen> screen;
    };
    enum class Action { Push, Pop, PopUntil };
    struct Request {
        Action action;
        std::string route;
    };

    std::unordered_map<std::string, Factory> mRoutes;
    std::vector<Entry> mStack;
    std::vector<Request> mPending;
};

void RegisterRoutes(Router& router);
