#pragma once

#include "AXrService.h"

namespace UnrealVr {
    /**
     * An XR service implementation for OpenXR, a multi-vendor runtime
     */
    class OpenXrD3D11Service : public AXrService {
    public:
        InjectionMap GetInjections() override;
        ErrorCode Init() override;
        ErrorCode Stop() override;
        Directional2D GetPose(Eye eye) override;
        ErrorCode SubmitFrame(Eye eye) override;
    };
}
