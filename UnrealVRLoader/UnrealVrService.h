#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "IService.h"

namespace UnrealVr {
    class UnrealVrService final : public IService {
    public:
        ErrorCode Init() override;
        ErrorCode Stop() override;

    private:
        DWORD WINAPI InitThread(LPVOID);
    };
}