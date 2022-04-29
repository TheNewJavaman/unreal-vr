#pragma once

#include <vector>

#include "APipeService.h"

namespace UnrealVr {
    enum class PipeServerCommand : uint8_t {
        LOG = 0
    };

    class PipeServerService : public APipeService {
    public:
        ErrorCode Init() override;
        ErrorCode Stop() override;
        ErrorCode SendMessage(PipeServerCommand command, std::vector<uint8_t> buffer);
    };
}

