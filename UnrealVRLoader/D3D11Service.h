#pragma once

#include "IGraphicsService.h"

namespace UnrealVr {
    class D3D11Service final : public IGraphicsService {
    public:
        ErrorCode Init() override;
        ErrorCode Stop() override;
    };
}