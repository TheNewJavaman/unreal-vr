#include "UnrealEngineService.h"

namespace UnrealVr {
    InjectionMap UnrealEngineService::GetInjections() {
        return {
            INJECTION(PipeClientService, pipeClientService)
        };
    }
    
    ErrorCode UnrealEngineService::Init() {
        return ErrorCode::Success;
    }
}
