#pragma once

#include <d3d11.h>

namespace UnrealVR
{
    class D3D11Manager
    {
    public:
        /** Find VTables and set up D3D11/DXGI hooks */
        static void AddHooks();

    private:
        /** Hooking runs in a separate thread due to DXGI DLL-loading quirks */
        static DWORD __stdcall AddHooksThread(LPVOID);

        /**
         * Present functions
         *
         * When Unreal Engine indicates to DXGI that the frame is ready to be presented, we must call the analogous
         * function on OpenXR's side, and switch eyes
         *
         * TODO: How will we display to the monitor? Ideas:
         *   - Only use SteamVR preview (confusing for users)
         *   - Create another swapchain for the monitor and copy frames to it (inefficient)
         */
        typedef HRESULT (__stdcall PresentFunc)(
            IDXGISwapChain* pSwapChain,
            UINT SyncInterval,
            UINT Flags
        );
        static PresentFunc PresentDetour;
        inline static PresentFunc* PresentTarget = nullptr;
        inline static PresentFunc* PresentOriginal = nullptr;

        inline static bool resized = false;
    };
}
