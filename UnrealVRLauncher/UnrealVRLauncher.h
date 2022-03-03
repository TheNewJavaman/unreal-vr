#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string>

namespace UnrealVR
{
	class Launcher
	{
	public:
		/** Create game process */
		static bool StartGame(std::wstring gamePath);

		/** Inject a DLL located in the launcher's folder */
		static bool InjectDLL(std::string launcherPath, std::string dllName);

		/** Close the game process handles */
		static bool CloseHandles();
		
	private:
		inline static PROCESS_INFORMATION procInfo = {};
	};
}
