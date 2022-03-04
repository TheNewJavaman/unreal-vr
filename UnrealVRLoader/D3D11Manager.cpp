#include "D3D11Manager.h"

#include <algorithm>
#include <Utilities/Logger.h>

#include "HookManager.h"
#include "Matrix.h"

#define VTABLE(instance) reinterpret_cast<DWORD_PTR*>(reinterpret_cast<DWORD_PTR*>(instance)[0]);

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
        const DWORD_PTR* factory2VTable = VTABLE(factory2);

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
        const DWORD_PTR* swapChainVTable = VTABLE(swapChain);
        const DWORD_PTR* contextVTable = VTABLE(context);

        CreateSwapChainTarget = reinterpret_cast<CreateSwapChainFunc*>(factory2VTable[10]);
        CreateSwapChainForHwndTarget = reinterpret_cast<CreateSwapChainForHwndFunc*>(factory2VTable[15]);
        VSSetConstantBuffersTarget = reinterpret_cast<VSSetConstantBuffersFunc*>(contextVTable[7]);
        UpdateSubresourceTarget = reinterpret_cast<UpdateSubresourceFunc*>(contextVTable[48]);
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
        HookManager::Add<UpdateSubresourceFunc>(
            UpdateSubresourceTarget,
            &UpdateSubresourceDetour,
            &UpdateSubresourceOriginal,
            "UpdateSubresource"
        );
        HookManager::Add<PresentFunc>(
            PresentTarget,
            &PresentDetour,
            &PresentOriginal,
            "Present"
        );
        DWORD createSwapChainOld;
        DWORD createSwapChainForHwndOld;
        DWORD vsSetConstantBuffersOld;
        DWORD updateSubresourceOld;
        DWORD presentOld;
        VirtualProtect(CreateSwapChainTarget, 2, PAGE_EXECUTE_READWRITE, &createSwapChainOld);
        VirtualProtect(CreateSwapChainForHwndTarget, 2, PAGE_EXECUTE_READWRITE, &createSwapChainForHwndOld);
        VirtualProtect(VSSetConstantBuffersTarget, 2, PAGE_EXECUTE_READWRITE, &vsSetConstantBuffersOld);
        VirtualProtect(UpdateSubresourceTarget, 2, PAGE_EXECUTE_READWRITE, &updateSubresourceOld);
        VirtualProtect(PresentTarget, 2, PAGE_EXECUTE_READWRITE, &presentOld);

        Log::Info("[UnrealVR] Added D3D11/DXGI hooks");

        while (true)
        {
            Sleep(10);
        }

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
        HWND hwnd,
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

    void D3D11Manager::VSSetConstantBuffersDetour(
        ID3D11DeviceContext* pDeviceContext,
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer** ppConstantBuffers
    )
    {
        for (int i = 0; i < NumBuffers; i++)
        {
            ID3D11Buffer* buffer = ppConstantBuffers[i];
            if (buffer != nullptr)
            {
                D3D11_BUFFER_DESC desc;
                buffer->GetDesc(&desc);
                if (desc.ByteWidth == 64 && !matrixResources.contains(buffer))
                {
                    Log::Info("[UnrealVR] Possibly found a view matrix at (0x%p)",buffer);
                    matrixResources.insert(buffer);
                }
            }
        }
        VSSetConstantBuffersOriginal(pDeviceContext, StartSlot, NumBuffers, ppConstantBuffers);
    }

    void D3D11Manager::UpdateSubresourceDetour(
        ID3D11DeviceContext* pDeviceContext,
        ID3D11Resource* pDstResource,
        UINT DstSubresource,
        D3D11_BOX* pDstBox,
        void* pSrcData,
        UINT SrcRowPitch,
        UINT SrcDepthPitch
    )
    {
        if (std::ranges::any_of(matrixResources, [pDstResource](ID3D11Resource* resource)
        {
            return resource == pDstResource;
        }))
        {
            const Matrix translation = Matrix::FromTranslation(0, 0, 10000);
            const Matrix* original = static_cast<Matrix*>(pSrcData);
            Matrix transformed = translation * *original;
            Log::Info("[UnrealVR] Intercepted a matrix resource update");
            UpdateSubresourceOriginal(pDeviceContext, pDstResource, DstSubresource, pDstBox,
                                      static_cast<void*>(&transformed), SrcRowPitch, SrcDepthPitch);
        }
        else
        {
            UpdateSubresourceOriginal(pDeviceContext, pDstResource, DstSubresource, pDstBox,
                                      pSrcData, SrcRowPitch, SrcDepthPitch);
        }
    }

    HRESULT __stdcall D3D11Manager::PresentDetour(
        IDXGISwapChain* pSwapChain,
        UINT SyncInterval,
        UINT Flags
    )
    {
        PresentOriginal(pSwapChain, SyncInterval, Flags);
        return S_OK;
    }
}
