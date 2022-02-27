#include "HookManager.h"

namespace UnrealVR
{
	namespace HookManager
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
}