#pragma once

#include <clay.h>

#include <deque>
#include <mutex>
#include <string>

class ClayUi;

class Toasts {
  public:
    struct Toast {
        std::string text;
        Clay_Color background;
        Clay_Color textColor;
        const char* icon = nullptr;
        Clay_Color iconColor = {};
        float duration = 1.0f;
        double shownAt = -1.0;
    };

    void Post(Toast toast);
    void Error(std::string text);

    void Layout(ClayUi& ui);

  private:
    std::mutex mMutex;
    std::deque<Toast> mQueue;
};
