#pragma once

#include <format>
#include <MinHook.h>
#include <string>
#include <Utilities/Logger.h>

namespace UnrealVR
{
    class HookManager
    {
    public:
        static bool Init();

        template <typename T>
        static bool Add(T* pTarget, T* pDetour, T** ppOriginal, const std::string displayName)
        {
            if (MH_CreateHook(reinterpret_cast<LPVOID>(reinterpret_cast<DWORD64>(pTarget)),
                              reinterpret_cast<LPVOID>(pDetour),
                              reinterpret_cast<LPVOID*>(ppOriginal)) != MH_OK)
            {
                Log::Error(std::format("[UnrealVR] Failed to create hook ({})", displayName) + "");
                return false;
            }
            if (MH_EnableHook(reinterpret_cast<LPVOID>(reinterpret_cast<DWORD64>(pTarget))) != MH_OK)
            {
                Log::Error(std::format("[UnrealVR] Failed to enable hook ({})", displayName) + "");
                return false;
            }
            Log::Info(std::format("[UnrealVR] Added hook ({})", displayName) + "");
            return true;
        }

        static void Stop();
    };
}
