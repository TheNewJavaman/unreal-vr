#include "UnrealVrService.h"

#include <thread>

#include "D3D11Service.h"
#include "OpenXrD3D11Service.h"
#include "UnrealEngineService.h"

#define CHECK_INIT(s)                                                             \
    if ((s)->Init() != ErrorCode::Success) {                                      \
        logger->Error(#m " service init failed");                                 \
        Stop();                                                                   \
        return ErrorCode::InitFailed;                                             \
    }                                                                             \
    orderedStopFunctions.insert(orderedStopFunctions.begin(), { (s)->Stop, #s });

namespace UnrealVr {
    void OnAttach() {
        std::thread workerThread([] {
            REGISTER_SERVICE(D3D11Service)
            REGISTER_SERVICE(LoggingService)
            REGISTER_SERVICE(OpenXrD3D11Service)
            REGISTER_SERVICE(PipeService)
            REGISTER_SERVICE(ThreadPoolService)
            REGISTER_SERVICE(UnrealEngineService)
            REGISTER_SERVICE(UnrealVrService)

            INJECT_SERVICE_AS(D3D11Service, AGraphicsService)
            INJECT_SERVICE(LoggingService)
            INJECT_SERVICE_AS(OpenXrD3D11Service, AXrService)
            INJECT_SERVICE(PipeService)
            INJECT_SERVICE(ThreadPoolService)
            INJECT_SERVICE_AS(UnrealEngineService, AEngineService)
            INJECT_SERVICE(UnrealVrService)

            GET_SERVICE(UnrealVrService)->Init();
        });
    }

    void OnDetach() {
        GET_SERVICE(UnrealVrService)->Stop();
    }

    InjectionMap UnrealVrService::GetInjections() {
        return {
            INJECTION(AEngineService, engineService),
            INJECTION(AGraphicsService, graphicsService),
            INJECTION(AXrService, xrService),
            INJECTION(LoggingService, loggingService),
            INJECTION(PipeService, pipeService),
            INJECTION(ThreadPoolService, threadPoolService)
        };
    }

    ErrorCode UnrealVrService::Init() {
        CHECK_INIT(threadPoolService)
        CHECK_INIT(pipeService)
        CHECK_INIT(loggingService)
        return ErrorCode::Success;
    }

    void UnrealVrService::OnPipeInitDone() {
        threadPoolService->QueueJob([this] {
            CHECK_INIT(xrService)
            CHECK_INIT(graphicsService)
            CHECK_INIT(engineService)
            return ErrorCode::Success;
        });
    }

    void UnrealVrService::OnGraphicsPresent(const std::shared_ptr<APresentParams>& presentParams) {
        std::unique_lock queueLock(swapchainPresentMtx);
        swapchainPresentCv.wait(queueLock, [this] {
            return !swapchainPresentJobActive;
        });
        threadPoolService->QueueJob([this] {
            std::lock_guard jobLock(swapchainPresentMtx);
            swapchainPresentJobActive = true;
            
            swapchainPresentCv.notify_one();
            swapchainPresentJobActive = false;
        });
    }

    void UnrealVrService::OnPipeStop() {
        std::thread workerThread(this->Stop);
    }

    ErrorCode UnrealVrService::Stop() {
        for (const auto& [stop, name] : orderedStopFunctions) {
            if (stop() != ErrorCode::Success) {
                logger->Error(name + " service stop failed");
            }
        }
        return ErrorCode::Success;
    }
}
