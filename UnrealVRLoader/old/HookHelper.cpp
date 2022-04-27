#include "HookHelper.h"

namespace UnrealVR
{
    bool HookHelper::Init()
    {
        if (MH_Initialize() != MH_OK)
        {
            Log::Error("[UnrealVR] Failed to initialize MinHook");
            return false;
        }
        Log::Info("[UnrealVR] Initialized MinHook");
        return true;
    }

    void HookHelper::Stop()
    {
        if (MH_DisableHook(nullptr) != MH_OK)
        {
            Log::Error("[UnrealVR] Failed to disable all hooks");
            return;
        }
        if (MH_Uninitialize() != MH_OK)
        {
            Log::Error("[UnrealVR] Failed to uninitialize MinHook");
            return;
        }
        Log::Info("[UnrealVR] Stopped MinHook");
    }
}
