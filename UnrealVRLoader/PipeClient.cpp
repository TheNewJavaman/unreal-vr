#include "PipeClient.h"

#include "UE4Manager.h"
#include "Utilities/Logger.h"

#define BUFFER_SIZE 512

namespace UnrealVR
{
    bool PipeClient::Init()
    {
        hPipe = CreateFile(
            TEXT("\\\\.\\pipe\\UnrealVR"),
            GENERIC_READ,
            0,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );
        if (hPipe == INVALID_HANDLE_VALUE)
        {
            Log::Error("[UnrealVR] Couldn't open named pipe; error %d", GetLastError());
            return false;
        }
        CreateThread(nullptr, 0, InitThread, nullptr, 0, nullptr);
        return true;
    }

    DWORD PipeClient::InitThread(LPVOID)
    {
        bool fs = false;
        DWORD cbRead;
        while (fs)
        {
            TCHAR chBuf[BUFFER_SIZE];
            fs = ReadFile(hPipe, chBuf, BUFFER_SIZE * sizeof(TCHAR), &cbRead, nullptr);
            if (!fs && GetLastError() != ERROR_MORE_DATA) break;
            HandleCommand(chBuf);
        }
        return NULL;
    }

    void PipeClient::HandleCommand(TCHAR buffer[])
    {
        switch(static_cast<Setting>(buffer[0]))
        {
        case Setting::CmUnitsScale:
            UE4Manager::CmUnitsScale = *reinterpret_cast<float*>(&buffer[1]);
            break;
        }
    }

    void PipeClient::Stop()
    {
        CloseHandle(hPipe);
    }
}
