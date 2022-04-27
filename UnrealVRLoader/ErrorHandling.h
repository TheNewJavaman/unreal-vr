#pragma once

#include <cstdint>
#include <map>
#include <string>

namespace UnrealVr {
    enum class ErrorCode : uint16_t {
        SUCCESS = 0,
        GENERIC = 1
    };

    inline std::map<ErrorCode, std::string> ErrorString = {
        { ErrorCode::SUCCESS, "Success" },
        { ErrorCode::GENERIC, "Generic error" }
    };
}