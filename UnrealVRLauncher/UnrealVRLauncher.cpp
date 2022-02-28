#include "UnrealVRLauncher.h"

namespace UnrealVR
{
    namespace Launcher
    {
        PROCESS_INFORMATION procInfo;

        bool StartGame(std::wstring gamePath)
        {
            std::wstring gameDir = gamePath.substr(0, gamePath.find_last_of(L"/\\"));
            STARTUPINFOW startupInfo;
            memset(&startupInfo, 0, sizeof(startupInfo));
            startupInfo.cb = sizeof(startupInfo);
            if (!CreateProcessW(gamePath.c_str(), NULL, NULL, NULL, TRUE, 0, NULL, gameDir.c_str(), &startupInfo, &procInfo))
            {
                std::cout << "Failed to create game process\n";
                return false;
            };
            if (SuspendThread(procInfo.hThread) == -1)
            {
                std::cout << "Failed to suspend main thread\n";
                return false;
            }
            std::cout << "Created game process and suspended main thread\n";
            return true;
        }

        bool InjectDLL(std::string launcherPath)
        {
            std::string dllPath = launcherPath.substr(0, launcherPath.find_last_of("/\\")) + "\\UnrealVRLoader.dll";
            void* loc = VirtualAllocEx(procInfo.hProcess, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (!loc)
            {
                std::cout << "Failed to allocate memory in remote process\n";
                return false;
            }
            if (!WriteProcessMemory(procInfo.hProcess, loc, dllPath.c_str(), strlen(dllPath.c_str()) + 1, 0))
            {
                std::cout << "Failed to write DLL injection to memory\n";
                return false;
            }
            HANDLE hThread = CreateRemoteThread(procInfo.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, NULL);
            if (!hThread)
            {
                std::cout << "Failed to obtain remote thread\n";
                return false;
            }
            else if (!CloseHandle(hThread))
            {
                std::cout << "Failed to close DLL thread\n";
                return false;
            }
            if (!CloseHandle(procInfo.hProcess))
            {
                std::cout << "Failed to close game process\n";
                return false;
            }
            if (!CloseHandle(procInfo.hThread))
            {
                std::cout << "Failed to close game thread\n";
                return false;
            }
            std::cout << "Started DLL injection\n";
            return true;
        }
    }
}
