#include "UnrealVRLauncher.h"

#include <iostream>

namespace UnrealVR
{
    bool Launcher::StartGame(const std::wstring gamePath)
    {
        const std::wstring gameDir = gamePath.substr(0, gamePath.find_last_of(L"/\\"));
        STARTUPINFOW startupInfo = {};
        startupInfo.cb = sizeof startupInfo;
        if (!CreateProcessW(gamePath.c_str(),
                            nullptr,
                            nullptr,
                            nullptr,
                            TRUE,
                            0,
                            nullptr,
                            gameDir.c_str(),
                            &startupInfo,
                            &procInfo
        ))
        {
            std::cout << "Failed to create game process\n";
            return false;
        }
        std::cout << "Created game process\n";
        return true;
    }

    bool Launcher::InjectDLL(const std::string launcherPath, const std::string dllName)
    {
        const std::string dllPath = launcherPath.substr(0, launcherPath.find_last_of("/\\")) + "\\" + dllName;
        void* loc = VirtualAllocEx(procInfo.hProcess, nullptr, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!loc)
        {
            std::cout << "Failed to allocate memory in remote process\n";
            return false;
        }
        if (!WriteProcessMemory(procInfo.hProcess, loc, dllPath.c_str(), strlen(dllPath.c_str()) + 1, nullptr))
        {
            std::cout << "Failed to write DLL injection to memory\n";
            return false;
        }
        const HANDLE hThread = CreateRemoteThread(
            procInfo.hProcess,
            nullptr,
            0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA),
            loc,
            0,
            nullptr
        );
        if (!hThread)
        {
            std::cout << "Failed to obtain remote thread\n";
            return false;
        }
        if (ResumeThread(procInfo.hThread) == -1)
        {
            std::cout << "Failed to resume DLL thread\n";
            return false;
        }
        if (!CloseHandle(hThread))
        {
            std::cout << "Failed to close DLL thread\n";
            return false;
        }
        std::cout << "Injected " + dllName + "\n";
        return true;
    }

    bool Launcher::CloseHandles()
    {
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
        std::cout << "Successfully closed handles\n";
        return true;
    }
}
