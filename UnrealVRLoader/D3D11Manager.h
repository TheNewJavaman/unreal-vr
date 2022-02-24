#pragma once

#include <d3d11.h>
#include <dxgi1_6.h>

namespace UnrealVR
{
	typedef void(__stdcall* D3D11CreateSwapChainHook) (
		IDXGIAdapter* pAdapter,
		D3D_DRIVER_TYPE DriverType,
		HMODULE Software,
		UINT Flags,
		D3D_FEATURE_LEVEL* pFeatureLevels,
		UINT FeatureLevels,
		UINT SDKVersion,
		ID3D11Device** ppDevice,
		D3D_FEATURE_LEVEL* pFeatureLevel,
		ID3D11DeviceContext** ppImmediateContext
	);

	class D3D11Manager
	{
	public:
		static bool AddHooks();

	private:
		static D3D11CreateSwapChainHook pHookCreateSwapChain;
	};
}