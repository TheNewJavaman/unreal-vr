#include "Logging.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "DependencyInjection.h"

namespace UnrealVr {
    InjectionMap LoggingService::GetInjections() {
        return {
            INJECTION(PipeServerService, pipeServerService)
        };
    }

    ErrorCode LoggingService::Init() {
        workerThread = std::thread([&] {
            while (true) {
                if (pipeServerService->isInitialized && buffer.length() != 0) {
                    std::lock_guard guard(bufferMtx);
                    pipeServerService->SendString(PipeServerCommand::LOG, buffer);
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
