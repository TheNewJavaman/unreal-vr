#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tlHelp32.h>
#include <processthreadsapi.h>
#include <MinHook.h>
#include "Logger.h"
#include "HookManager.h"
#include "D3D11LoaderManager.h"
#include "VRLoaderManager.h"

namespace UnrealVR
{
	namespace Loader
	{
		void Init();
		void Stop();
	};
}
