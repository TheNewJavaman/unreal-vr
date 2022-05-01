#include "Logging.h"

#include "DependencyInjection.h"

namespace UnrealVr {
    InjectionMap LoggingService::GetInjections() {
        return {
            INJECTION(PipeService, pipeService)
        };
    }

    ErrorCode LoggingService::Init() {
        workerThread = std::thread([&] {
            while (true) {
                if (pipeService->connected && buffer.length() != 0) {
                    std::lock_guard guard(bufferMtx);
                    pipeService->SendData(PipeCommand::Log, std::vector(buffer.begin(), buffer.end()));
                    buffer = "";
                }
                Sleep(250);
            }
        });
        return ErrorCode::Success;
    }

    Logger::Logger(std::string source) {
        if (source.length() > 24) {
            this->source = source.substr(0, 21) + "...";
        } else {
            this->source = std::format("{: >24}", source);
        }
    }
}
