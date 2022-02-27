#include "Loader.h"

namespace UnrealVR
{
	namespace Loader
	{
		bool ResumeGame();

		void Init()
		{
			Logger::Init(true);
			if (!VRLoaderManager::Init())
			{
				Logger::Error(L"Failed to init VR");
				return;
			}
			if (!HookManager::Init())
			{
				Logger::Error(L"Failed to init hooks");
				return;
			}
			D3D11LoaderManager::AddHooks();
			if (!ResumeGame())
			{
				Logger::Error(L"Failed to resume game");
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
				Logger::Error(L"Failed to create thread snap");
				return false;
			}
			te32.dwSize = sizeof(THREADENTRY32);
			if (!Thread32First(hThreadSnap, &te32))
			{
				Logger::Error(L"Failed to find first game process thread");
				if (!CloseHandle(hThreadSnap))
				{
					Logger::Error(L"Failed to close thread snap handle");
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
						Logger::Error(L"Failed to resume a thread");
					}
					if (!CloseHandle(hThread))
					{
						Logger::Error(L"Failed to close a thread handle");
					}
				}
			} while (Thread32Next(hThreadSnap, &te32));
			if (!CloseHandle(hThreadSnap))
			{
				Logger::Error(L"Failed to close thread snap handle");
				return false;
			}
			return true;
		}

		void Stop()
		{
			if (!HookManager::Stop())
			{
				Logger::Error(L"Failed to stop hooks");
				return;
			}
		}
	}
}