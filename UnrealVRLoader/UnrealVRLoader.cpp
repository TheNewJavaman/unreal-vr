#include "UnrealVRLoader.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <processthreadsapi.h>
#include <tlHelp32.h>
#include <Utilities/Logger.h>

#include "D3D11Manager.h"
#include "HookManager.h"
#include "VRManager.h"

namespace UnrealVR
{
    void Loader::Init()
    {
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
        if (!ResumeGame())
        {
            Log::Error("[UnrealVR] Failed to resume game");
        }
    }

    bool Loader::ResumeGame()
    {
        auto hThreadSnap = INVALID_HANDLE_VALUE;
        THREADENTRY32 te32;
        hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (hThreadSnap == INVALID_HANDLE_VALUE)
        {
            Log::Error("[UnrealVR] Failed to create thread snap");
            return false;
        }
        te32.dwSize = sizeof(THREADENTRY32);
        if (!Thread32First(hThreadSnap, &te32))
        {
            Log::Error("[UnrealVR] Failed to find first game process thread");
            if (!CloseHandle(hThreadSnap))
            {
                Log::Error("[UnrealVR] Failed to close thread snap handle");
                return false;
            }
            return false;
        }
        do
        {
            if (te32.th32OwnerProcessID == GetCurrentProcessId() && te32.th32ThreadID != GetCurrentThreadId())
            {
                const HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                if (ResumeThread(hThread) == -1)
                {
                    Log::Error("[UnrealVR] Failed to resume a thread");
                }
                if (!CloseHandle(hThread))
                {
                    Log::Error("[UnrealVR] Failed to close a thread handle");
                }
            }
        }
        while (Thread32Next(hThreadSnap, &te32));
        if (!CloseHandle(hThreadSnap))
        {
            Log::Error("[UnrealVR] Failed to close thread snap handle");
            return false;
        }
        return true;
    }

    void Loader::Stop()
    {
        VRManager::Stop();
        HookManager::Stop();
    }
}
