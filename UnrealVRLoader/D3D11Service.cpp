#include "D3D11Service.h"

#include <Utilities/Logger.h>

#include "HookHelper.h"
#include "PixelShader.h"
#include "UE4Service.h"
#include "VertexShader.h"
#include "OpenXRService.h"

#define VTABLE(instance) reinterpret_cast<DWORD_PTR*>(reinterpret_cast<DWORD_PTR*>(instance)[0]);
#define CHECK_HR(hr, message) \
    if ((hr) != S_OK) \
    { \
        Log::Error("[UnrealVR] %s; error code (%X)", message, hr); \
        return false; \
    }

namespace UnrealVR {
    class OpenXRService;

    void D3D11Service::AddHooks() {
        CreateThread(nullptr, 0, AddHooksThread, nullptr, 0, nullptr);
    }

    DWORD __stdcall D3D11Service::AddHooksThread(LPVOID) {
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
        const auto hWnd = CreateWindowA("DX", NULL, WS_OVERLAPPEDWINDOW,
                                        100, 100, 300, 300,
                                        NULL, NULL, wc.hInstance, NULL);
        constexpr D3D_FEATURE_LEVEL requestedLevels[] = { D3D_FEATURE_LEVEL_11_0 };
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
        HookHelper::Add<PresentFunc>(PresentTarget, &PresentDetour, &PresentOriginal, "Present");
        DWORD presentOld;
        VirtualProtect(PresentTarget, 2, PAGE_EXECUTE_READWRITE, &presentOld);

        context->Release();
        device->Release();
        swapChain->Release();

        return NULL;
    }

    HRESULT __stdcall D3D11Service::PresentDetour(
        IDXGISwapChain* pSwapChain,
        UINT SyncInterval,
        UINT Flags
    ) {
        if (!OpenXRService::VRLoaded) {
            ID3D11Device* device;
            pSwapChain->GetDevice(IID_PPV_ARGS(&device));
            DXGI_SWAP_CHAIN_DESC desc;
            pSwapChain->GetDesc(&desc);
            if (!OpenXRService::FinishInit(device, desc.SampleDesc.Count)) {
                return PresentOriginal(pSwapChain, SyncInterval, Flags);
            }
            device->Release();
            return PresentOriginal(pSwapChain, SyncInterval, Flags);
        }
        UE4Service::SetViewTarget();
        if (!UE4Service::Resized) {
            return PresentOriginal(pSwapChain, SyncInterval, Flags);
        }
        ID3D11Texture2D* texture;
        pSwapChain->GetBuffer(0, IID_PPV_ARGS(&texture));
        OpenXRService::SubmitFrame(texture);
        texture->Release();
        if (OpenXRService::LastEyeShown == Eye::Left) {
            return PresentOriginal(pSwapChain, SyncInterval, Flags);
        }
        return S_OK;
    }

    bool D3D11Service::ConvertFrame(ID3D11Texture2D* source, ID3D11RenderTargetView* rtv) {
        HRESULT hr;
        ID3D11Device* device;
        rtv->GetDevice(&device);
        ID3D11DeviceContext* context;
        device->GetImmediateContext(&context);

        // Create shaders
        if (!shadersCreated) {
            hr = device->CreateVertexShader(
                VertexShader,
                ARRAYSIZE(VertexShader),
                nullptr,
                &vertexShader
            );
            CHECK_HR(hr, "Couldn't create vertex shader")
            hr = device->CreatePixelShader(
                PixelShader,
                ARRAYSIZE(PixelShader),
                nullptr,
                &pixelShader
            );
            CHECK_HR(hr, "Couldn't create pixel shader")
            shadersCreated = true;
        }

        // If the swapchain settings changed, invalidate previous shader resource view
        bool shouldCreateSRV = false;
        if (srv == nullptr) {
            shouldCreateSRV = true;
        } else {
            D3D11_TEXTURE2D_DESC sourceDesc;
            source->GetDesc(&sourceDesc);
            D3D11_TEXTURE2D_DESC copyDesc;
            copy->GetDesc(&copyDesc);
            if (copyDesc.Format != sourceDesc.Format
                || copyDesc.Width != sourceDesc.Width
                || copyDesc.Height != sourceDesc.Height) {
                shouldCreateSRV = true;
                srv->Release();
                copy->Release();
            }
        }

        // Create a new shader resource view if necessary
        if (shouldCreateSRV) {
            D3D11_TEXTURE2D_DESC sourceDesc;
            source->GetDesc(&sourceDesc);
            sourceDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            hr = device->CreateTexture2D(&sourceDesc, nullptr, &copy);
            CHECK_HR(hr, "Couldn't create copy texture")
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
            srvDesc.Format = sourceDesc.Format;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;
            srvDesc.Texture2D.MostDetailedMip = 0;
            hr = device->CreateShaderResourceView(copy, &srvDesc, &srv);
            CHECK_HR(hr, "Couldn't create shader resource view")
        }

        // Draw the frame
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

    void D3D11Service::Stop() {
        if (pixelShader != nullptr) {
            pixelShader->Release();
        }
        if (vertexShader != nullptr) {
            vertexShader->Release();
        }
        if (srv != nullptr) {
            srv->Release();
        }
        if (copy != nullptr) {
            copy->Release();
        }
    }
}