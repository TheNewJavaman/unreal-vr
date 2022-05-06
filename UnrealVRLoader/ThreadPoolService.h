#pragma once

#include <functional>
#include <queue>

#include "AService.h"
#include "Logger.h"

namespace UnrealVr {
    typedef std::function<void()> Job;

    /**
     * Manages multithreading for the mod
     *
     * Thread allocations:
     * 0. Pipe listener
     * 1. Log flush loop
     * 2. Graphics present callback (active)
     * 3. Graphics present callback (queued with mutex lock)
     * 4. Engine tick callback (active)
     * 5. Engine tick callback (queued with mutex lock)
     * 6. Free
     * 7. Free
     */
    class ThreadPoolService : public AService {
    public:
        ErrorCode Init() override;
        ErrorCode Stop() override;

        void QueueJob(const Job& job);
        
    private:
        static constexpr uint32_t THREAD_COUNT = 8;
        
        LOGGER(ThreadingService)
        
        bool shouldStop = false;
        std::vector<std::thread> threadPool;
        std::queue<Job> jobQueue;
        std::mutex jobQueueMtx;
        std::condition_variable jobQueueCv;

        void ThreadLoop();
    };
}
