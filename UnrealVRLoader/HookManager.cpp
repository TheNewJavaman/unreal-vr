#include "HookManager.h"

namespace UnrealVR
{
	bool HookManager::Init()
	{
		if (MH_Initialize() != MH_OK)
		{
			Logger::Error(L"Failed to initialize MinHook");
			return false;
		}
		Logger::Info(L"Initialized MinHook");
		return true;
	}

	template <typename T>
	bool HookManager::Add(DWORD_PTR pTarget, LPVOID pDetour, T** ppOriginal, std::wstring displayName)
	{
		if (MH_CreateHook((LPVOID)pTarget, pDetour, reinterpret_cast<LPVOID*>(ppOriginal)) != MH_OK)
		{
			Logger::Error(L"Failed to create hook (" + displayName + L")");
			return false;
		}
		if (MH_EnableHook((LPVOID)pTarget) != MH_OK)
		{
			Logger::Error(L"Failed to enable hook (" + displayName + L")");
			return false;
		}
		Logger::Info(L"Added hook (" + displayName + L")");
		return true;
	}

	bool HookManager::Stop()
	{
		if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK)
		{
			Logger::Error(L"Failed to disable all hooks");
			return false;
		}
		if (MH_Uninitialize() != MH_OK)
		{
			Logger::Error(L"Failed to uninitialize MinHook");
			return false;
		}
		Logger::Info(L"Stopped MinHook");
		return true;
	}
}