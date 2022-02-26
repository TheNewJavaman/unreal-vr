#pragma once

#include <d3d11.h>
#include <dxgi1_6.h>
#include "Logger.h"
#include "HookManager.h"

namespace UnrealVR
{
	class D3D11LoaderManager
	{
	public:
		static void AddHooks();
		static bool IsHooked;

	private:
		static DWORD __stdcall AddHooksThread(LPVOID);

		typedef HRESULT(__stdcall CreateSwapChainFunc)(
			IDXGIFactory* pFactory,
			IUnknown* pDevice,
			DXGI_SWAP_CHAIN_DESC* pDesc,
			IDXGISwapChain** ppSwapChain
			);
		static CreateSwapChainFunc CreateSwapChainDetour;
		static CreateSwapChainFunc* CreateSwapChainTarget;
		static CreateSwapChainFunc* CreateSwapChainOriginal;

		typedef HRESULT(__stdcall* CreateSwapChainForHwndFunc)(
			IDXGIFactory2* pFactory,
			IUnknown* pDevice,
			HWND hWnd,
			DXGI_SWAP_CHAIN_DESC1* pDesc,
			DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
			IDXGIOutput* pRestrictToOutput,
			IDXGISwapChain1** ppSwapChain
			);
		static CreateSwapChainForHwndFunc CreateSwapChainForHwndDetour;
		static CreateSwapChainForHwndFunc* CreateSwapChainForHwndTarget;
		static CreateSwapChainForHwndFunc* CreateSwapChainForHwndOriginal;
	};
}