#pragma once

#include "AGraphicsService.h"

namespace UnrealVr {
    struct D3D11PresentParams : APresentParams {};

    /**
     * A graphics service implementation for DirectX 11's 3D renderer, Direct3D 11
     */
    class D3D11Service : public AGraphicsService {
    public:
        ErrorCode Init() override;
        ErrorCode Stop() override;
    };
}
