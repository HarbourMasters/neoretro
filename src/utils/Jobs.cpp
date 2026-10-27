#include "Jobs.h"

#include "ThreadPool.h"

#include <chrono>
#include <exception>
#include <spdlog/spdlog.h>

Jobs& Jobs::Instance() {
    static Jobs sInstance;
    return sInstance;
}

void Jobs::Run(Work work, Completion onComplete) {
    auto result = ThreadPool::Instance().Submit([work = std::move(work)] {
        try {
            work();
        } catch (const std::exception& e) { SPDLOG_ERROR("Background job failed: {}", e.what()); } catch (...) {
            SPDLOG_ERROR("Background job failed with an unknown error");
        }
    });
    mJobs.push_back({ std::move(result), std::move(onComplete) });
}

void Jobs::Update() {
    for (size_t i = 0; i < mJobs.size();) {
        if (mJobs[i].result.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
            i++;
            continue;
        }
        Job job = std::move(mJobs[i]);
        mJobs.erase(mJobs.begin() + (std::ptrdiff_t)i);
        if (job.onComplete) {
            job.onComplete();
        }
    }
}

void Jobs::WaitAll() {
    for (auto& job : mJobs) {
        job.result.wait();
    }
    mJobs.clear();
}
