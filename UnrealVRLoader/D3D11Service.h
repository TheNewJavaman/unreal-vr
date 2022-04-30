#pragma once

#include "AGraphicsService.h"

namespace UnrealVr {
    struct D3D11PresentParams : APresentParams {};

    class D3D11Service : public AGraphicsService {
    public:
        ErrorCode Init() override;
        ErrorCode Stop() override;
    };
}
