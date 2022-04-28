#include "UnrealVrService.h"

#include "DependencyInjection.h"

namespace UnrealVr {
    ErrorCode UnrealVrService::Init() {
        CreateThread(nullptr, 0, InitThread, nullptr, 0, nullptr);
        return ErrorCode::SUCCESS;
    }

    DWORD WINAPI UnrealVrService::InitThread(LPVOID) {
        RegisterServices();
        return 0;
    }

    ErrorCode UnrealVrService::Stop() {
        return ErrorCode::SUCCESS;
    }
}