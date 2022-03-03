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

        const WNDCLASSEXA wc = {
            sizeof(WNDCLASSEX),
            CS_CLASSDC,
            DefWindowProc,
            0L,
            0L,
            GetModuleHandleA(nullptr),
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            "DX",
            nullptr
        };
        RegisterClassExA(&wc);
        const HWND hWnd = CreateWindowA("DX", NULL, WS_OVERLAPPEDWINDOW,
                                        100, 100, 300, 300,
                                        NULL, NULL, wc.hInstance, NULL);
        constexpr D3D_FEATURE_LEVEL requestedLevels[] = {D3D_FEATURE_LEVEL_11_0};
        D3D_FEATURE_LEVEL obtainedLevel;
        DXGI_SWAP_CHAIN_DESC scd;
        ZeroMemory(&scd, sizeof(scd));
        scd.BufferCount = 1;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        scd.OutputWindow = hWnd;
        scd.SampleDesc.Count = 1;
        scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        scd.Windowed = ((GetWindowLongPtr(hWnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;
        scd.BufferDesc.Width = 1;
        scd.BufferDesc.Height = 1;
        scd.BufferDesc.RefreshRate.Numerator = 0;
        scd.BufferDesc.RefreshRate.Denominator = 1;
        constexpr UINT createFlags = 0;
        IDXGISwapChain* swapChain;
        ID3D11Device* device;
        ID3D11DeviceContext* context;
        D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            createFlags,
            requestedLevels,
            sizeof(requestedLevels) / sizeof(D3D_FEATURE_LEVEL),
            D3D11_SDK_VERSION,
            &scd,
            &swapChain,
            &device,
            &obtainedLevel,
            &context
        );
        const DWORD_PTR* contextVTable = reinterpret_cast<DWORD_PTR*>(reinterpret_cast<DWORD_PTR*>(context)[0]);
        const DWORD_PTR* swapChainVTable = reinterpret_cast<DWORD_PTR*>(reinterpret_cast<DWORD_PTR*>(swapChain)[0]);

        CreateSwapChainTarget = reinterpret_cast<CreateSwapChainFunc*>(factory2VTable[10]);
        CreateSwapChainForHwndTarget = reinterpret_cast<CreateSwapChainForHwndFunc*>(factory2VTable[15]);
        VSSetConstantBuffersTarget = reinterpret_cast<VSSetConstantBuffersFunc*>(contextVTable[7]);
        PresentTarget = reinterpret_cast<PresentFunc*>(swapChainVTable[8]);
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
        HookManager::Add<VSSetConstantBuffersFunc>(
            VSSetConstantBuffersTarget,
            &VSSetConstantBuffersDetour,
            &VSSetConstantBuffersOriginal,
            "VSSetConstantBuffers"
        );
        /*
        HookManager::Add<PresentFunc>(
            PresentTarget,
            &PresentDetour,
            &PresentOriginal,
            "Present"
        );
         */
        DWORD createSwapChainOld;
        DWORD createSwapChainForHwndOld;
        DWORD vsSetConstantBuffersOld;
        DWORD presentOld;
        VirtualProtect(CreateSwapChainTarget, 2, PAGE_EXECUTE_READWRITE, &createSwapChainOld);
        VirtualProtect(CreateSwapChainForHwndTarget, 2, PAGE_EXECUTE_READWRITE, &createSwapChainForHwndOld);
        VirtualProtect(VSSetConstantBuffersTarget, 2, PAGE_EXECUTE_READWRITE, &vsSetConstantBuffersOld);
        //VirtualProtect(PresentTarget, 2, PAGE_EXECUTE_READWRITE, &presentOld);

        Log::Info("[UnrealVR] Added D3D11 hooks");

        while (true)
        {
            Sleep(10);
        }

        // ReSharper disable once CppUnreachableCode

        context->Release();
        device->Release();
        swapChain->Release();

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
        Log::Info("[UnrealVR] Intercepted CreateSwapChain");
        CreateSwapChainOriginal(pFactory, pDevice, pDesc, ppSwapChain);
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
        Log::Info("[UnrealVR] Intercepted CreateSwapChainForHwnd");
        CreateSwapChainForHwndOriginal(pFactory, pDevice, hwnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
        return S_OK;
    }

    void __stdcall D3D11Manager::VSSetConstantBuffersDetour(
        ID3D11DeviceContext* pContext,
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer* const* ppConstantBuffers
    )
    {
        Log::Info("[UnrealVR] Intercepted VSSetConstantBuffers");
        for (int i = 0; i < NumBuffers; i++)
        {
            if (ppConstantBuffers[i] != nullptr)
            {
                D3D11_BUFFER_DESC desc;
                ppConstantBuffers[i]->GetDesc(&desc);
                Log::Info("%d, %d", desc.CPUAccessFlags, desc.ByteWidth);
                ID3D11Resource* resource;
                ppConstantBuffers[i]->QueryInterface(IID_PPV_ARGS(&resource));
                D3D11_MAPPED_SUBRESOURCE subresource;
                HRESULT hr = pContext->Map(resource, 0, D3D11_MAP_READ_WRITE, 0, &subresource);
                if (hr != S_OK)
                {
                    Log::Error("[UnrealVR] Couldn't map VS buffer; error (%X)", hr);
                }
                else
                {
                    pContext->Unmap(resource, 0);
                }
                resource->Release();
            }
        }
        VSSetConstantBuffersOriginal(pContext, StartSlot, NumBuffers, ppConstantBuffers);
    }

    HRESULT __stdcall D3D11Manager::PresentDetour(
        IDXGISwapChain* pSwapChain,
        UINT SyncInterval,
        UINT Flags
    )
    {
        Log::Info("[UnrealVR] Intercepted Present");
        PresentOriginal(pSwapChain, SyncInterval, Flags);
        return S_OK;
    }
}
