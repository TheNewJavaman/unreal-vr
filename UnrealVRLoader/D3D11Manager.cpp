#include "D3D11Manager.h"

namespace UnrealVR
{
	namespace D3D11Manager
	{
		DWORD __stdcall AddHooksThread(LPVOID);

		typedef HRESULT(__stdcall CreateSwapChainFunc)(
			IDXGIFactory* pFactory,
			IUnknown* pDevice,
			DXGI_SWAP_CHAIN_DESC* pDesc,
			IDXGISwapChain** ppSwapChain
			);
		CreateSwapChainFunc CreateSwapChainDetour;
		CreateSwapChainFunc* CreateSwapChainTarget = nullptr;
		CreateSwapChainFunc* CreateSwapChainOriginal = nullptr;

		typedef HRESULT(__stdcall CreateSwapChainForHwndFunc)(
			IDXGIFactory2* pFactory,
			IUnknown* pDevice,
			HWND hWnd,
			DXGI_SWAP_CHAIN_DESC1* pDesc,
			DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
			IDXGIOutput* pRestrictToOutput,
			IDXGISwapChain1** ppSwapChain
			);
		CreateSwapChainForHwndFunc CreateSwapChainForHwndDetour;
		CreateSwapChainForHwndFunc* CreateSwapChainForHwndTarget = nullptr;
		CreateSwapChainForHwndFunc* CreateSwapChainForHwndOriginal = nullptr;

		void AddHooks()
		{
			CreateThread(NULL, 0, AddHooksThread, NULL, 0, NULL);
		}

		DWORD __stdcall AddHooksThread(LPVOID)
		{
			IDXGIFactory* factory;
			CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
			IDXGIFactory2* factory2;
			factory->QueryInterface<IDXGIFactory2>(&factory2);
			DWORD_PTR* factory2VTable = (DWORD_PTR*)((DWORD_PTR*)factory2)[0];

			CreateSwapChainTarget = (CreateSwapChainFunc*)factory2VTable[10];
			CreateSwapChainForHwndTarget = (CreateSwapChainForHwndFunc*)factory2VTable[15];
			HookManager::Add<CreateSwapChainFunc>(CreateSwapChainTarget, &CreateSwapChainDetour, &CreateSwapChainOriginal, "CreateSwapChain");
			HookManager::Add<CreateSwapChainForHwndFunc>(CreateSwapChainForHwndTarget, &CreateSwapChainForHwndDetour, &CreateSwapChainForHwndOriginal, "CreateSwapChainForHwnd");
			DWORD createSwapChainOld;
			DWORD createSwapChainForHwndOld;
			VirtualProtect(CreateSwapChainTarget, 2, PAGE_EXECUTE_READWRITE, &createSwapChainOld);
			VirtualProtect(CreateSwapChainForHwndTarget, 2, PAGE_EXECUTE_READWRITE, &createSwapChainForHwndOld);

			Log::Info("Added D3D11 hooks");

			while (true)
			{
				Sleep(10);
			}

			factory2->Release();
			factory->Release();

			return NULL;
		}

		HRESULT __stdcall CreateSwapChainDetour(
			IDXGIFactory* pFactory,
			IUnknown* pDevice,
			DXGI_SWAP_CHAIN_DESC* pDesc,
			IDXGISwapChain** ppSwapChain
		)
		{
			CreateSwapChainOriginal(pFactory, pDevice, pDesc, ppSwapChain);
			Log::Info("Intercepted CreateSwapChain");
			return S_OK;
		}

		HRESULT __stdcall CreateSwapChainForHwndDetour(
			IDXGIFactory2* pFactory,
			IUnknown* pDevice,
			HWND hWnd,
			DXGI_SWAP_CHAIN_DESC1* pDesc,
			DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
			IDXGIOutput* pRestrictToOutput,
			IDXGISwapChain1** ppSwapChain
		)
		{
			CreateSwapChainForHwndOriginal(pFactory, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
			Log::Info("Intercepted CreateSwapChainForHwnd");
			return S_OK;
		}
	}
}