#include "D3D11Manager.h"

#include <Utilities/Logger.h>

#include "HookManager.h"
#include "PixelShader.h"
#include "UE4Manager.h"
#include "VertexShader.h"
#include "VRManager.h"

#define VTABLE(instance) reinterpret_cast<DWORD_PTR*>(reinterpret_cast<DWORD_PTR*>(instance)[0]);
#define CHECK_HR(hr, message) \
    if ((hr) != S_OK) { \
        Log::Error("[UnrealVR] %s; error code (%X)", message, hr); \
        return false; \
    }

namespace UnrealVR
{
    class VRManager;

    void D3D11Manager::AddHooks()
    {
        CreateThread(nullptr, 0, AddHooksThread, nullptr, 0, nullptr);
    }

    DWORD __stdcall D3D11Manager::AddHooksThread(LPVOID)
    {
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
        const auto swapChainVTable = VTABLE(swapChain)

        PresentTarget = reinterpret_cast<PresentFunc*>(swapChainVTable[8]);
        HookManager::Add<PresentFunc>(
            PresentTarget,
            &PresentDetour,
            &PresentOriginal,
            "Present"
        );
        DWORD presentOld;
        VirtualProtect(PresentTarget, 2, PAGE_EXECUTE_READWRITE, &presentOld);

        context->Release();
        device->Release();
        swapChain->Release();

        return NULL;
    }

    HRESULT __stdcall D3D11Manager::PresentDetour(
        IDXGISwapChain* pSwapChain,
        UINT SyncInterval,
        UINT Flags
    )
    {
        if (!UE4Manager::GameLoaded)
            return PresentOriginal(pSwapChain, SyncInterval, Flags);
        if (!VRManager::VRLoaded)
        {
            ID3D11Device* device;
            pSwapChain->GetDevice(IID_PPV_ARGS(&device));
            DXGI_SWAP_CHAIN_DESC desc;
            pSwapChain->GetDesc(&desc);
            VRManager::FinalizeInit(device, desc);
            device->Release();
            return PresentOriginal(pSwapChain, SyncInterval, Flags);
        }
        UE4Manager::SetViewTarget();
        ID3D11Texture2D* texture;
        pSwapChain->GetBuffer(0, IID_PPV_ARGS(&texture));
        VRManager::SubmitFrame(texture);
        texture->Release();
        if (VRManager::LastEyeShown == Eye::Right)
            return PresentOriginal(pSwapChain, SyncInterval, Flags);
        return S_OK;
    }

    bool D3D11Manager::ConvertFrame(ID3D11Texture2D* source, ID3D11RenderTargetView* rtv)
    {
        ID3D11Device* device;
        rtv->GetDevice(&device);
        ID3D11DeviceContext* context;
        device->GetImmediateContext(&context);
        D3D11_TEXTURE2D_DESC textureDesc;
        source->GetDesc(&textureDesc);
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
        rtv->GetDesc(&rtvDesc);
        if (!convertResourcesCreated)
        {
            HRESULT hr = device->CreateVertexShader(VertexShader, ARRAYSIZE(VertexShader), nullptr, &vertexShader);
            CHECK_HR(hr, "Couldn't create vertex shader")
            hr = device->CreatePixelShader(PixelShader, ARRAYSIZE(PixelShader), nullptr, &pixelShader);
            CHECK_HR(hr, "Couldn't create pixel shader")
            textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            hr = device->CreateTexture2D(&textureDesc, nullptr, &copy);
            CHECK_HR(hr, "Couldn't create copy texture")
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
            srvDesc.Format = textureDesc.Format;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;
            srvDesc.Texture2D.MostDetailedMip = 0;
            hr = device->CreateShaderResourceView(copy, &srvDesc, &srv);
            CHECK_HR(hr, "Couldn't create shader resource view")
            convertResourcesCreated = true;
        }
        context->CopyResource(copy, source);
        context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
        context->IASetIndexBuffer(nullptr, static_cast<DXGI_FORMAT>(0), 0);
        context->IASetInputLayout(nullptr);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->VSSetShader(vertexShader, nullptr, 0);
        context->PSSetShader(pixelShader, nullptr, 0);
        context->OMSetRenderTargets(1, &rtv, nullptr);
        context->PSSetShaderResources(0, 1, &srv);
        context->Draw(3, 0);
        context->Release();
        device->Release();
        return true;
    }

    void D3D11Manager::Stop()
    {
        if (convertResourcesCreated)
        {
            srv->Release();
            copy->Release();
        }
    }
}
