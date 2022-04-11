#pragma once

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
        static void HandleCommand(CHAR buffer[]);
    };

    enum class Setting : int
    {
        CmUnitsScale = 0x00,
        FOVScale = 0x01
    };
}