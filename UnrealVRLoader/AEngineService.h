#pragma once

#include "AService.h"
#include "AXrService.h"

namespace UnrealVr {
    /**
     * Defines required functionality for game engines
     */
    class AEngineService : public AService {
    public:
        virtual ErrorCode UpdatePose(Eye eye);
    };
}
