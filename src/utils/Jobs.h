#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

class Jobs {
  public:
    using Work = std::function<void()>;
    using Completion = std::function<void()>;

    static Jobs& Instance();

    void Run(Work work, Completion onComplete = {});
    void Update();
    void WaitAll();

  private:
    struct Job {
        std::thread thread;
        std::shared_ptr<std::atomic<bool>> done;
        Completion onComplete;
    };

    std::vector<Job> mJobs;
};
