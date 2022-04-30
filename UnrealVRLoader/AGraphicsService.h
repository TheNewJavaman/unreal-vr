#pragma once

#include <memory>

#include "AService.h"
#include "DependencyInjection.h"
#include "UnrealVrService.h"

namespace UnrealVr {
    enum class GraphicsApi : uint8_t {
        D3D11 = 0
    };

    struct APresentParams {
        GraphicsApi graphicsApi;
    };

    class AGraphicsService : public AService, public AInitable, public AStoppable {
    public:
        InjectionMap GetInjections() override;
        ErrorCode Init() override;
        ErrorCode Stop() override;

    protected:
        SERVICE(UnrealVrService, unrealVrService)
    };
}
