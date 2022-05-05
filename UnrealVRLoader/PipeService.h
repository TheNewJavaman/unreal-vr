#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <memory>
#include <thread>

#include "Logging.h"
#include "ThreadPoolService.h"
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

    struct PipeSettings {
        float cmUnitsScale = 1.f;
    };

    /**
     * Facilitates communication with the app UI for realtime settings changes, logging, and more
     */
    class PipeService : public AService, AInitable, AStoppable {
    public:
        bool connected = false;
        PipeSettings settings = {};

        InjectionMap GetInjections() override;
        ErrorCode Init() override;
        ErrorCode Stop() override;
        
        ErrorCode SendData(PipeCommand command, std::vector<char> data);

    private:
        static constexpr int BUFFER_SIZE = 1024;

        LOGGER(PipeService)
        SERVICE(ThreadPoolService, threadPoolService)
        SERVICE(UnrealVrService, unrealVrService)

        HANDLE pipe = nullptr;
        bool shouldStop = false;

        void ListenerJob();
    };
}
