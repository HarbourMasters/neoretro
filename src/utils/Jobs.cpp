#include "Jobs.h"

#include <atomic>
#include <exception>
#include <spdlog/spdlog.h>

Jobs& Jobs::Instance() {
    static Jobs sInstance;
    return sInstance;
}

void Jobs::Run(Work work, Completion onComplete) {
    auto done = std::make_shared<std::atomic<bool>>(false);
    std::thread thread([work = std::move(work), done] {
        try {
            work();
        } catch (const std::exception& e) { SPDLOG_ERROR("Background job failed: {}", e.what()); } catch (...) {
            SPDLOG_ERROR("Background job failed with an unknown error");
        }
        done->store(true);
    });
    mJobs.push_back({ std::move(thread), std::move(done), std::move(onComplete) });
}

void Jobs::Update() {
    for (size_t i = 0; i < mJobs.size();) {
        if (!mJobs[i].done->load()) {
            i++;
            continue;
        }
        Job job = std::move(mJobs[i]);
        mJobs.erase(mJobs.begin() + (std::ptrdiff_t)i);
        job.thread.join();
        if (job.onComplete) {
            job.onComplete();
        }
    }
}

void Jobs::WaitAll() {
    for (auto& job : mJobs) {
        job.thread.join();
    }
    mJobs.clear();
}
