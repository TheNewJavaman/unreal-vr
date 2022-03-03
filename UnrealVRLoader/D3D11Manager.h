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
	};
}
