#include "ThreadPoolService.h"

namespace UnrealVr {
    ErrorCode ThreadPoolService::Init() {
        logger->Info("Initializing thread pool");
        threadPool.resize(THREAD_COUNT);
        for (uint32_t i = 0; i < THREAD_COUNT; i++) {
            threadPool.at(i) = std::thread(ThreadLoop);
        }
        logger->Info("Started thread pool with thread count {}", THREAD_COUNT);
        return ErrorCode::Success;
    }

    void ThreadPoolService::ThreadLoop() {
        while (true) {
            Job job;
            {
                std::unique_lock lock(jobQueueMtx);
                jobQueueCv.wait(lock, [this] {
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

    ErrorCode ThreadPoolService::Stop() {
        logger->Info("Stopping thread pool");
        {
            std::unique_lock lock(jobQueueMtx);
            shouldStop = true;
        }
        jobQueueCv.notify_all();
        logger->Info("Notified threads to exit");
        for (auto& activeThread : threadPool) {
            activeThread.join();
        }
        logger->Info("Joined all threads");
        return ErrorCode::Success;
    }

    void ThreadPoolService::QueueJob(const Job& job) {
        {
            std::unique_lock lock(jobQueueMtx);
            jobQueue.push(job);
        }
        jobQueueCv.notify_one();
    }
}
