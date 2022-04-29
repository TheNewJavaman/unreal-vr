#include "UnrealEngineService.h"

#include "DependencyInjection.h"

namespace UnrealVr {
    void UnrealEngineService::RegisterInjections() {
        INJECTION(PipeService, pipeService)
    }

    ErrorCode UnrealEngineService::Init() {
        return ErrorCode::SUCCESS;
    }
}
