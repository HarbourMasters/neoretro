#pragma once

#include <functional>
#include <future>
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
        std::future<void> result;
        Completion onComplete;
    };

    std::vector<Job> mJobs;
};
