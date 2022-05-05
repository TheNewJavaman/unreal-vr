#include "PipeService.h"

namespace UnrealVr {
    InjectionMap PipeService::GetInjections() {
        return {
            INJECTION(UnrealVrService, unrealVrService)
        };
    }

    ErrorCode PipeService::Init() {
        pipe = CreateFile(
            TEXT("\\\\.\\pipe\\UnrealVRLauncher"),
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            nullptr
        );
        if (pipe == INVALID_HANDLE_VALUE) {
            logger->Error("Couldn't open launcher pipe; error {}", GetLastError());
            return ErrorCode::PipeOpen;
        }
        connected = true;
        listenerThread = std::thread(this->ListenerThread);
        return ErrorCode::Success;
    }

    void PipeService::ListenerThread() {
        while (true) {
            CHAR buffer[BUFFER_SIZE];
            DWORD bytesRead = 0;
            OVERLAPPED overlapped;
            ReadFile(pipe, buffer, BUFFER_SIZE * sizeof(CHAR), &bytesRead, &overlapped);
            while (bytesRead == 0) {
                if (GetLastError() != ERROR_IO_PENDING) {
                    logger->Warn("Launcher pipe closed; error {}", GetLastError());
                    return;
                }
                if (shouldStopThread) {
                    return;
                }
                Sleep(20);
            }
            switch (static_cast<PipeCommand>(buffer[0])) {
            case PipeCommand::Setting:
                switch (static_cast<PipeSetting>(buffer[1])) {
                case PipeSetting::CmUnitsScale:
                    memcpy(&settings.cmUnitsScale, buffer + 1, sizeof(float));
                    logger->Info("Set cm units scale to {:.3f}", settings.cmUnitsScale);
                    break;
                }
                break;
            case PipeCommand::Initialized:
                unrealVrService->OnPipeSettingsInitialized();
                break;
            case PipeCommand::Stop:
                unrealVrService->Stop();
                break;
            case PipeCommand::Log:
                break;
            }
        }
    }

    ErrorCode PipeService::Stop() {
        if (!CloseHandle(pipe)) {
            logger->Error("Couldn't close launcher pipe handle; error {}", GetLastError());
            return ErrorCode::PipeClose;
        }
        return ErrorCode::Success;
    }
}
