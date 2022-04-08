#include "VRManager.h"

#include <algorithm>
#include <format>
#include <Utilities/Logger.h>

#include "D3D11Manager.h"
#include "UE4Manager.h"

#define PI 3.141592653589793f
#define CHECK_XR(xr, message) \
    if ((xr) != XR_SUCCESS) \
    { \
        Log::Error( \
            std::format("[UnrealVR] {}; error code ({})", message, static_cast<int>(xr)) \
        ); \
        return false; \
    }

namespace UnrealVR
{
    bool VRManager::Init()
    {
        if (!GetInstanceWithExtensions())
        {
            Log::Error("[UnrealVR] Could not get an OpenXR instance");
            return false;
        }
        Log::Info("[UnrealVR] OpenXR initialized");
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
        XrResult xr = xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr);
        CHECK_XR(xr, "Could not query OpenXR extension count")
        std::vector<XrExtensionProperties> allExtensions(extensionCount, {XR_TYPE_EXTENSION_PROPERTIES});
        xr = xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, allExtensions.data());
        CHECK_XR(xr, "Could not query OpenXR extensions")
        Log::Info("[UnrealVR] Found (%d) OpenXR extensions:", extensionCount);
        for (auto& allExtension : allExtensions)
        {
            Log::Info(std::format("[UnrealVR] - {}", allExtension.extensionName) + "");
            for (auto& requiredExtension : requiredExtensions)
            {
                if (strcmp(requiredExtension, allExtension.extensionName) == 0)
                {
                    enabledExtensions.push_back(requiredExtension);
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
        createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
        createInfo.enabledExtensionNames = enabledExtensions.data();
        createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
        strcpy_s(createInfo.applicationInfo.applicationName, applicationName);
        xr = xrCreateInstance(&createInfo, &xrInstance);
        CHECK_XR(xr, "Could not create OpenXR instance")
        if (!LoadDebugExtension())
        {
            Log::Error("[UnrealVR] Could not load OpenXR debug extension");
            return false;
        }
        if (!LoadD3D11Extension())
        {
            Log::Error("[UnrealVR] Could not load OpenXR D3D11 extension");
            return false;
        }
        Log::Info("[UnrealVR] Obtained OpenXR instance");
        return true;
    }

    bool VRManager::LoadDebugExtension()
    {
        XrResult xr = xrGetInstanceProcAddr(xrInstance, "xrCreateDebugUtilsMessengerEXT",
                                            reinterpret_cast<PFN_xrVoidFunction*>(&xrExtCreateDebugUtilsMessenger));
        CHECK_XR(xr, "Could not get address for xrCreateDebugUtilsMessengerEXT")
        xr = xrGetInstanceProcAddr(xrInstance, "xrDestroyDebugUtilsMessengerEXT",
                                   reinterpret_cast<PFN_xrVoidFunction*>(&xrExtDestroyDebugUtilsMessenger));
        CHECK_XR(xr, "Could not get address for xrDestroyDebugUtilsMessengerEXT")
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
        xr = xrExtCreateDebugUtilsMessenger(xrInstance, &debugInfo, &xrDebug);
        CHECK_XR(xr, "Could not create OpenXR debug utils messenger")
        Log::Info("[UnrealVR] Loaded OpenXR debug extension");
        return true;
    }

    bool VRManager::LoadD3D11Extension()
    {
        XrResult xr = xrGetInstanceProcAddr(xrInstance, "xrGetD3D11GraphicsRequirementsKHR",
                                            reinterpret_cast<PFN_xrVoidFunction*>(&xrExtGetD3D11GraphicsRequirements));
        CHECK_XR(xr, "Could not get address for xrGetD3D11GraphicsRequirementsKHR")
        XrSystemGetInfo systemInfo = {XR_TYPE_SYSTEM_GET_INFO};
        systemInfo.formFactor = xrFormFactor;
        xr = xrGetSystem(xrInstance, &systemInfo, &xrSystemId);
        CHECK_XR(xr, "Could not get OpenXR system")
        uint32_t blendCount = 0;
        xr = xrEnumerateEnvironmentBlendModes(xrInstance, xrSystemId, xrViewType, 1, &blendCount, &xrBlend);
        CHECK_XR(xr, "Could not get OpenXR blend modes")
        XrGraphicsRequirementsD3D11KHR graphicsRequirements = {XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR};
        xr = xrExtGetD3D11GraphicsRequirements(xrInstance, xrSystemId, &graphicsRequirements);
        CHECK_XR(xr, "Could not get D3D11 graphics requirements")
        Log::Info("[UnrealVR] Partially loaded OpenXR D3D11 extension");
        return true;
    }

    bool VRManager::ContinueInit(ID3D11Device* device)
    {
        graphicsBinding.device = device;
        XrSessionCreateInfo sessionInfo = {XR_TYPE_SESSION_CREATE_INFO};
        sessionInfo.next = &graphicsBinding;
        sessionInfo.systemId = xrSystemId;
        XrResult xr = xrCreateSession(xrInstance, &sessionInfo, &xrSession);
        CHECK_XR(xr, "Could not create OpenXR session")
        XrReferenceSpaceCreateInfo spaceInfo = {XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
        spaceInfo.poseInReferenceSpace = xrPoseIdentity;
        spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        xr = xrCreateReferenceSpace(xrSession, &spaceInfo, &xrAppSpace);
        CHECK_XR(xr, "Could not create OpenXR reference space")
        Log::Info("[UnrealVR] Finished initializing OpenXR");
        return true;
    }

    bool VRManager::GetRecommendedResolution(uint32_t* width, uint32_t* height)
    {
        XrResult xr = xrEnumerateViewConfigurationViews(xrInstance, xrSystemId, xrViewType, 0, &xrViewCount, nullptr);
        CHECK_XR(xr, "Could not enumerate OpenXR view configuration view count")
        xrConfigViews.resize(xrViewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
        xrViews.resize(xrViewCount, {XR_TYPE_VIEW});
        xr = xrEnumerateViewConfigurationViews(xrInstance, xrSystemId, xrViewType,
                                               xrViewCount, &xrViewCount, xrConfigViews.data());
        CHECK_XR(xr, "Could not enumerate OpenXR view configuration views")
        xrWidth = xrConfigViews[0].recommendedImageRectWidth;
        xrHeight = xrConfigViews[0].recommendedImageRectHeight;
        *width = xrWidth;
        *height = xrHeight;
        Log::Info("[UnrealVR] Obtained recommended VR resolution");
        return true;
    }

    bool VRManager::CreateSwapChains(const uint32_t sampleCount)
    {
        uint32_t formatCount = 0;
        XrResult xr = xrEnumerateSwapchainFormats(xrSession, 0, &formatCount, nullptr);
        CHECK_XR(xr, "Could not enumerate OpenXR swapchain format count")
        std::vector<int64_t> formats(formatCount);
        xr = xrEnumerateSwapchainFormats(xrSession, formatCount, &formatCount, formats.data());
        CHECK_XR(xr, "Could not enumerate OpenXR swapchain formats")
        Log::Info(std::format("[UnrealVR] Found ({}) OpenXR swapchain formats:", formatCount));
        xrFormat = static_cast<DXGI_FORMAT>(formats.at(0));
        for (int64_t& format : formats)
        {
            Log::Info(std::format("[UnrealVR] - {}", format));
        }
        for (uint32_t i = 0; i < xrViewCount; i++)
        {
            XrSwapchainCreateInfo swapchainInfo = {XR_TYPE_SWAPCHAIN_CREATE_INFO};
            XrSwapchain swapchain;
            swapchainInfo.arraySize = 1;
            swapchainInfo.mipCount = 1;
            swapchainInfo.faceCount = 1;
            swapchainInfo.format = xrFormat;
            swapchainInfo.width = xrConfigViews[0].recommendedImageRectWidth;
            swapchainInfo.height = xrConfigViews[0].recommendedImageRectHeight;
            swapchainInfo.sampleCount = sampleCount;
            swapchainInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
            xr = xrCreateSwapchain(xrSession, &swapchainInfo, &swapchain);
            CHECK_XR(xr, "Could not create OpenXR swapchain")
            uint32_t surfaceCount = 0;
            xr = xrEnumerateSwapchainImages(swapchain, 0, &surfaceCount, nullptr);
            CHECK_XR(xr, "Could not enumerate OpenXR swapchain image count")
            xrSwapChains.push_back(swapchain);
            std::vector<XrSwapchainImageD3D11KHR> surfaces;
            surfaces.resize(surfaceCount, {XR_TYPE_SWAPCHAIN_IMAGE_D3D11_KHR});
            xr = xrEnumerateSwapchainImages(swapchain, surfaceCount, &surfaceCount,
                                            reinterpret_cast<XrSwapchainImageBaseHeader*>(surfaces.data()));
            CHECK_XR(xr, "Could not enumerate OpenXR swapchain images")
            std::vector<ID3D11RenderTargetView*> rtvs;
            for (const auto& surface : surfaces)
            {
                D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
                rtvDesc.Format = xrFormat;
                rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                rtvDesc.Texture2D.MipSlice = 0;
                ID3D11RenderTargetView* rtv;
                ID3D11Device* device;
                surface.texture->GetDevice(&device);
                HRESULT hr = device->CreateRenderTargetView(surface.texture, &rtvDesc, &rtv);
                if (hr != S_OK)
                {
                    Log::Error("[UnrealVR] Failed to create an RTV for OpenXR; error code (%X)", hr);
                    return false;
                }
                rtvs.push_back(rtv);
            }
            xrRTVs.insert(std::pair(i, rtvs));
        }
        Log::Info("[UnrealVR] Created OpenXR swapchains");
        return true;
    }

    bool VRManager::FinalizeInit(ID3D11Device* device, const DXGI_SWAP_CHAIN_DESC desc)
    {
        if (!ContinueInit(device)) return false;
        if (!CreateSwapChains(desc.SampleDesc.Count)) return false;
        XrSessionBeginInfo beginInfo = {XR_TYPE_SESSION_BEGIN_INFO};
        beginInfo.primaryViewConfigurationType = xrViewType;
        CHECK_XR(xrBeginSession(xrSession, &beginInfo), "Could not begin OpenXR session")
        Log::Info("[UnrealVR] Started OpenXR session");
        VRLoaded = true;
        return true;
    }

    bool VRManager::SubmitFrame(ID3D11Texture2D* texture)
    {
        uint32_t imageId;
        constexpr XrSwapchainImageAcquireInfo acquireInfo = {XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
        XrSwapchainImageWaitInfo waitInfo = {XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        waitInfo.timeout = XR_INFINITE_DURATION;
        constexpr XrSwapchainImageReleaseInfo releaseInfo = {XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        ID3D11Device* device;
        texture->GetDevice(&device);
        ID3D11DeviceContext* context;
        device->GetImmediateContext(&context);
        if (LastEyeShown == Eye::Right)
        {
            xrFrameState = {XR_TYPE_FRAME_STATE};
            XrResult xr = xrWaitFrame(xrSession, nullptr, &xrFrameState);
            CHECK_XR(xr, "Could not wait on OpenXR frame")
            xr = xrBeginFrame(xrSession, nullptr);
            CHECK_XR(xr, "Could not begin OpenXR frame")
            xrLayerProj = {XR_TYPE_COMPOSITION_LAYER_PROJECTION};
            XrViewState viewState = {XR_TYPE_VIEW_STATE};
            XrViewLocateInfo locateInfo = {XR_TYPE_VIEW_LOCATE_INFO};
            locateInfo.viewConfigurationType = xrViewType;
            locateInfo.displayTime = xrFrameState.predictedDisplayTime;
            locateInfo.space = xrAppSpace;
            xr = xrLocateViews(xrSession, &locateInfo, &viewState,
                               static_cast<uint32_t>(xrViews.size()),
                               &xrProjectionViewCount, xrViews.data());
            CHECK_XR(xr, "Could not locate OpenXR views")
            xrProjectionViews.resize(xrProjectionViewCount);
            xrProjectionViews.at(0) = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
            xrProjectionViews.at(0).pose = xrViews.at(0).pose;
            xrProjectionViews.at(0).fov = xrViews.at(0).fov;
            FOV = (xrViews.at(0).fov.angleRight - xrViews.at(0).fov.angleLeft) * 180.0f / PI;
            auto [qx, qy, qz, qw] = xrViews.at(0).pose.orientation;
            UE4Manager::SetChildRelativeRotation({-qz, qx, qy, -qw});
            xrProjectionViews.at(0).subImage.swapchain = xrSwapChains.at(0);
            xrProjectionViews.at(0).subImage.imageRect.offset = {0, 0};
            xrProjectionViews.at(0).subImage.imageRect.extent = {
                static_cast<int32_t>(xrWidth),
                static_cast<int32_t>(xrHeight)
            };
            xr = xrAcquireSwapchainImage(xrSwapChains.at(0), &acquireInfo, &imageId);
            CHECK_XR(xr, "Could not acquire OpenXR swapchain image")
            xr = xrWaitSwapchainImage(xrSwapChains.at(0), &waitInfo);
            CHECK_XR(xr, "Could not wait on OpenXR swapchain image")
            if (!D3D11Manager::ConvertFrame(texture, xrRTVs.at(0).at(imageId)))
            {
                Log::Error("[UnrealVR] Couldn't convert frame");
                return false;
            }
            xr = xrReleaseSwapchainImage(xrSwapChains.at(0), &releaseInfo);
            CHECK_XR(xr, "Could not release OpenXR swapchain image")
            LastEyeShown = Eye::Left;
            UE4Manager::SetChildRelativeLocation({
                -xrViews.at(0).pose.position.z * 100.f,
                xrViews.at(0).pose.position.x * 100.f,
                xrViews.at(0).pose.position.y * 100.f
            });
        }
        else
        {
            XrResult xr = xrAcquireSwapchainImage(xrSwapChains.at(1), &acquireInfo, &imageId);
            CHECK_XR(xr, "Could not acquire OpenXR swapchain image")
            xr = xrWaitSwapchainImage(xrSwapChains.at(1), &waitInfo);
            CHECK_XR(xr, "Could not wait on OpenXR swapchain image")
            if (!D3D11Manager::ConvertFrame(texture, xrRTVs.at(1).at(imageId)))
            {
                Log::Error("[UnrealVR] Couldn't convert frame");
                return false;
            }
            xrProjectionViews.resize(xrProjectionViewCount);
            xrProjectionViews.at(1) = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
            xrProjectionViews.at(1).pose = xrViews.at(1).pose;
            xrProjectionViews.at(1).fov = xrViews.at(1).fov;
            auto [qx, qy, qz, qw] = xrViews.at(1).pose.orientation;
            UE4Manager::SetChildRelativeRotation({-qz, qx, qy, -qw});
            xrProjectionViews.at(1).subImage.swapchain = xrSwapChains.at(1);
            xrProjectionViews.at(1).subImage.imageRect.offset = {0, 0};
            xrProjectionViews.at(1).subImage.imageRect.extent = {
                static_cast<int32_t>(xrWidth),
                static_cast<int32_t>(xrHeight)
            };
            xr = xrReleaseSwapchainImage(xrSwapChains.at(1), &releaseInfo);
            CHECK_XR(xr, "Could not release OpenXR swapchain image")
            xrLayerProj.space = xrAppSpace;
            xrLayerProj.viewCount = static_cast<uint32_t>(xrProjectionViews.size());
            xrLayerProj.views = xrProjectionViews.data();
            const auto xrLayer = reinterpret_cast<XrCompositionLayerBaseHeader*>(&xrLayerProj);
            XrFrameEndInfo endInfo = {XR_TYPE_FRAME_END_INFO};
            endInfo.displayTime = xrFrameState.predictedDisplayTime;
            endInfo.environmentBlendMode = xrBlend;
            endInfo.layerCount = xrLayer == nullptr ? 0 : 1;
            endInfo.layers = &xrLayer;
            xr = xrEndFrame(xrSession, &endInfo);
            CHECK_XR(xr, "Could not end OpenXR frame")
            LastEyeShown = Eye::Right;
            UE4Manager::SetChildRelativeLocation({
                -xrViews.at(1).pose.position.z * 100.f,
                xrViews.at(1).pose.position.x * 100.f,
                xrViews.at(1).pose.position.y * 100.f
            });
        }
        return true;
    }

    void VRManager::Stop()
    {
        for (uint64_t i = 0; i < xrRTVs.at(0).size(); i++)
        {
            xrRTVs.at(0).at(i)->Release();
            xrRTVs.at(1).at(i)->Release();
        }
        for (uint32_t i = 0; i < xrViewCount; i++)
            if (xrSwapChains.at(i) != XR_NULL_HANDLE && xrDestroySwapchain(xrSwapChains.at(i)))
                Log::Error("[UnrealVR] Failed to destroy OpenXR swapchain");
        if (xrAppSpace != XR_NULL_HANDLE && xrDestroySpace(xrAppSpace) != XR_SUCCESS)
            Log::Error("[UnrealVR] Failed to destroy OpenXR app space");
        if (xrSession != XR_NULL_HANDLE && xrDestroySession(xrSession) != XR_SUCCESS)
            Log::Error("[UnrealVR] Failed to destroy OpenXR session");
        if (xrDebug != XR_NULL_HANDLE && xrExtDestroyDebugUtilsMessenger(xrDebug) != XR_SUCCESS)
            Log::Error("[UnrealVR] Failed to destroy OpenXR debug");
        if (xrInstance != XR_NULL_HANDLE && xrDestroyInstance(xrInstance) != XR_SUCCESS)
            Log::Error("[UnrealVR] Failed to destroy OpenXR instance");
    }
}
