#include "UnrealVRLoader.h"

#include <Utilities/Logger.h>

#include "D3D11Service.h"
#include "HookHelper.h"
#include "PipeClientService.h"
#include "UE4Service.h"
#include "OpenXRService.h"

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
        if (!OpenXRService::BeginInit())
        {
            Log::Error("[UnrealVR] Failed to init VR");
            return NULL;
        }
        if (!PipeClientService::Init())
        {
            Log::Error("[UnrealVR] Failed to init pipe client");
            return NULL;
        }
        if (!HookHelper::Init())
        {
            Log::Error("[UnrealVR] Failed to init hooks");
            return NULL;
        }
        UE4Service::AddEvents();
        D3D11Service::AddHooks();
        return NULL;
    }
    
    void Loader::Stop()
    {
        D3D11Service::Stop();
        HookHelper::Stop();
        PipeClientService::Stop();
        OpenXRService::Stop();
    }
}
