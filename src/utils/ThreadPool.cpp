#include "ThreadPool.h"

#include <atomic>
#include <exception>

namespace {
struct ParallelForState {
    std::atomic<size_t> next = 0;
    size_t count = 0;
    const std::function<void(size_t)>* fn = nullptr;
    std::atomic<int> active = 0;
    std::mutex mutex;
    std::condition_variable idle;
    std::exception_ptr error;
};

void RunRange(const std::shared_ptr<ParallelForState>& state) {
    for (;;) {
        state->active++;
        const size_t index = state->next++;
        if (index >= state->count) {
            if (--state->active == 0) {
                std::lock_guard lock(state->mutex);
                state->idle.notify_all();
            }
            return;
        }
        try {
            (*state->fn)(index);
        } catch (...) {
            std::lock_guard lock(state->mutex);
            if (!state->error) {
                state->error = std::current_exception();
            }
        }
        if (--state->active == 0) {
            std::lock_guard lock(state->mutex);
            state->idle.notify_all();
        }
    }
}
} // namespace

ThreadPool& ThreadPool::Instance() {
    static ThreadPool sInstance;
    return sInstance;
}

ThreadPool::ThreadPool(unsigned threads) {
    mWorkers.reserve(threads);
    for (unsigned i = 0; i < threads; i++) {
        mWorkers.emplace_back([this] { Work(); });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard lock(mMutex);
        mStopping = true;
    }
    mCondition.notify_all();
    for (auto& worker : mWorkers) {
        worker.join();
    }
}

void ThreadPool::Enqueue(std::function<void()> task) {
    {
        std::lock_guard lock(mMutex);
        mTasks.push(std::move(task));
    }
    mCondition.notify_one();
}

void ThreadPool::Work() {
    for (;;) {
        std::function<void()> task;
        {
            std::unique_lock lock(mMutex);
            mCondition.wait(lock, [this] { return mStopping || !mTasks.empty(); });
            if (mTasks.empty()) {
                return;
            }
            task = std::move(mTasks.front());
            mTasks.pop();
        }
        task();
    }
}

void ThreadPool::ParallelFor(size_t count, const std::function<void(size_t)>& fn) {
    if (count == 0) {
        return;
    }
    auto state = std::make_shared<ParallelForState>();
    state->count = count;
    state->fn = &fn;

    const size_t helpers = std::min(Size(), count - 1);
    for (size_t i = 0; i < helpers; i++) {
        Enqueue([state] { RunRange(state); });
    }
    RunRange(state);

    std::unique_lock lock(state->mutex);
    state->idle.wait(lock, [&] { return state->active.load() == 0; });
    if (state->error) {
        std::rethrow_exception(state->error);
    }
}
