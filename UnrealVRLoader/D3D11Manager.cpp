#include "D3D11Manager.h"

namespace UnrealVR
{
	DWORD __stdcall AddHooksThread(LPVOID)
	{
		IDXGIFactory* factory;
		CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		IDXGIFactory2* factory2;
		factory->QueryInterface<IDXGIFactory2>(&factory2);
		DWORD_PTR* factory2VTable = (DWORD_PTR*)((DWORD_PTR*)factory2)[0];



		return S_OK;
	}

	bool D3D11Manager::AddHooks()
	{
		CreateThread(NULL, 0, AddHooksThread, NULL, 0, NULL);
		return true;
	}
}