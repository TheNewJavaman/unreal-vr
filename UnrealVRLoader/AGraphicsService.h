#pragma once

#include "AService.h"
#include "ErrorHandling.h"

namespace UnrealVr {
    class IGraphicsService : public AService {
    public:
        virtual ErrorCode Stop() { return ErrorCode::SUCCESS; }
    };
}
