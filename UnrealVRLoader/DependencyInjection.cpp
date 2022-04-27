#include "DependencyInjection.h"

#include "D3D11Service.h"
#include "UnrealVrService.h"

namespace UnrealVr {
    void RegisterServices() {
        REGISTER_SERVICE(UnrealVrService)
        REGISTER_SERVICE(D3D11Service)
        REGISTER_SERVICE()
    }
}