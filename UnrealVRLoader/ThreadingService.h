#pragma once

#include <functional>
#include <queue>

#include "AService.h"
#include "Logging.h"

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
    class ThreadingService : public AService, public AInitable, public AStoppable {
    public:
        ErrorCode Init() override;
        ErrorCode Stop() override;

        void QueueJob(const Job& job);
        
    private:
        void ThreadLoop();
        
        LOGGER(ThreadingService)

        static constexpr uint32_t THREAD_COUNT = 8;
        
        bool shouldStop = false;
        std::mutex mtx;
        std::condition_variable cv;
        std::vector<std::thread> threadPool;
        std::queue<Job> jobQueue;
    };
}
