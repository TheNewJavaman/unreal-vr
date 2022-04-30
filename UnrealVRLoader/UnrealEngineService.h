#pragma once

#include "AEngineService.h"
#include "Logging.h"
#include "PipeService.h"

namespace UnrealVr {
    class UnrealEngineService : public AEngineService {
    public:
        InjectionMap GetInjections() override;
        ErrorCode Init() override;

    private:
        SERVICE(PipeService, pipeService)
    };
}
