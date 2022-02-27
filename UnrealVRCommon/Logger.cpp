#include "Logger.h"

namespace UnrealVR
{
	namespace Logger
	{
		void Init(bool bNewWindow)
		{
			if (bNewWindow)
			{
				ShowWindow(GetConsoleWindow(), SW_SHOW);
				FreeConsole();
			}
			AllocConsole();
			FILE* fpstdin = stdin;
			FILE* fpstdout = stdout;
			FILE* fpstderr = stderr;
			freopen_s(&fpstdin, "CONIN$", "r", stdin);
			freopen_s(&fpstdout, "CONOUT$", "w", stdout);
			freopen_s(&fpstderr, "CONOUT$", "w", stderr);
		}

		void Info(std::wstring message)
		{
			std::wcout << message << "\n";
		}

		void Error(std::wstring message)
		{
			std::wcout << message << "\n";
		}
	}
}