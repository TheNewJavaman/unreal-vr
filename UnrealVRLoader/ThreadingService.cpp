#include "ThreadingService.h"

namespace UnrealVr {
    ErrorCode ThreadingService::Init() {
        logger->Info("Initializing thread pool...");
        threadPool.resize(THREAD_COUNT);
        for (uint32_t i = 0; i < THREAD_COUNT; i++) {
            threadPool.at(i) = std::thread(ThreadLoop);
        }
        logger->Info("Started thread pool with thread count {}", THREAD_COUNT);
        return ErrorCode::Success;
    }

    void ThreadingService::ThreadLoop() {
        while (true) {
            Job job;
            {
                std::unique_lock lock(mtx);
                cv.wait(lock, [this] {
                    return !jobQueue.empty() || shouldStop;
                });
                if (shouldStop) {
                    return;
                }
                job = jobQueue.front();
                jobQueue.pop();
            }
            job();
        }
    }

    ErrorCode ThreadingService::Stop() {
        logger->Info("Stopping thread pool...");
        {
            std::unique_lock lock(mtx);
            shouldStop = true;
        }
        cv.notify_all();
        logger->Info("Notified threads to exit");
        for (auto& activeThread : threadPool) {
            activeThread.join();
        }
        logger->Info("Joined all threads");
        return ErrorCode::Success;
    }

    void ThreadingService::QueueJob(const Job& job) {
        {
            std::unique_lock lock(mtx);
            jobQueue.push(job);
        }
        cv.notify_one();
    }
}
