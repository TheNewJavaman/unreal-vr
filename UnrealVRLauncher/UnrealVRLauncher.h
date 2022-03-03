#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string>

namespace UnrealVR
{
	class Launcher
	{
	public:
		static bool StartGame(std::wstring gamePath);
		
		static bool InjectDLL(std::string launcherPath, std::string dllName);
		
		static bool CloseHandles();
		
	private:
		inline static PROCESS_INFORMATION procInfo = {};
	};
}
