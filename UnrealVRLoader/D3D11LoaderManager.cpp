#include "D3D11LoaderManager.h"

namespace UnrealVR
{
	//D3D11LoaderManager::CreateSwapChainFunc* D3D11LoaderManager::CreateSwapChainTarget;

	HRESULT __stdcall D3D11LoaderManager::CreateSwapChainDetour(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
	{
		return S_OK;
	}

	DWORD __stdcall D3D11LoaderManager::AddHooksThread(LPVOID)
	{
		IDXGIFactory* factory;
		CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		IDXGIFactory2* factory2;
		factory->QueryInterface<IDXGIFactory2>(&factory2);
		DWORD_PTR* factory2VTable = (DWORD_PTR*)((DWORD_PTR*)factory2)[0];
		Logger::Info(L"Created IDXGIFactory2 VTable");

		CreateSwapChainTarget = (CreateSwapChainFunc*)factory2VTable[10];
		CreateSwapChainForHwndTarget = (CreateSwapChainForHwndFunc*)factory2VTable[15];

		HookManager::Add((DWORD64)CreateSwapChainTarget, &CreateSwapChainDetour, &CreateSwapChainOriginal, "UnrealVR-D3D11-CreateSwapChain");

		return S_OK;
	}

	void D3D11LoaderManager::AddHooks()
	{
		CreateThread(NULL, 0, AddHooksThread, NULL, 0, NULL);
	}
}