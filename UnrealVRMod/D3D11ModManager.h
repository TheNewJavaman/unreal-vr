#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

namespace UnrealVR
{
	class PresentManager
	{
	public:
		static void AddHooks();
		static bool IsHooked;

	private:
		static DWORD __stdcall AddHooksThread();

		typedef HRESULT(__stdcall* Present)(
			IDXGISwapChain* pSwapChain,
			UINT SyncInterval,
			UINT Flags
			);
	};
}