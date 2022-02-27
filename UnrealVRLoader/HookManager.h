#pragma once
#pragma once

#include <string>
#include <format>
#include "MinHook.h"
#include "Logger.h"

namespace UnrealVR
{
	namespace HookManager
	{
		bool Init();

		template <typename T>
		bool Add(T* pTarget, T* pDetour, T** ppOriginal, std::wstring displayName)
		{
			if (MH_CreateHook((LPVOID)(DWORD64)pTarget, (LPVOID)pDetour, reinterpret_cast<LPVOID*>(ppOriginal)) != MH_OK)
			{
				Logger::Error(std::format(L"Failed to create hook ({})", displayName));
				return false;
			}
			if (MH_EnableHook((LPVOID)(DWORD64)pTarget) != MH_OK)
			{
				Logger::Error(std::format(L"Failed to enable hook ({})", displayName));
				return false;
			}
			Logger::Error(std::format(L"Added hook ({})", displayName));
			return true;
		}

		bool Stop();
	};
}