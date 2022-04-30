#include "UnrealVrService.h"

namespace UnrealVr {
    ErrorCode UnrealVrService::Init() {
        return ErrorCode::Success;
    }

    ErrorCode UnrealVrService::Stop() {
        return ErrorCode::Success;
    }

    ErrorCode UnrealVrService::OnGraphicsPresent(APresentParams* presentParams) {
        return ErrorCode::Success;
    }
}
