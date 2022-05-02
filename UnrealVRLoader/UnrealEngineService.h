#pragma once

#include "AEngineService.h"
#include "Logging.h"
#include "PipeService.h"

namespace UnrealVr {
    /**
     * An engine service implementation for Unreal Engine 4/5
     */
    class UnrealEngineService : public AEngineService {
    public:
        InjectionMap GetInjections() override;
        ErrorCode Init() override;

    private:
        LOGGER(UnrealEngineService)
        SERVICE(PipeService, pipeService)
    };
}
