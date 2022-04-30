#pragma once

#include "AGraphicsService.h"
#include "AService.h"
#include "ErrorHandling.h"

namespace UnrealVr {
    class UnrealVrService : public AService, public AInitable, public AStoppable {
    public:
        ErrorCode Init() override;
        ErrorCode Stop() override;

        ErrorCode OnPipeSettingsInitialized();
        ErrorCode OnGraphicsPresent(APresentParams* presentParams);
    };
}
