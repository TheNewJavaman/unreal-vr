#pragma once

#include <d3d11.h>

namespace UnrealVR
{
    class D3D11Manager
    {
    public:
        /** Find VTables and set up D3D11/DXGI hooks */
        static void AddHooks();

        /**
         * Converts Unreal Engine's RGB10A2 format to whatever OpenXR prefers
         *
         * Note that this function makes a copy of the source texture; Unreal Engine usually does not create resources
         * that can bind freely to a shader resource view
         */
        static bool ConvertFrame(ID3D11Texture2D* source, ID3D11RenderTargetView* rtv);

        /** Release D3D11 resources */
        static void Stop();

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

        /** ConvertFrame */
        inline static bool convertResourcesCreated = false;
        inline static ID3D11PixelShader* pixelShader = nullptr;
        inline static ID3D11VertexShader* vertexShader = nullptr;
        inline static ID3D11Texture2D* copy = nullptr;
        inline static ID3D11ShaderResourceView* srv = nullptr;
    };
}
