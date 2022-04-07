#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

namespace UnrealVR
{
    class PipeClient
    {
    public:
        /** Connects to the app UI to listen for settings changes in realtime */
        static bool Init();

        /** Closes the pipe connection */
        static void Stop();

    private:
        /** Run the client in a separate thread */
        static DWORD __stdcall InitThread(LPVOID);
        inline static HANDLE hPipe = nullptr;

        /** Handle change in settings */
        static void HandleCommand(TCHAR buffer[]);
    };

    enum class Setting : int
    {
        CmUnitsScale = 0x00
    };
}