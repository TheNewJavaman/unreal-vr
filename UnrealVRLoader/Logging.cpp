#include "Logging.h"

#include "DependencyInjection.h"

namespace UnrealVr {
    Logger::Logger(std::string source) {
        if (source.length() > 24) {
            this->source = source.substr(0, 21) + "...";
        } else {
            this->source = std::format("{: >24}", source);
        }
    }

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

    void LoggingService::FlushJob() const {
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
                logger->Info("Stopping flush job due because the pipe was closed");
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
