#pragma once

#include "IService.h"

namespace UnrealVr {
    class IEngineService : public IService {
    protected:
        ~IEngineService() = default;
    };
}