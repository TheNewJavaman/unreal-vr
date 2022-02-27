#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "Logger.h"

namespace UnrealVR
{
	namespace Launcher
	{
		bool StartGame(std::wstring gamePath);
		bool InjectDLL(std::string launcherPath);
	};
}
