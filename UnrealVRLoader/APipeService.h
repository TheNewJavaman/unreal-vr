#pragma once

#include "AService.h"
#include "ErrorHandling.h"

namespace UnrealVr {
    class APipeService : public AService {
    protected:
        static constexpr int BUFFER_SIZE = 1024;

    public:
        virtual ErrorCode Init() { return ErrorCode::SUCCESS; }
        virtual ErrorCode Stop() { return ErrorCode::SUCCESS; }
    };
}
