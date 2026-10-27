#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

class ThreadPool {
  public:
    static ThreadPool& Instance();

    explicit ThreadPool(unsigned threads = std::max(1u, std::thread::hardware_concurrency()));
    ~ThreadPool();
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    size_t Size() const {
        return mWorkers.size();
    }

    template <typename F> auto Submit(F&& fn) -> std::future<std::invoke_result_t<F>> {
        using Result = std::invoke_result_t<F>;
        auto task = std::make_shared<std::packaged_task<Result()>>(std::forward<F>(fn));
        std::future<Result> future = task->get_future();
        Enqueue([task] { (*task)(); });
        return future;
    }

    void ParallelFor(size_t count, const std::function<void(size_t)>& fn);

  private:
    void Enqueue(std::function<void()> task);
    void Work();

    std::vector<std::thread> mWorkers;
    std::queue<std::function<void()>> mTasks;
    std::mutex mMutex;
    std::condition_variable mCondition;
    bool mStopping = false;
};
