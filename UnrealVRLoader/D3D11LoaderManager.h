#pragma once

#include <d3d11.h>
#include <dxgi1_6.h>
#include "Logger.h"
#include "HookManager.h"

namespace UnrealVR
{
	namespace D3D11LoaderManager
	{
		void AddHooks();
		extern bool IsHooked;
	};
}