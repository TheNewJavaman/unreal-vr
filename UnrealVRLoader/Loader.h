#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tlHelp32.h>
#include <processthreadsapi.h>
#include <MinHook.h>
#include "Logger.h"
#include "HookManager.h"
#include "D3D11Manager.h"

namespace UnrealVR
{
	class Loader
	{
	public:
		static void Init();
		static void Stop();

	private:
		static bool ResumeGame();
	};
}
