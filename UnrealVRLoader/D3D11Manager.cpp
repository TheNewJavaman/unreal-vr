#include "D3D11Manager.h"

#include <Utilities/Logger.h>

#include "HookManager.h"

namespace UnrealVR
{
    void D3D11Manager::AddHooks()
    {
        CreateThread(nullptr, 0, AddHooksThread, nullptr, 0, nullptr);
    }

    DWORD __stdcall D3D11Manager::AddHooksThread(LPVOID)
    {
        IDXGIFactory* factory;
        CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory));
        IDXGIFactory2* factory2;
        factory->QueryInterface<IDXGIFactory2>(&factory2);
        const DWORD_PTR* factory2VTable = reinterpret_cast<DWORD_PTR*>(reinterpret_cast<DWORD_PTR*>(factory2)[0]);

        CreateSwapChainTarget = reinterpret_cast<CreateSwapChainFunc*>(factory2VTable[10]);
        CreateSwapChainForHwndTarget = reinterpret_cast<CreateSwapChainForHwndFunc*>(factory2VTable[15]);
        HookManager::Add<CreateSwapChainFunc>(
            CreateSwapChainTarget,
            &CreateSwapChainDetour,
            &CreateSwapChainOriginal,
            "CreateSwapChain"
        );
        HookManager::Add<CreateSwapChainForHwndFunc>(
            CreateSwapChainForHwndTarget,
            &CreateSwapChainForHwndDetour,
            &CreateSwapChainForHwndOriginal,
            "CreateSwapChainForHwnd"
        );
        DWORD createSwapChainOld;
        DWORD createSwapChainForHwndOld;
        VirtualProtect(CreateSwapChainTarget, 2, PAGE_EXECUTE_READWRITE, &createSwapChainOld);
        VirtualProtect(CreateSwapChainForHwndTarget, 2, PAGE_EXECUTE_READWRITE, &createSwapChainForHwndOld);

        Log::Info("[UnrealVR] Added D3D11 hooks");

        while (true)
        {
            Sleep(10);
        }

        // ReSharper disable once CppUnreachableCode
        factory2->Release();
        factory->Release();

        return NULL;
    }

    HRESULT __stdcall D3D11Manager::CreateSwapChainDetour(
        IDXGIFactory* pFactory,
        IUnknown* pDevice,
        DXGI_SWAP_CHAIN_DESC* pDesc,
        IDXGISwapChain** ppSwapChain
    )
    {
        CreateSwapChainOriginal(pFactory, pDevice, pDesc, ppSwapChain);
        Log::Info("[UnrealVR] Intercepted CreateSwapChain");
        return S_OK;
    }

    HRESULT __stdcall D3D11Manager::CreateSwapChainForHwndDetour(
        IDXGIFactory2* pFactory,
        IUnknown* pDevice,
        const HWND hwnd,
        DXGI_SWAP_CHAIN_DESC1* pDesc,
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
        IDXGIOutput* pRestrictToOutput,
        IDXGISwapChain1** ppSwapChain
    )
    {
        CreateSwapChainForHwndOriginal(pFactory, pDevice, hwnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
        Log::Info("[UnrealVR] Intercepted CreateSwapChainForHwnd");
        return S_OK;
    }
}
