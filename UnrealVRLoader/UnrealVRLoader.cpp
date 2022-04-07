#include "UnrealVRLoader.h"

#include <Utilities/Logger.h>

#include "D3D11Manager.h"
#include "HookManager.h"
#include "PipeClient.h"
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
        HMODULE umlDLL = GetModuleHandle(L"UnrealEngineModLoader.dll");
        while (!umlDLL)
        {
            Sleep(100);
            umlDLL = GetModuleHandle(L"UnrealEngineModLoader.dll");
        }
        while (!IsGameInfoLoaded())
            Sleep(100);
        UE4::InitSDK();
        if (!VRManager::Init())
        {
            Log::Error("[UnrealVR] Failed to init VR");
            return NULL;
        }
        if (!PipeClient::Init())
        {
            Log::Error("[UnrealVR] Failed to init pipe client");
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
        PipeClient::Stop();
        VRManager::Stop();
    }
}
