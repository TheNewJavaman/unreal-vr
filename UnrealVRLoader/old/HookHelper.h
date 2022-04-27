#pragma once

#include <format>
#include <MinHook.h>
#include <string>
#include <Utilities/Logger.h>

namespace UnrealVR
{
    class HookHelper
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
                Log::Error("[UnrealVR] Failed to create hook %s", displayName.c_str());
                return false;
            }
            if (MH_EnableHook(reinterpret_cast<LPVOID>(reinterpret_cast<DWORD64>(pTarget))) != MH_OK)
            {
                Log::Error("[UnrealVR] Failed to enable hook %s", displayName.c_str());
                return false;
            }
            Log::Info("[UnrealVR] Added hook %s", displayName.c_str());
            return true;
        }

        /** Disable/remove all hooks */
        static void Stop();
    };
}
