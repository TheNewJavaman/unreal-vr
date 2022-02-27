#pragma once

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11

#include <vector>
#include <d3d11.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <format>
#include "Logger.h"

namespace UnrealVR
{
	namespace VRLoaderManager
	{
		bool Init();
		void Stop();
	};
}