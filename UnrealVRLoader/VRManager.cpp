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
        systemInfo.formFactor = xrFormFactor;
        xrGetSystem(xrInstance, &systemInfo, &xrSystemId);
        uint32_t blendCount = 0;
        xrEnumerateEnvironmentBlendModes(xrInstance, xrSystemId, xrViewType, 1, &blendCount, &xrBlend);
        XrGraphicsRequirementsD3D11KHR graphicsRequirements = {XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR};
        xrExtGetD3D11GraphicsRequirements(xrInstance, xrSystemId, &graphicsRequirements);
        Log::Info("[UnrealVR] Partially loaded OpenXR D3D11 extension");
    }

    bool VRManager::ContinueInitAndCreateSwapChain(ID3D11Device* device, DXGI_SWAP_CHAIN_DESC* swapChainDesc)
    {
        graphicsBinding.device = device;
        XrSessionCreateInfo sessionInfo = {XR_TYPE_SESSION_CREATE_INFO};
        sessionInfo.next = &graphicsBinding;
        sessionInfo.systemId = xrSystemId;
        xrCreateSession(xrInstance, &sessionInfo, &xrSession);
        if (xrSession == nullptr)
        {
            Log::Error("[UnrealVR] Failed to create OpenXR session");
            return false;
        }
        XrReferenceSpaceCreateInfo spaceInfo = {XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
        spaceInfo.poseInReferenceSpace = xrPoseIdentity;
        spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
        xrCreateReferenceSpace(xrSession, &spaceInfo, &xrAppSpace);
        uint32_t viewCount = 0;
        xrEnumerateViewConfigurationViews(xrInstance, xrSystemId, xrViewType, 0, &viewCount, nullptr);
        std::vector<XrViewConfigurationView> configViews;
        configViews.resize(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
        xrViews.resize(viewCount, {XR_TYPE_VIEW});
        xrEnumerateViewConfigurationViews(xrInstance, xrSystemId, xrViewType,
                                          viewCount, &viewCount, configViews.data());
        XrSwapchainCreateInfo swapchainInfo = {XR_TYPE_SWAPCHAIN_CREATE_INFO};
        swapchainInfo.arraySize = 1;
        swapchainInfo.mipCount = 1;
        swapchainInfo.faceCount = 1;
        swapchainInfo.format = swapChainDesc->BufferDesc.Format;
        swapchainInfo.width = configViews[0].recommendedImageRectWidth;
        swapchainInfo.height = configViews[0].recommendedImageRectHeight;
        swapchainInfo.sampleCount = swapChainDesc->SampleDesc.Count;
        swapchainInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
        xrCreateSwapchain(xrSession, &swapchainInfo, &xrSwapchain);
        StartFrame();
        Log::Info("[UnrealVR] Finished loading OpenXR D3D11 extension");
        return true;
    }

    void VRManager::SubmitFrame()
    {
        EndFrame();
        StartFrame();
    }

    void VRManager::StartFrame()
    {
        xrFrameState = {XR_TYPE_FRAME_STATE};
        xrWaitFrame(xrSession, nullptr, &xrFrameState);
        xrBeginFrame(xrSession, nullptr);
        xrLayer = nullptr;
        xrLayerProj = {XR_TYPE_COMPOSITION_LAYER_PROJECTION};
        std::vector<XrCompositionLayerProjectionView> views;
    }

    void VRManager::EndFrame()
    {
        xrLayer = reinterpret_cast<XrCompositionLayerBaseHeader*>(&xrLayerProj);
        XrFrameEndInfo endInfo = {XR_TYPE_FRAME_END_INFO};
        endInfo.displayTime = xrFrameState.predictedDisplayTime;
        endInfo.environmentBlendMode = xrBlend;
        endInfo.layerCount = xrLayer == nullptr ? 0 : 1;
        endInfo.layers = &xrLayer;
        xrEndFrame(xrSession, &endInfo);
    }

    void VRManager::Stop()
    {
        if (xrSwapchain != XR_NULL_HANDLE && xrDestroySwapchain(xrSwapchain))
        {
            Log::Error("[UnrealVR] Failed to destroy OpenXR swapchain");
        }
        if (xrAppSpace != XR_NULL_HANDLE && xrDestroySpace(xrAppSpace) != XR_SUCCESS)
        {
            Log::Error("[UnrealVR] Failed to destroy OpenXR app space");
        }
        if (xrSession != XR_NULL_HANDLE && xrDestroySession(xrSession) != XR_SUCCESS)
        {
            Log::Error("[UnrealVR] Failed to destroy OpenXR session");
        }
        if (xrDebug != XR_NULL_HANDLE && xrExtDestroyDebugUtilsMessenger(xrDebug) != XR_SUCCESS)
        {
            Log::Error("[UnrealVR] Failed to destroy OpenXR debug");
        }
        if (xrInstance != XR_NULL_HANDLE && xrDestroyInstance(xrInstance) != XR_SUCCESS)
        {
            Log::Error("[UnrealVR] Failed to destroy OpenXR instance");
        }
    }
}
