#pragma once

#include "AEngineService.h"
#include "Logging.h"
#include "PipeService.h"

namespace UnrealVr {
    class UnrealEngineService : public IEngineService {
    public:
        void RegisterInjections() override;
        ErrorCode Init() override;
        
    private:
        LOGGER(UnrealEngineService)
        PipeService* pipeService = nullptr;
    };
}
