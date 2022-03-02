#include "HookManager.h"

namespace UnrealVR
{
	namespace HookManager
	{
		bool Init()
		{
			if (MH_Initialize() != MH_OK)
			{
				Log::Error("[UnrealVR] Failed to initialize MinHook");
				return false;
			}
			Log::Info("[UnrealVR] Initialized MinHook");
			return true;
		}

		void Stop()
		{
			if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK)
			{
				Log::Error("[UnrealVR] Failed to disable all hooks");
				return;
			}
			if (MH_Uninitialize() != MH_OK)
			{
				Log::Error("[UnrealVR] Failed to uninitialize MinHook");
				return;
			}
			Log::Info("[UnrealVR] Stopped MinHook");
		}
	}
}