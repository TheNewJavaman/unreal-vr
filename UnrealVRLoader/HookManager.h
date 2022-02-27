#pragma once
#pragma once

#include <string>
#include <format>
#include "MinHook.h"
#include "Utilities/Logger.h"

namespace UnrealVR
{
	namespace HookManager
	{
		bool Init();

		template <typename T>
		bool Add(T* pTarget, T* pDetour, T** ppOriginal, std::string displayName)
		{
			if (MH_CreateHook((LPVOID)(DWORD64)pTarget, (LPVOID)pDetour, reinterpret_cast<LPVOID*>(ppOriginal)) != MH_OK)
			{
				Log::Error(std::format("Failed to create hook ({})", displayName));
				return false;
			}
			if (MH_EnableHook((LPVOID)(DWORD64)pTarget) != MH_OK)
			{
				Log::Error(std::format("Failed to enable hook ({})", displayName));
				return false;
			}
			Log::Error(std::format("Added hook ({})", displayName));
			return true;
		}

		void Stop();
	};
}