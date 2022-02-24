#include "Launcher.h"

namespace UnrealVR
{
    PROCESS_INFORMATION Launcher::procInfo;

    bool Launcher::StartGame(std::wstring gamePath)
    {
        std::wstring gameDir = gamePath.substr(0, gamePath.find_last_of(L"/\\"));
        STARTUPINFOW startupInfo;
        memset(&startupInfo, 0, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);
        if (!CreateProcessW(gamePath.c_str(), NULL, NULL, NULL, TRUE, 0, NULL, gameDir.c_str(), &startupInfo, &procInfo))
        {
            Logger::Info(L"Failed to create game process");
            return false;
        };
        //if (SuspendThread(procInfo.hThread) == -1)
        if (false)
        {
            Logger::Info(L"Failed to suspend thread");
            return false;
        }
        Logger::Info(L"Created game process and suspended main thread");
        return true;
    }

    bool Launcher::InjectDLL(std::string launcherPath)
    {
        std::string dllPath = launcherPath.substr(0, launcherPath.find_last_of("/\\")) + "\\VirtualUnrealityEngineLoader.dll";
        void* loc = VirtualAllocEx(procInfo.hProcess, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!loc)
        {
            Logger::Error(L"Failed to allocate memory in remote process");
            return false;
        }
        if (!WriteProcessMemory(procInfo.hProcess, loc, dllPath.c_str(), strlen(dllPath.c_str()) + 1, 0))
        {
            Logger::Error(L"Failed to write DLL injection to memory");
            return false;
        }
        HANDLE hThread = CreateRemoteThread(procInfo.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, NULL);
        if (!hThread)
        {
            Logger::Error(L"Failed to obtain remote thread");
            return false;
        }
        else if (!CloseHandle(hThread))
        {
            Logger::Error(L"Failed to close DLL thread");
            return false;
        }
        if (!CloseHandle(procInfo.hProcess))
        {
            Logger::Error(L"Failed to close game process");
            return false;
        }
        if (!CloseHandle(procInfo.hThread))
        {
            Logger::Error(L"Failed to close game thread");
            return false;
        }
        Logger::Info(L"Started DLL injection");
        return true;
    }
}
