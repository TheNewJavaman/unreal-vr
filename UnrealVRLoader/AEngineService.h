#pragma once

#include "AService.h"
#include "ErrorHandling.h"

namespace UnrealVr {
    class IEngineService : public AService {
        virtual ErrorCode Init() { return ErrorCode::SUCCESS; }
    };
}
