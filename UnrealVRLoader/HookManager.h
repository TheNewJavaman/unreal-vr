#pragma once
#pragma once

#include <string>
#include <format>
#include "Utilities/Logger.h"
#include "MinHook.h"
#include "Defs.h"

namespace UnrealVR
{
	namespace HookManager
	{
		bool Init();

		template <typename T>
		UNREALVR_API bool Add(T* pTarget, T* pDetour, T** ppOriginal, std::string displayName)
		{
			if (MH_CreateHook((LPVOID)(DWORD64)pTarget, (LPVOID)pDetour, reinterpret_cast<LPVOID*>(ppOriginal)) != MH_OK)
			{
				Log::Info(std::format("Failed to create hook ({})", displayName) + "");
				return false;
			}
			if (MH_EnableHook((LPVOID)(DWORD64)pTarget) != MH_OK)
			{
				Log::Info(std::format("Failed to enable hook ({})", displayName) + "");
				return false;
			}
			Log::Info(std::format("Added hook ({})", displayName) + "");
			return true;
		}

		void Stop();
	};
}