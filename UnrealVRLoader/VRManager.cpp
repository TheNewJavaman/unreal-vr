#include "VRManager.h"

#include <algorithm>
#include <format>
#include <Utilities/Logger.h>

#include "UE4Manager.h"

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
        LoadDebugExtension();
        LoadD3D11Extension();
        return true;
    }

    void VRManager::LoadDebugExtension()
    {
        xrGetInstanceProcAddr(xrInstance, "xrCreateDebugUtilsMessengerEXT",
                              reinterpret_cast<PFN_xrVoidFunction*>(&xrExtCreateDebugUtilsMessenger));
        xrGetInstanceProcAddr(xrInstance, "xrDestroyDebugUtilsMessengerEXT",
                              reinterpret_cast<PFN_xrVoidFunction*>(&xrExtDestroyDebugUtilsMessenger));
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
        xrGetInstanceProcAddr(xrInstance, "xrGetD3D11GraphicsRequirementsKHR",
                              reinterpret_cast<PFN_xrVoidFunction*>(&xrExtGetD3D11GraphicsRequirements));
        XrSystemGetInfo systemInfo = {XR_TYPE_SYSTEM_GET_INFO};
        systemInfo.formFactor = xrFormFactor;
        xrGetSystem(xrInstance, &systemInfo, &xrSystemId);
        uint32_t blendCount = 0;
        xrEnumerateEnvironmentBlendModes(xrInstance, xrSystemId, xrViewType, 1, &blendCount, &xrBlend);
        XrGraphicsRequirementsD3D11KHR graphicsRequirements = {XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR};
        xrExtGetD3D11GraphicsRequirements(xrInstance, xrSystemId, &graphicsRequirements);
        Log::Info("[UnrealVR] Partially loaded OpenXR D3D11 extension");
    }

    bool VRManager::ContinueInit(ID3D11Device* device)
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
        ContinueInitDone = true;
        Log::Info("[UnrealVR] Finished initializing OpenXR");
        return true;
    }

    void VRManager::GetRecommendedResolution(uint32_t* width, uint32_t* height)
    {
        xrEnumerateViewConfigurationViews(xrInstance, xrSystemId, xrViewType, 0, &xrViewCount, nullptr);
        configViews.resize(xrViewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
        xrViews.resize(xrViewCount, {XR_TYPE_VIEW});
        xrEnumerateViewConfigurationViews(xrInstance, xrSystemId, xrViewType,
                                          xrViewCount, &xrViewCount, configViews.data());
        xrWidth = configViews[0].recommendedImageRectWidth;
        xrHeight = configViews[0].recommendedImageRectHeight;
        *width = xrWidth;
        *height = xrHeight;
        Log::Info("[UnrealVR] Obtained recommended VR resolution");
    }

    void VRManager::CreateSwapChains(DXGI_FORMAT format, uint32_t sampleCount)
    {
        for (uint32_t i = 0; i < xrViewCount; i++)
        {
            XrSwapchainCreateInfo swapchainInfo = {XR_TYPE_SWAPCHAIN_CREATE_INFO};
            XrSwapchain swapchain;
            swapchainInfo.arraySize = 1;
            swapchainInfo.mipCount = 1;
            swapchainInfo.faceCount = 1;
            swapchainInfo.format = format;
            swapchainInfo.width = configViews[0].recommendedImageRectWidth;
            swapchainInfo.height = configViews[0].recommendedImageRectHeight;
            swapchainInfo.sampleCount = sampleCount;
            swapchainInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
            xrCreateSwapchain(xrSession, &swapchainInfo, &swapchain);
            uint32_t surfaceCount = 0;
            xrEnumerateSwapchainImages(swapchain, 0, &surfaceCount, nullptr);
            xrSwapChains.push_back(swapchain);
            std::vector<XrSwapchainImageD3D11KHR> surfaces;
            surfaces.resize(surfaceCount);
            xrEnumerateSwapchainImages(swapchain, surfaceCount, &surfaceCount,
                                       reinterpret_cast<XrSwapchainImageBaseHeader*>(surfaces.data()));
        }
        UE4Manager::AddRelativeLocation({0.0f, -3.175f, 0.0f});
        CreateSwapChainsDone = true;
        Log::Info("[UnrealVR] Created OpenXR swapchains");
    }

    void VRManager::SubmitFrame(ID3D11Texture2D* texture)
    {
        uint32_t imageId;
        XrSwapchainImageAcquireInfo acquireInfo = {XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
        XrSwapchainImageWaitInfo waitInfo = {XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        waitInfo.timeout = XR_INFINITE_DURATION;
        ID3D11Device* device;
        texture->GetDevice(&device);
        ID3D11DeviceContext* context;
        device->GetImmediateContext(&context);
        if (lastEyeShown == Eye::Right)
        {
            xrFrameState = {XR_TYPE_FRAME_STATE};
            xrWaitFrame(xrSession, nullptr, &xrFrameState);
            xrBeginFrame(xrSession, nullptr);
            xrLayerProj = {XR_TYPE_COMPOSITION_LAYER_PROJECTION};
            std::vector<XrCompositionLayerProjectionView> views;
            uint32_t viewCount = 0;
            XrViewState viewState = {XR_TYPE_VIEW_STATE};
            XrViewLocateInfo locateInfo = {XR_TYPE_VIEW_LOCATE_INFO};
            locateInfo.viewConfigurationType = xrViewType;
            locateInfo.displayTime = xrFrameState.predictedDisplayTime;
            locateInfo.space = xrAppSpace;
            xrLocateViews(xrSession, &locateInfo, &viewState,
                          static_cast<uint32_t>(xrViews.size()),
                          &viewCount, xrViews.data());
            views.resize(viewCount);
            for (uint32_t i = 0; i < viewCount; i++)
            {
                views.at(i) = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
                views.at(i).pose = xrViews.at(i).pose;
                views.at(i).fov = xrViews.at(i).fov;
                views.at(i).subImage.swapchain = xrSwapChains.at(i);
                views.at(i).subImage.imageRect.offset = {0, 0};
                views.at(i).subImage.imageRect.extent = {static_cast<int32_t>(xrWidth), static_cast<int32_t>(xrHeight)};
            }
            xrLayerProj.space = xrAppSpace;
            xrLayerProj.viewCount = static_cast<uint32_t>(views.size());
            xrLayerProj.views = views.data();
            xrAcquireSwapchainImage(xrSwapChains.at(0), &acquireInfo, &imageId);
            xrWaitSwapchainImage(xrSwapChains.at(0), &waitInfo);
            context->CopyResource(xrSurfaces.at(0).at(imageId).texture, texture);
            lastEyeShown = Eye::Left;
            UE4Manager::AddRelativeLocation({0.0f, 6.35f, 0.0f});
        }
        else
        {
            xrAcquireSwapchainImage(xrSwapChains.at(1), &acquireInfo, &imageId);
            xrWaitSwapchainImage(xrSwapChains.at(1), &waitInfo);
            context->CopyResource(xrSurfaces.at(1).at(imageId).texture, texture);
            const auto xrLayer = reinterpret_cast<XrCompositionLayerBaseHeader*>(&xrLayerProj);
            XrFrameEndInfo endInfo = {XR_TYPE_FRAME_END_INFO};
            endInfo.displayTime = xrFrameState.predictedDisplayTime;
            endInfo.environmentBlendMode = xrBlend;
            endInfo.layerCount = xrLayer == nullptr ? 0 : 1;
            endInfo.layers = &xrLayer;
            xrEndFrame(xrSession, &endInfo);
            lastEyeShown = Eye::Right;
            UE4Manager::AddRelativeLocation({0.0f, -6.35f, 0.0f});
        }
    }

    void VRManager::Stop()
    {
        for (uint32_t i = 0; i < xrViewCount; i++)
        {
            if (xrSwapChains.at(i) != XR_NULL_HANDLE && xrDestroySwapchain(xrSwapChains.at(i)))
            {
                Log::Error("[UnrealVR] Failed to destroy OpenXR swapchain");
            }
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
