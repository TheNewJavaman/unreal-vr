#include "VRManager.h"

#include <algorithm>
#include <d3d11.h>
#include <format>
#include <vector>
#include <Utilities/Logger.h>

namespace UnrealVR
{
    bool VRManager::Init()
    {
        if (!GetInstanceWithExtensions())
        {
            Log::Error("Could not get an OpenXR instance");
            return false;
        }
        return true;
    }

    bool VRManager::GetInstanceWithExtensions()
    {
        std::vector<const char*> enabledExtensions;
        const char* requiredExtensions[] = {
            XR_KHR_D3D11_ENABLE_EXTENSION_NAME,
            XR_EXT_DEBUG_UTILS_EXTENSION_NAME
        };
        uint32_t extensionCount = 0;
        xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr);
        std::vector<XrExtensionProperties> allExtensions(extensionCount, {XR_TYPE_EXTENSION_PROPERTIES});
        xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, allExtensions.data());
        Log::Info("[UnrealVR] OpenXR extensions available:");
        for (size_t i = 0; i < allExtensions.size(); i++)
        {
            Log::Info(std::format("[UnrealVR] - {}", allExtensions[i].extensionName) + "");
            for (int32_t j = 0; j < _countof(requiredExtensions); j++)
            {
                if (strcmp(requiredExtensions[j], allExtensions[i].extensionName) == 0)
                {
                    enabledExtensions.push_back(requiredExtensions[j]);
                    break;
                }
            }
        }
        if (!std::ranges::any_of(enabledExtensions,
                                 [](const char* extension)
                                 {
                                     return strcmp(extension, XR_KHR_D3D11_ENABLE_EXTENSION_NAME) == 0;
                                 }
        ))
        {
            Log::Error("[UnrealVR] The active OpenXR runtime does not support D3D11");
            return false;
        }
        XrInstanceCreateInfo createInfo = {XR_TYPE_INSTANCE_CREATE_INFO};
        createInfo.enabledExtensionCount = enabledExtensions.size();
        createInfo.enabledExtensionNames = enabledExtensions.data();
        createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
        strcpy_s(createInfo.applicationInfo.applicationName, applicationName);
        xrCreateInstance(&createInfo, &xrInstance);
        if (xrInstance == nullptr)
        {
            Log::Error("[UnrealVR] Failed to create OpenXR instance");
            return false;
        }
        return true;
    }

    void VRManager::LoadDebugExtension()
    {
        xrGetInstanceProcAddr(xrInstance, "xrCreateDebugUtilsMessengerEXT",
                              reinterpret_cast<PFN_xrVoidFunction*>(&xrExtCreateDebugUtilsMessenger));
        xrGetInstanceProcAddr(xrInstance, "xrDestroyDebugUtilsMessengerEXT",
                              reinterpret_cast<PFN_xrVoidFunction*>(&xrExtDestroyDebugUtilsMessenger));
        xrGetInstanceProcAddr(xrInstance, "xrGetD3D11GraphicsRequirementsKHR",
                              reinterpret_cast<PFN_xrVoidFunction*>(&xrExtGetD3D11GraphicsRequirements));
        XrDebugUtilsMessengerCreateInfoEXT debugInfo = {XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
        debugInfo.messageTypes =
            XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
            XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;
        debugInfo.messageSeverities =
            XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugInfo.userCallback = [](XrDebugUtilsMessageSeverityFlagsEXT severity, XrDebugUtilsMessageTypeFlagsEXT types,
                                    const XrDebugUtilsMessengerCallbackDataEXT* msg, void* user_data)
        {
            Log::Info(std::format("[UnrealVR] OpenXR | {}: {}", msg->functionName, msg->message) + "");
            return static_cast<XrBool32>(XR_FALSE);
        };
        if (xrExtCreateDebugUtilsMessenger)
        {
            xrExtCreateDebugUtilsMessenger(xrInstance, &debugInfo, &xrDebug);
        }
        Log::Info("[UnrealVR] Loaded OpenXR debug extension");
    }

    void VRManager::LoadD3D11Extension()
    {
        XrSystemGetInfo systemInfo = {XR_TYPE_SYSTEM_GET_INFO};
        systemInfo.formFactor = formFactor;
        xrGetSystem(xrInstance, &systemInfo, &xrSystemId);
        uint32_t blendCount = 0;
        xrEnumerateEnvironmentBlendModes(xrInstance, xrSystemId, viewType, 1, &blendCount, &xrBlend);
        XrGraphicsRequirementsD3D11KHR graphicsRequirements = {XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR};
        xrExtGetD3D11GraphicsRequirements(xrInstance, xrSystemId, &graphicsRequirements);
        XrGraphicsBindingD3D11KHR graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_D3D11_KHR};
        Log::Info("[UnrealVR] Partially loaded OpenXR D3D11 extension");
    }

    bool VRManager::ContinueInitAndCreateSwapChain()
    {
        Log::Info("[UnrealVR] Finished loading OpenXR D3D11 extension");
        return true;
    }

    void VRManager::Stop()
    {
        if (xrInstance != XR_NULL_HANDLE && xrDestroyInstance(xrInstance) != XR_SUCCESS)
        {
            Log::Error("[UnrealVR] Failed to destroy OpenXR instance");
        }
        if (xrDebug != XR_NULL_HANDLE && xrExtDestroyDebugUtilsMessenger(xrDebug) != XR_SUCCESS)
        {
            Log::Error("[UnrealVR] Failed to destroy OpenXR debug");
        }
    }
}
