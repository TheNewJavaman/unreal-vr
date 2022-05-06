#pragma once

#include "AService.h"
#include "DependencyInjection.h"
#include "Logger.h"
#include "PipeService.h"

namespace UnrealVr {
    /**
     * Automatically flushes the log buffer to the app UI
     */
    class LoggingService : public AService {
    public:
        static inline std::string buffer;
        static inline std::mutex bufferMtx;
        static inline std::condition_variable bufferCv;
        
        InjectionMap GetInjections() override;
        ErrorCode Init() override;
        ErrorCode Stop() override;

    private:
        LOGGER(LoggingService)
        SERVICE(PipeService, pipeService)
        SERVICE(ThreadPoolService, threadPoolService)
        
        bool shouldStop = false;

        void FlushJob();
    };
}
