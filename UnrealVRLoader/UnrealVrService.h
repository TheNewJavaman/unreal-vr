#pragma once

#include "AEngineService.h"
#include "AGraphicsService.h"
#include "AService.h"
#include "AXrService.h"
#include "DependencyInjection.h"
#include "ErrorHandling.h"
#include "Logger.h"
#include "PipeService.h"

namespace UnrealVr {
    void OnAttach();
    void OnDetach();
    
    /**
     * Manages the UnrealVR's logic at a high level using abstract services
     */
    class UnrealVrService : public AService {
    public:
        InjectionMap GetInjections() override;
        ErrorCode Init() override;
        ErrorCode Stop() override;

        void OnPipeInitDone();
        void OnPipeStop();
        void OnEngineTick();
        void OnGraphicsPresent(const std::shared_ptr<APresentParams>& presentParams);
        
    private:
        LOGGER(UnrealVrService)
        SERVICE(AEngineService, engineService)
        SERVICE(AGraphicsService, graphicsService)
        SERVICE(AXrService, xrService)
        SERVICE(LoggingService, loggingService)
        SERVICE(PipeService, pipeService)
        SERVICE(ThreadPoolService, threadPoolService)

        std::vector<std::pair<std::function<ErrorCode()>, std::string>> orderedStopFunctions;

        std::mutex engineTickMtx;
        std::condition_variable engineTickCv;
        bool engineTickJobActive = false;
        Eye cpuEye = Eye::Right;
        
        std::mutex graphicsPresentMtx;
        std::condition_variable graphicsPresentCv;
        bool graphicsPresentJobActive = false;
        Eye gpuEye = Eye::Right;
    };
}
