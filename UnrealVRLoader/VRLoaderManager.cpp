#include "VRLoaderManager.h"

namespace UnrealVR
{
	namespace VRLoaderManager
	{
		bool GetInstanceWithExtensions();

		const char* applicationName = "UnrealVR";
		XrFormFactor formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		XrViewConfigurationType viewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
		XrInstance xrInstance = {};

		bool Init()
		{
			if (!GetInstanceWithExtensions())
			{
				Logger::Error(L"Could not get an OpenXR instance");
				return false;
			}
			return true;
		}

		bool GetInstanceWithExtensions()
		{
			std::vector<const char*> enabledExtensions;
			const char* requiredExtensions[] = {
				XR_KHR_D3D11_ENABLE_EXTENSION_NAME,
				XR_EXT_DEBUG_UTILS_EXTENSION_NAME
			};
			uint32_t extensionCount = 0;
			xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr);
			std::vector<XrExtensionProperties> allExtensions(extensionCount, { XR_TYPE_EXTENSION_PROPERTIES });
			xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, allExtensions.data());
			Logger::Info(L"OpenXR extensions available:");
			for (size_t i = 0; i < allExtensions.size(); i++)
			{
				std::string extensionName(allExtensions[i].extensionName);
				Logger::Info(std::format(L"- {}", std::wstring(extensionName.begin(), extensionName.end())));
				for (int32_t j = 0; j < _countof(requiredExtensions); j++)
				{
					if (strcmp(requiredExtensions[j], allExtensions[i].extensionName) == 0)
					{
						enabledExtensions.push_back(requiredExtensions[j]);
						break;
					}
				}
			}
			XrInstanceCreateInfo createInfo = { XR_TYPE_INSTANCE_CREATE_INFO };
			createInfo.enabledExtensionCount = enabledExtensions.size();
			createInfo.enabledExtensionNames = enabledExtensions.data();
			createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
			strcpy_s(createInfo.applicationInfo.applicationName, applicationName);
			xrCreateInstance(&createInfo, &xrInstance);
			if (xrInstance == nullptr)
			{
				Logger::Error(L"No OpenXR runtime found!");
				return false;
			}
			return true;
		}

		void Stop()
		{
			if (xrInstance != XR_NULL_HANDLE && xrDestroyInstance(xrInstance) != XR_SUCCESS)
			{
				Logger::Error(L"Failed to destroy XR instance");
			}
		}
	}
}