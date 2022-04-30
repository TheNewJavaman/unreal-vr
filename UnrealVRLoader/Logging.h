#pragma once

#include <chrono>
#include <cstdint>
#include <format>
#include <map>
#include <mutex>
#include <string>
#include <thread>

#include "AService.h"
#include "PipeService.h"

#define LOGGER(T) auto logger = std::make_unique<Logger>(#T);

namespace UnrealVr {
    enum class LogLevel : uint8_t {
        Debug = 0,
        Info = 1,
        Warn = 2,
        Error = 3
    };

    static std::map<LogLevel, std::string> LogLevelStrings = {
        { LogLevel::Debug, "Debug" },
        { LogLevel::Info, "Info " },
        { LogLevel::Warn, "Warn " },
        { LogLevel::Error, "Error" }
    };

    static std::string scopedBuffer;
    static std::mutex scopedBufferMtx;
    
    class LoggingService : public AService, public AInitable {
    public:
        InjectionMap GetInjections() override;
        ErrorCode Init() override;

    private:
        SERVICE(PipeService, pipeService)

        std::string& buffer = scopedBuffer;
        std::mutex& bufferMtx = scopedBufferMtx;
        std::thread workerThread;
    };
    
    class Logger {
    public:
        Logger(std::string source);

        template<typename... Args>
        void Log(LogLevel logLevel, const std::string& format, Args ...args) {
            auto now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
            auto line = std::format("[{:%F %T}] [{}] [{}] " + format + "\n", now, source, logLevel, args...);
            std::lock_guard guard(scopedBufferMtx);
            scopedBuffer += line;
        }

        template<typename... Args>
        void Debug(const std::string& format, Args ...args) {
            Log(LogLevel::Debug, format, args...);
        }
        
        template<typename... Args>
        void Info(const std::string& format, Args ...args) {
            Log(LogLevel::Info, format, args...);
        }

        template<typename... Args>
        void Warn(const std::string& format, Args ...args) {
            Log(LogLevel::Warn, format, args...);
        }
        
        template<typename... Args>
        void Error(const std::string& format, Args ...args) {
            Log(LogLevel::Error, format, args...);
        }

    private:
        std::string source;
    };
}
