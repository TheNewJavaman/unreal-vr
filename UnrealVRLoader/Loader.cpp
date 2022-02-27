#include "Loader.h"

namespace UnrealVR
{
	namespace Loader
	{
		bool ResumeGame();

		void Init()
		{
			if (!VRManager::Init())
			{
				Log::Error("Failed to init VR");
				return;
			}
			if (!HookManager::Init())
			{
				Log::Error("Failed to init hooks");
				return;
			}
			D3D11Manager::AddHooks();
			if (!ResumeGame())
			{
				Log::Error("Failed to resume game");
				return;
			}
		}

		bool ResumeGame()
		{
			HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
			THREADENTRY32 te32;
			hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
			if (hThreadSnap == INVALID_HANDLE_VALUE)
			{
				Log::Error("Failed to create thread snap");
				return false;
			}
			te32.dwSize = sizeof(THREADENTRY32);
			if (!Thread32First(hThreadSnap, &te32))
			{
				Log::Error("Failed to find first game process thread");
				if (!CloseHandle(hThreadSnap))
				{
					Log::Error("Failed to close thread snap handle");
					return false;
				}
				return false;
			}
			do {
				if (te32.th32OwnerProcessID == GetCurrentProcessId() && te32.th32ThreadID != GetCurrentThreadId())
				{
					HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
					if (ResumeThread(hThread) == -1)
					{
						Log::Error("Failed to resume a thread");
					}
					if (!CloseHandle(hThread))
					{
						Log::Error("Failed to close a thread handle");
					}
				}
			} while (Thread32Next(hThreadSnap, &te32));
			if (!CloseHandle(hThreadSnap))
			{
				Log::Error("Failed to close thread snap handle");
				return false;
			}
			return true;
		}

		void Stop()
		{
			VRManager::Stop();
			HookManager::Stop();
		}
	}
}