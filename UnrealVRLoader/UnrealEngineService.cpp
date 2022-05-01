#include "UnrealEngineService.h"

namespace UnrealVr {
    InjectionMap UnrealEngineService::GetInjections() {
        return {
            INJECTION(PipeService, pipeService)
        };
    }
    
    ErrorCode UnrealEngineService::Init() {
        auto processEvent
        return ErrorCode::Success;
    }
}
