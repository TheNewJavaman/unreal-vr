#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "Logger.h"

namespace UnrealVR
{
	class Launcher
	{
	public:
		static bool StartGame(std::wstring gamePath);
		static bool InjectDLL(std::string launcherPath);

	private:
		static PROCESS_INFORMATION procInfo;
	};
}
