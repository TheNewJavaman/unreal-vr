#pragma once

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11

#include <d3d11.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

namespace UnrealVR
{
	class VRManager
	{
	public:
		static bool Init();

		// TODO: Finish initializing OpenXR once we can send the swapchain back to Unreal Engine
		static bool ContinueInitAndCreateSwapChain();

		static void Stop();

	private:
		static bool GetInstanceWithExtensions();
		
		static void LoadDebugExtension();
		
		static void LoadD3D11Extension();

		inline static const char* applicationName = "UnrealVR";
		inline static XrFormFactor formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		inline static XrViewConfigurationType viewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
		inline static XrInstance xrInstance = {};

		inline static PFN_xrGetD3D11GraphicsRequirementsKHR xrExtGetD3D11GraphicsRequirements = nullptr;
		inline static PFN_xrCreateDebugUtilsMessengerEXT xrExtCreateDebugUtilsMessenger = nullptr;
		inline static PFN_xrDestroyDebugUtilsMessengerEXT xrExtDestroyDebugUtilsMessenger = nullptr;

		inline static XrDebugUtilsMessengerEXT xrDebug = {};
		inline static XrSystemId xrSystemId = XR_NULL_SYSTEM_ID;
		inline static XrEnvironmentBlendMode xrBlend = {};
	};
}
