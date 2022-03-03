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
        /** Hooking runs in a separate thread due to DXGI quirks */
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
         * CreateBuffer functions
         *
         * If the buffer has a byte width of 64, flag the handle internally as the projection view matrix. Why 64 bytes?
         *     Float:     4 bytes
         *     Rows:      4
         *     Columns: x 4
         *     ------------------
         *               64 bytes
         */
        typedef HRESULT (__stdcall CreateBufferFunc)(
            ID3D11Device* pDevice,
            const D3D11_BUFFER_DESC* pDesc,
            const D3D11_SUBRESOURCE_DATA* pInitialData,
            ID3D11Buffer** ppBuffer
        );
        static CreateBufferFunc CreateBufferDetour;
        inline static CreateBufferFunc* CreateBufferTarget = nullptr;
        inline static CreateBufferFunc* CreateBufferOriginal = nullptr;

        /**
         * UpdateSubresource functions
         *
         * When the subresource handle equals the projection view matrix handle, apply transformations to the matrix:
         *     1. Rotate to match the player's head movement
         *     2. Translate to a specific eye
         *     3. Translate to match the player's head movement
         *
         * It is not feasible to hook ID3D11DeviceContext::VSSetConstantBuffers instead of this function. We would have
         * to read the buffer's contents via ID3D11DeviceContext::Map each frame, which has two issues: a) it would
         * probably introduce latency; b) the buffer was not created with CPU read access. Therefore, it is easier and
         * faster to hook this function instead.
         *
         * As of 4.27, Unreal Engine does not use v1 of this function, just this one (the original) instead 
         */
        typedef void (__stdcall UpdateSubresourceFunc)(
            ID3D11DeviceContext* pDeviceContext,
            ID3D11Resource* pDstResource,
            UINT DstSubresource,
            const D3D11_BOX* pDstBox,
            const void* pSrcData,
            UINT SrcRowPitch,
            UINT SrcDepthPitch
        );
        static UpdateSubresourceFunc UpdateSubresourceDetour;
        inline static UpdateSubresourceFunc* UpdateSubresourceTarget = nullptr;
        inline static UpdateSubresourceFunc* UpdateSubresourceOriginal = nullptr;

        /**
         * Present functions
         *
         * When Unreal Engine indicates to DXGI that the frame is ready to be presented, we must call the analogous
         * function on OpenXR's side, and switch eyes.
         *
         * TODO: How will we display to the monitor? Just rely on the SteamVR preview? Will the window be fullscreen?
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
