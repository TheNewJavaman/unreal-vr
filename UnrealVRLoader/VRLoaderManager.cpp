#include "VRLoaderManager.h"

namespace UnrealVR
{
	namespace VRLoaderManager
	{
		const char* applicationName = "UnrealVR";
		XrFormFactor formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		XrViewConfigurationType viewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;

		XrInstance xrInstance;

		bool GetInstanceWithExtensions()
		{
			std::vector<const char*> enabledExtensions;
			const char* requestedExtensions[] = {
				XR_KHR_D3D11_ENABLE_EXTENSION_NAME,
				XR_EXT_DEBUG_UTILS_EXTENSION_NAME
			};
			uint32_t extCount = 0;
			xrEnumerateInstanceExtensionProperties(nullptr, 0, &extCount, nullptr);
			std::vector<XrExtensionProperties> allExtensions(extCount, { XR_TYPE_EXTENSION_PROPERTIES });
			Logger::Info(L"OpenXR extensions available:");
			for (size_t i = 0; i < allExtensions.size(); i++)
			{
				Logger::Info(std::format(L"- {}", allExtensions[i].extensionName));
				for (int32_t j = 0; j < _countof(requestedExtensions); j++)
				{
					if (strcmp(requestedExtensions[j], allExtensions[i].extensionName) == 0)
					{
						enabledExtensions.push_back(requestedExtensions[j]);
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

		bool Init()
		{
			if (!GetInstanceWithExtensions())
			{
				Logger::Error(L"Could not get an OpenXR instance");
				return false;
			}
			return true;
		}
	}
}