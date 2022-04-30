#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <memory>
#include <thread>

#include "Logging.h"
#include "UnrealVrService.h"

namespace UnrealVr {
    enum class PipeCommand : uint8_t {
        Setting = 0,
        Initialized = 1,
        Stop = 2,
        Log = 3
    };

    enum class PipeSetting : uint8_t {
        CmUnitsScale = 0
    };

    class PipeService : public AService, AInitable, AStoppable {
    public:
        InjectionMap GetInjections() override;
        ErrorCode Init() override;
        ErrorCode Stop() override;

        bool settingsInitialized = false;

        struct {
            float cmUnitsScale = 1.f;
        } settings;

    private:
        static constexpr int BUFFER_SIZE = 1024;
        
        LOGGER(PipeClientService)
        SERVICE(UnrealVrService, unrealVrService)

        HANDLE pipe = nullptr;
        std::thread listenerThread;
        bool shouldStopThread = false;
        
        void ListenerThread();
    };
}
