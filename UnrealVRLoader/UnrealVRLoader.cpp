#include "UnrealVRLoader.h"

#include <Utilities/Logger.h>

#include "D3D11Manager.h"
#include "HookManager.h"
#include "UE4Manager.h"
#include "VRManager.h"

namespace UnrealVR
{
    void Loader::Init()
    {
        UE4::InitSDK();
        if (!VRManager::Init())
        {
            Log::Error("[UnrealVR] Failed to init VR");
            return;
        }
        if (!HookManager::Init())
        {
            Log::Error("[UnrealVR] Failed to init hooks");
            return;
        }
        D3D11Manager::AddHooks();
        UE4Manager::AddEvents();
    }

    void Loader::Stop()
    {
        VRManager::Stop();
        HookManager::Stop();
    }
}
