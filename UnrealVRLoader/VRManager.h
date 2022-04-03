#pragma once

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11

#include <d3d11.h>
#include <map>
#include <vector>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#include "Vector3.h"

namespace UnrealVR
{
    /** UnrealVR currently uses alternate eye rendering (AER), so we must keep track of which eye was last rendered */
    enum class Eye
    {
        Left,
        Right
    };

    class VRManager
    {
    public:
        /** Begin initializing OpenXR: starts the runtime, loads extensions, etc. */
        static bool Init();

        /** Finish initializing OpenXR: set up app space, D3D11 device, etc. */
        static bool ContinueInit(ID3D11Device* device);
        static inline bool ContinueInitDone = false;

        /** Get the recommended headset render resolution for Unreal Engine */
        static bool GetRecommendedResolution(uint32_t* width, uint32_t* height);

        static void GetRecommendedFieldOfView(float* ptr);
        
        /**
         * Create a VR swapchain with the same format and sample count as Unreal Engine's swapchain
         *
         * TODO: Use the correct IPD upon swapchain creation
         */
        static bool CreateSwapChains(uint32_t sampleCount);
        static inline bool CreateSwapChainsDone = false;

        /** Start the OpenXR session */
        static bool FinalizeInit();
        static inline bool FinalizeInitDone = false;

        /** Copy a frame and present it to the headset */
        static bool SubmitFrame(ID3D11Texture2D* texture);
        inline static Eye LastEyeShown = Eye::Right;
        inline static float FOV = 0.0f;
        
        /** Releases OpenXR resources */
        static void Stop();

    private:
        /**
         * Loads the features that will be used, alongside an OpenXR handle
         *
         * TODO: Are the debug tools needed?
         */
        static bool GetInstanceWithExtensions();

        /**
         * Loads the debug extension for logging
         *
         * TODO: Handle different log levels
         */
        static bool LoadDebugExtension();

        /** Loads the D3D11 extension for swapchain management */
        static bool LoadD3D11Extension();

        /** GetInstanceWithExtensions */
        inline static const char* applicationName = "UnrealVR";
        inline static XrFormFactor xrFormFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
        inline static XrViewConfigurationType xrViewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
        inline static XrInstance xrInstance = {};

        /** LoadDebugExtension */
        inline static PFN_xrGetD3D11GraphicsRequirementsKHR xrExtGetD3D11GraphicsRequirements = nullptr;
        inline static PFN_xrCreateDebugUtilsMessengerEXT xrExtCreateDebugUtilsMessenger = nullptr;
        inline static PFN_xrDestroyDebugUtilsMessengerEXT xrExtDestroyDebugUtilsMessenger = nullptr;
        inline static XrDebugUtilsMessengerEXT xrDebug = {};

        /** LoadD3D11Extension */
        inline static XrSystemId xrSystemId = XR_NULL_SYSTEM_ID;
        inline static XrEnvironmentBlendMode xrBlend = {};

        /** ContinueInit */
        inline static XrGraphicsBindingD3D11KHR graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_D3D11_KHR};
        inline static XrSession xrSession = {};
        inline static XrSpace xrAppSpace = {};

        /** GetRecommendedResolution */
        inline static uint32_t xrViewCount = 0;
        inline static std::vector<XrViewConfigurationView> xrConfigViews;
        inline static uint32_t xrWidth, xrHeight;

        /** CreateSwapChains */
        inline static DXGI_FORMAT xrFormat = DXGI_FORMAT_UNKNOWN;
        inline static constexpr XrPosef xrPoseIdentity = {{0, 0, 0, 1}, {0, 0, 0}};
        inline static std::vector<XrView> xrViews;
        inline static std::vector<XrSwapchain> xrSwapChains;
        inline static std::map<uint32_t, std::vector<ID3D11RenderTargetView*>> xrRTVs;

        /** SubmitFrame */
        inline static XrCompositionLayerProjection xrLayerProj = {};
        inline static XrFrameState xrFrameState = {};
        inline static std::vector<XrCompositionLayerProjectionView> xrProjectionViews;
        inline static uint32_t xrProjectionViewCount = 0;
        inline static bool fovLogged = false;
    };
}
