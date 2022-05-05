#include "PipeService.h"

namespace UnrealVr {
    InjectionMap PipeService::GetInjections() {
        return {
            INJECTION(ThreadPoolService, threadPoolService),
            INJECTION(UnrealVrService, unrealVrService)
        };
    }

    ErrorCode PipeService::Init() {
        logger->Info("Opening launcher pipe");
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
            return ErrorCode::PipeOpenFailed;
        }
        logger->Info("Successfully opened launcher pipe");
        connected = true;
        threadPoolService->QueueJob(this->ListenerJob);
        return ErrorCode::Success;
    }

    void PipeService::ListenerJob() {
        while (true) {
            CHAR buffer[BUFFER_SIZE];
            OVERLAPPED overlapped;
            if (!ReadFile(pipe, buffer, BUFFER_SIZE * sizeof(CHAR), nullptr, &overlapped)) {
                while (true) {
                    if (shouldStop) {
                        logger->Info("Exiting listener job");
                        return;
                    }
                    GetOverlappedResult(pipe, &overlapped, nullptr, false);
                    const auto lastError = GetLastError();
                    if (lastError == ERROR_SUCCESS) {
                        break;
                    }
                    if (lastError != ERROR_IO_PENDING) {
                        logger->Warn("Launcher pipe closed while reading; error {}", lastError);
                        return;
                    }
                    Sleep(20);
                }
            }
            switch (static_cast<PipeCommand>(buffer[0])) {
            case PipeCommand::Setting:
                logger->Info("Received Setting command");
                switch (static_cast<PipeSetting>(buffer[1])) {
                case PipeSetting::CmUnitsScale:
                    memcpy(&settings.cmUnitsScale, buffer + 1, sizeof(float));
                    logger->Info("Set cm units scale to {:.3f}", settings.cmUnitsScale);
                    break;
                }
                break;
            case PipeCommand::Initialized:
                logger->Info("Received Initialized command");
                unrealVrService->OnPipeSettingsInitialized();
                break;
            case PipeCommand::Stop:
                logger->Info("Received Stop command");
                unrealVrService->Stop();
                break;
            case PipeCommand::Log:
                break;
            }
        }
    }

    ErrorCode PipeService::SendData(PipeCommand command, std::vector<char> data) {
        data.insert(data.begin(), static_cast<char>(command));
        OVERLAPPED overlapped;
        if (!WriteFile(pipe, data.data(), static_cast<uint32_t>(data.size()), nullptr, &overlapped)) {
            while (true) {
                if (shouldStop) {
                    logger->Info("Exiting writing job");
                    return ErrorCode::Stopping;
                }
                GetOverlappedResult(pipe, &overlapped, nullptr, false);
                const auto lastError = GetLastError();
                if (lastError == ERROR_SUCCESS) {
                    break;
                }
                if (lastError != ERROR_IO_PENDING) {
                    logger->Warn("Launcher pipe closed while writing; error {}", lastError);
                    return ErrorCode::PipeClosed;
                }
                Sleep(20);
            }
        }
        return ErrorCode::Success;
    }

    ErrorCode PipeService::Stop() {
        shouldStop = true;
        logger->Info("Closing launcher pipe");
        if (!CloseHandle(pipe)) {
            logger->Error("Couldn't close launcher pipe handle; error {}", GetLastError());
            return ErrorCode::PipeCloseFailed;
        }
        logger->Info("Successfully closed launcher pipe");
        return ErrorCode::Success;
    }
}
