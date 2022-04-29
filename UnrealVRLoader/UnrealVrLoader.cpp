#include "UnrealVrLoader.h"

#include "D3D11Service.h"
#include "DependencyInjection.h"
#include "OpenXrService.h"
#include "UnrealEngineService.h"

namespace UnrealVr {
    void OnAttach() { CreateThread(nullptr, 0, OnAttachThread, nullptr, 0, nullptr); }

    DWORD OnAttachThread(LPVOID) {
        SERVICE(D3D11Service)
        SERVICE(OpenXrService)
        SERVICE(PipeService)
        SERVICE(UnrealEngineService)
        INJECT(D3D11Service)
        INJECT(OpenXrService)
        INJECT(PipeService)
        INJECT(UnrealEngineService)
        return 0;
    }

    void OnDetach() {
        GET_SERVICE(D3D11Service)->Stop();
        DeleteServices();
    }
}
