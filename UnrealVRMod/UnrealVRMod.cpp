#include "UnrealVRMod.h"

namespace UnrealVR
{
	void CoreMod::InitializeMod()
	{
		UE4::InitSDK();
		Log::Info("[UnrealVR] Initialized coremod");
	}
}