#include "AGraphicsService.h"

namespace UnrealVr {
    InjectionMap AGraphicsService::GetInjections() {
        return {
            INJECTION(UnrealVrService, unrealVrService)
        };
    }
}