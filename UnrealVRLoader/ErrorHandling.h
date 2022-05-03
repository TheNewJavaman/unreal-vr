#pragma once

#include <cstdint>

#define ERROR_VALUE(e) static_cast<uint16_t>(e)

namespace UnrealVr {
    enum class ErrorCategory : uint16_t {
        Generic = 0x0000,
        Pipe = 0x0100
    };

    enum class ErrorCode : uint16_t {
        Success = ERROR_VALUE(ErrorCategory::Generic) | 0x00,
        PipeOpen = ERROR_VALUE(ErrorCategory::Pipe) | 0x00,
        PipeClose = ERROR_VALUE(ErrorCategory::Pipe) | 0x01
    };
}