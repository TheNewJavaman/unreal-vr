#pragma once

#include "AGraphicsService.h"
#include "AService.h"
#include "ErrorHandling.h"

namespace UnrealVr {
    /**
     * Manages the UnrealVR's logic at a high level using abstract services
     */
    class UnrealVrService : public AService, public AInitable, public AStoppable {
    public:
        ErrorCode Init() override;
        ErrorCode Stop() override;

        ErrorCode OnPipeSettingsInitialized();
        ErrorCode OnGraphicsPresent(APresentParams* presentParams);
    };
}
