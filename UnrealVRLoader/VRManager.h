#pragma once

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11

#include <algorithm>
#include <vector>
#include <d3d11.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <format>
#include "Utilities/Logger.h"
#include "Defs.h"

namespace UnrealVR
{
	namespace VRManager
	{
		bool Init();
		UNREALVR_API bool ContinueInitAndCreateSwapChain();
		void Stop();
	};
}