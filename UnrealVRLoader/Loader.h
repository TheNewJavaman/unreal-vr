#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tlHelp32.h>
#include <processthreadsapi.h>
#include <MinHook.h>
#include "Utilities/Logger.h"
#include "HookManager.h"
#include "D3D11Manager.h"
#include "VRManager.h"

namespace UnrealVR
{
	namespace Loader
	{
		void Init();
		void Stop();
	};
}
