#pragma once

#include <cstdint>
#include <string>

namespace UnrealVr {
    enum class LogLevel : uint8_t {
        DEBUG = 0,
        INFO = 1,
        WARN = 2,
        ERROR = 3
    };

    class Logger {
    public:
        Logger(std::string source);

        void Log(LogLevel logLevel, std::string message);

        void Debug(std::string message);
        void Info(std::string message);
        void Warn(std::string message);
        void Error(std::string message);
    };
}