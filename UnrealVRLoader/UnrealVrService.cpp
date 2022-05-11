#include "UnrealVrService.h"

#include <thread>

#include "D3D11Service.h"
#include "OpenXrD3D11Service.h"
#include "UnrealEngineService.h"

#define CHECK_INIT(s, n)                                                             \
    {                                                                                \
        const auto e = (s)->Init();                                                  \
        if (e != ErrorCode::Success) {                                               \
            logger->Error(n " init failed; error {:x}", e);                          \
            Stop();                                                                  \
            return ErrorCode::InitFailed;                                            \
        }                                                                            \
        orderedStopFunctions.insert(orderedStopFunctions.begin(), { (s)->Stop, n }); \
    }
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
        CHECK_INIT(threadPoolService, "Thread pool service")
        CHECK_INIT(pipeService, "Pipe service")
        CHECK_INIT(loggingService, "Logging service")
        return ErrorCode::Success;
    }

    void UnrealVrService::OnPipeInitDone() {
        threadPoolService->QueueJob([this] {
            CHECK_INIT(xrService, "XR service")
            CHECK_INIT(graphicsService, "Graphics service")
            CHECK_INIT(engineService, "Engine service")
            return ErrorCode::Success;
        });
    }

    void UnrealVrService::OnEngineTick() {
        std::unique_lock queueLock(engineTickMtx);
        if (engineTickJobActive) {
            engineTickCv.wait(queueLock);
        }
        threadPoolService->QueueJob([this] {
            std::lock_guard jobLock(engineTickMtx);
            engineTickJobActive = true;
            const auto e = engineService->UpdatePose(cpuEye);
            if (e != ErrorCode::Success) {
                logger->Error("Engine pose update failed; error {:x}", e);
            }
        });
    }
    
    void UnrealVrService::OnGraphicsPresent(const std::shared_ptr<APresentParams>& presentParams) {
        std::unique_lock queueLock(graphicsPresentMtx);
        if (graphicsPresentJobActive) {
            graphicsPresentCv.wait(queueLock);
        }
        threadPoolService->QueueJob([this, presentParams] {
            std::lock_guard jobLock(graphicsPresentMtx);
            graphicsPresentJobActive = true;
            const auto e = xrService->SubmitFrame(presentParams, gpuEye);
            if (e != ErrorCode::Success) {
                logger->Error("XR frame submission failed; error {:x}", e);
            }
            graphicsPresentCv.notify_one();
            graphicsPresentJobActive = false;
        });
    }

    void UnrealVrService::OnPipeStop() {
        std::thread workerThread(this->Stop);
    }

    ErrorCode UnrealVrService::Stop() {
        for (const auto& [stop, name] : orderedStopFunctions) {
            const auto e = stop();
            if (e != ErrorCode::Success) {
                logger->Error(name + " stop failed; error {:x}", e);
            }
        }
        return ErrorCode::Success;
    }
}
