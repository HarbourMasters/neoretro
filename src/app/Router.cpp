#include "Router.h"
#include "AppContext.h"

#include <spdlog/spdlog.h>

void Router::Register(const std::string& route, Factory factory) {
    mRoutes[route] = std::move(factory);
}

void Router::Push(const std::string& route) {
    mPending.push_back({ Action::Push, route });
}

void Router::Pop() {
    mPending.push_back({ Action::Pop, {} });
}

void Router::PopUntil(const std::string& route) {
    mPending.push_back({ Action::PopUntil, route });
}

bool Router::CanPop() const {
    return mStack.size() > 1;
}

Screen* Router::Current() const {
    return mStack.empty() ? nullptr : mStack.back().screen.get();
}

const std::string& Router::CurrentRoute() const {
    static const std::string empty;
    return mStack.empty() ? empty : mStack.back().route;
}

void Router::Apply(AppContext& ctx) {
    auto pending = std::move(mPending);
    mPending.clear();

    for (auto& request : pending) {
        if (request.action == Action::Pop) {
            if (!CanPop()) {
                continue;
            }
            mStack.back().screen->OnExit(ctx);
            mStack.pop_back();
            SPDLOG_DEBUG("Router: pop -> {}", CurrentRoute());
            continue;
        }
        if (request.action == Action::PopUntil) {
            while (CanPop() && CurrentRoute() != request.route) {
                mStack.back().screen->OnExit(ctx);
                mStack.pop_back();
            }
            SPDLOG_DEBUG("Router: popUntil {} -> {}", request.route, CurrentRoute());
            continue;
        }

        const auto it = mRoutes.find(request.route);
        if (it == mRoutes.end()) {
            SPDLOG_ERROR("Router: unknown route {}", request.route);
            continue;
        }
        mStack.push_back({ request.route, it->second() });
        mStack.back().screen->OnEnter(ctx);
        SPDLOG_DEBUG("Router: push {}", request.route);
    }
}
