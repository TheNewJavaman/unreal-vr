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
        CreateThread(nullptr, 0, InitThread, nullptr, 0, nullptr);
    }

    DWORD __stdcall Loader::InitThread(LPVOID)
    {
        UE4::InitSDK();
        if (!VRManager::Init())
        {
            Log::Error("[UnrealVR] Failed to init VR");
            return NULL;
        }
        if (!HookManager::Init())
        {
            Log::Error("[UnrealVR] Failed to init hooks");
            return NULL;
        }
        UE4Manager::AddEvents();
        D3D11Manager::AddHooks();
        return NULL;
    }
    
    void Loader::Stop()
    {
        D3D11Manager::Stop();
        HookManager::Stop();
        VRManager::Stop();
    }
}
