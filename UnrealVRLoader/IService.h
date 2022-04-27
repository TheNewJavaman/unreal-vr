#pragma once

#include "ErrorHandling.h"

namespace UnrealVr {
    class IService {
    protected:
        ~IService() = default;
        
    public:
        virtual ErrorCode Init();
        virtual ErrorCode Stop();
    };
}