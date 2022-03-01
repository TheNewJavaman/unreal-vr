#include "UnrealVRMod.h"

namespace UnrealVR
{
	void CoreMod::InitializeMod()
	{
		UE4::InitSDK();
		SetupHooks();
		MinHook::Init();
		Log::Info("[UnrealVR] Initialized coremod");
	}
}