#include "LoggingService.h"

namespace UnrealVr {
    InjectionMap LoggingService::GetInjections() {
        return {
            INJECTION(PipeService, pipeService),
            INJECTION(ThreadPoolService, threadPoolService)
        };
    }

    ErrorCode LoggingService::Init() {
        threadPoolService->QueueJob(this->FlushJob);
        return ErrorCode::Success;
    }

    void LoggingService::FlushJob() {
        while (true) {
            std::unique_lock lock(bufferMtx);
            bufferCv.wait(lock, [this] {
                return buffer.length() != 0 && pipeService->connected || shouldStop;
            });
            if (shouldStop) {
                logger->Info("Stopping flush job");
                return;
            }
            const auto e = pipeService->SendData(PipeCommand::Log, std::vector(buffer.begin(), buffer.end()));
            if (e == ErrorCode::PipeClosed) {
                logger->Info("Stopping flush job because the pipe was closed");
                return;
            }
            buffer = "";
        }
    }

    ErrorCode LoggingService::Stop() {
        logger->Info("Stopping flush thread");
        shouldStop = true;
        return ErrorCode::Success;
    }
}
