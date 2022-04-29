#pragma once

#include "APipeService.h"

namespace UnrealVr {
    enum class PipeClientCommand : uint8_t {
        SETTING = 0,
        INITIALIZED = 1,
        STOP = 2
    };

    enum class PipeClientSetting : uint8_t {
        CM_UNITS_SCALE = 0
    };

    class PipeClientService : public APipeService {
    public:
        ErrorCode Init() override;
        ErrorCode Stop() override;
        
        bool settingsInitialized = false;

        struct {
            float cmUnitsScale = 1.f;
        } settings;
    };
}
