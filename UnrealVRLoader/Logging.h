#pragma once

#include <chrono>
#include <cstdint>
#include <format>
#include <iostream>
#include <map>
#include <string>

#define LOGGER(T) auto logger = new Logger(#T);

namespace UnrealVr {
    enum class LogLevel : uint8_t {
        DEBUG = 0,
        INFO = 1,
        WARN = 2,
        ERROR = 3
    };

    static std::map<LogLevel, std::string> LogLevelStrings = {
        { LogLevel::DEBUG, "DEBUG" },
        { LogLevel::INFO, "INFO " },
        { LogLevel::WARN, "WARN " },
        { LogLevel::ERROR, "ERROR" }
    };

    class Logger {
    public:
        Logger(std::string source);

        template<typename... Args>
        void Log(LogLevel logLevel, const std::string& format, Args ...args) {
            auto now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
            auto line = std::format("[{:%F %T}] [{}] [{}] " + format + "\n", now, source, logLevel, args...);
        }

        template<typename... Args>
        void Debug(const std::string& format, Args ...args) {
            Log(LogLevel::DEBUG, format, args...);
        }
        
        template<typename... Args>
        void Info(const std::string& format, Args ...args) {
            Log(LogLevel::INFO, format, args...);
        }

        template<typename... Args>
        void Warn(const std::string& format, Args ...args) {
            Log(LogLevel::WARN, format, args...);
        }
        
        template<typename... Args>
        void Error(const std::string& format, Args ...args) {
            Log(LogLevel::ERROR, format, args...);
        }

    private:
        std::string source;
    };
}
