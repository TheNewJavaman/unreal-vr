#include "UnrealVrLoader.h"

#include "D3D11Service.h"
#include "DependencyInjection.h"
#include "OpenXrService.h"
#include "PipeService.h"
#include "ThreadPoolService.h"
#include "UnrealEngineService.h"
#include "UnrealVrService.h"

namespace UnrealVr {
    void OnAttach() {
        std::thread workerThread([] {
            REGISTER_SERVICE(D3D11Service)
            REGISTER_SERVICE(OpenXrService)
            REGISTER_SERVICE(PipeService)
            REGISTER_SERVICE(ThreadPoolService)
            REGISTER_SERVICE(UnrealEngineService)
            REGISTER_SERVICE(UnrealVrService)

            INJECT_SERVICE_AS(D3D11Service, AGraphicsService)
            INJECT_SERVICE_AS(OpenXrService, AXrService)
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
}
