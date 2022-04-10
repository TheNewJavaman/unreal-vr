#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

namespace UnrealVR
{
    // TODO: Write a dependency injection system
	class Loader
	{
	public:
		static void Init();

		static void Stop();

	private:
		static DWORD __stdcall InitThread(LPVOID);
	};
}
