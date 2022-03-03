#pragma once

#include <d3d11.h>
#include <dxgi1_6.h>

namespace UnrealVR
{
    class D3D11Manager
    {
    public:
        static void AddHooks();

    private:
        static DWORD __stdcall AddHooksThread(LPVOID);

        typedef HRESULT (__stdcall CreateSwapChainFunc)(
            IDXGIFactory* pFactory,
            IUnknown* pDevice,
            DXGI_SWAP_CHAIN_DESC* pDesc,
            IDXGISwapChain** ppSwapChain
        );
        static CreateSwapChainFunc CreateSwapChainDetour;
        inline static CreateSwapChainFunc* CreateSwapChainTarget = nullptr;
        inline static CreateSwapChainFunc* CreateSwapChainOriginal = nullptr;

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

        typedef void (__stdcall VSSetConstantBuffersFunc)(
            ID3D11DeviceContext* pContext,
            UINT StartSlot,
            UINT NumBuffers,
            ID3D11Buffer* const* ppConstantBuffers
        );
        static VSSetConstantBuffersFunc VSSetConstantBuffersDetour;
        inline static VSSetConstantBuffersFunc* VSSetConstantBuffersTarget = nullptr;
        inline static VSSetConstantBuffersFunc* VSSetConstantBuffersOriginal = nullptr;

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
