#pragma once

#include <memory>

#include "AService.h"
#include "UnrealVrService.h"

namespace UnrealVr {
    struct APresentParams {};

    /**
     * Defines required functionality for graphics APIs
     */
    class AGraphicsService : public AService {
    public:
        virtual ErrorCode ConvertFrame(
            std::shared_ptr<APresentParams> inPresentParams,
            std::shared_ptr<APresentParams> outPresentParams
        );
    };
}
