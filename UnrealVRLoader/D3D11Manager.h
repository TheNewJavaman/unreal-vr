#pragma once

#include <d3d11.h>
#include <dxgi1_6.h>

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
         * CreateSwapChain functions
         * 
         * Use OpenXR's internal swapchain builder instead of D3D11; allows much more efficient frame processing, since
         * UE can write directly to the VR swapchain, instead of UnrealVR copying UE's backbuffer to a separate VR
         * resource
         *
         * Also allows RGB10A2-formatted textures, which OpenVR didn't like. Converting to RGBA8 was annoying and
         * inefficient, but was necessary because OpenVR couldn't handle HDR-formatted textures
         *
         * As of 4.27, Unreal Engine only uses 2/6 possible methods to create the swapchain
         */
        typedef HRESULT (__stdcall CreateSwapChainFunc)(
            IDXGIFactory* pFactory,
            IUnknown* pDevice,
            DXGI_SWAP_CHAIN_DESC* pDesc,
            IDXGISwapChain** ppSwapChain
        );
        static CreateSwapChainFunc CreateSwapChainDetour;
        inline static CreateSwapChainFunc* CreateSwapChainTarget = nullptr;
        inline static CreateSwapChainFunc* CreateSwapChainOriginal = nullptr;

        /**
         * CreateSwapChainForHwnd functions
         *
         * Same details as described for CreateSwapChain
         */
        typedef HRESULT (__stdcall CreateSwapChainForHwndFunc)(
            IDXGIFactory2* pFactory,
            IUnknown* pDevice,
            HWND hWnd,
            DXGI_SWAP_CHAIN_DESC1* pDesc,
            DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
            IDXGIOutput* pRestrictToOutput,
            IDXGISwapChain1** ppSwapChain
        );
        static CreateSwapChainForHwndFunc CreateSwapChainForHwndDetour;
        inline static CreateSwapChainForHwndFunc* CreateSwapChainForHwndTarget = nullptr;
        inline static CreateSwapChainForHwndFunc* CreateSwapChainForHwndOriginal = nullptr;

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
    };
}
