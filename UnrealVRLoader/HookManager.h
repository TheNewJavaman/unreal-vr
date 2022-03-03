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
        /** Links to libMinHook */
        static bool Init();

        /** My take on Russell.J's MinHook helper. Templates >> casting in the main code */ 
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

        /** Disable/remove all hooks */
        static void Stop();
    };
}
