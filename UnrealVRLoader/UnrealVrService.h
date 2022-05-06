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
        
        std::mutex swapchainPresentMtx;
        std::condition_variable swapchainPresentCv;
        bool swapchainPresentJobActive = false;
    };
}
