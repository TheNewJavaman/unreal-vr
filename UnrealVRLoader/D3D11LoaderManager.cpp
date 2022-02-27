#include "D3D11LoaderManager.h"

namespace UnrealVR
{
	namespace D3D11LoaderManager
	{
		DWORD __stdcall AddHooksThread(LPVOID);

		bool IsHooked = false;

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

		void D3D11LoaderManager::AddHooks()
		{
			if (!IsHooked)
			{
				CreateThread(NULL, 0, AddHooksThread, NULL, 0, NULL);
			}
		}

		DWORD __stdcall D3D11LoaderManager::AddHooksThread(LPVOID)
		{
			IDXGIFactory* factory;
			CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
			IDXGIFactory2* factory2;
			factory->QueryInterface<IDXGIFactory2>(&factory2);
			DWORD_PTR* factory2VTable = (DWORD_PTR*)((DWORD_PTR*)factory2)[0];

			CreateSwapChainTarget = (CreateSwapChainFunc*)factory2VTable[10];
			CreateSwapChainForHwndTarget = (CreateSwapChainForHwndFunc*)factory2VTable[15];
			HookManager::Add<CreateSwapChainFunc>(CreateSwapChainTarget, &CreateSwapChainDetour, &CreateSwapChainOriginal, L"CreateSwapChain");
			HookManager::Add<CreateSwapChainForHwndFunc>(CreateSwapChainForHwndTarget, &CreateSwapChainForHwndDetour, &CreateSwapChainForHwndOriginal, L"CreateSwapChainForHwnd");
			DWORD createSwapChainOld;
			DWORD createSwapChainForHwndOld;
			VirtualProtect(CreateSwapChainTarget, 2, PAGE_EXECUTE_READWRITE, &createSwapChainOld);
			VirtualProtect(CreateSwapChainForHwndTarget, 2, PAGE_EXECUTE_READWRITE, &createSwapChainForHwndOld);

			Logger::Info(L"Added D3D11 hooks");
			IsHooked = true;

			while (true)
			{
				Sleep(10);
			}

			factory2->Release();
			factory->Release();

			return NULL;
		}

		HRESULT __stdcall D3D11LoaderManager::CreateSwapChainDetour(
			IDXGIFactory* pFactory,
			IUnknown* pDevice,
			DXGI_SWAP_CHAIN_DESC* pDesc,
			IDXGISwapChain** ppSwapChain
		)
		{
			CreateSwapChainOriginal(pFactory, pDevice, pDesc, ppSwapChain);
			Logger::Info(L"Intercepted CreateSwapChain");
			return S_OK;
		}

		HRESULT __stdcall D3D11LoaderManager::CreateSwapChainForHwndDetour(
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
			Logger::Info(L"Intercepted CreateSwapChainForHwnd");
			return S_OK;
		}
	}
}