#pragma once

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11

#include <d3d11.h>
#include <map>
#include <vector>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

namespace UnrealVR
{
    /** UnrealVR currently uses alternate eye rendering (AER), so we must keep track of which eye was last rendered */
    enum class Eye
    {
        Left,
        Right
    };

    class OpenXRService
    {
    public:
        /** Begin initializing OpenXR: starts the runtime, loads extensions */
        static bool BeginInit();

        /** Finish initializing OpenXR: sets up app space, sets D3D11 device, starts session */
        static bool FinishInit(ID3D11Device* device, uint32_t sampleCount);
        static inline bool VRLoaded = false;
        static inline uint32_t EyeWidth = 1;
        static inline uint32_t EyeHeight = 1;
        static inline XrFovf EyeFOV = {1.f, 1.f, 1.f, 1.f};

        /** Copy a frame and present it to the headset */
        static bool SubmitFrame(ID3D11Texture2D* texture);
        static inline Eye LastEyeShown = Eye::Right;

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
        static inline PFN_xrGetD3D11GraphicsRequirementsKHR xrExtGetD3D11GraphicsRequirements = nullptr;
        static inline PFN_xrCreateDebugUtilsMessengerEXT xrExtCreateDebugUtilsMessenger = nullptr;
        static inline PFN_xrDestroyDebugUtilsMessengerEXT xrExtDestroyDebugUtilsMessenger = nullptr;
        static inline XrDebugUtilsMessengerEXT xrDebug = {};

        /** Loads the D3D11 extension for swapchain management */
        static bool LoadD3D11Extension();
        static inline XrSystemId xrSystemId = XR_NULL_SYSTEM_ID;
        static inline XrEnvironmentBlendMode xrBlend = {};

        /** FinishInit */
        static inline XrGraphicsBindingD3D11KHR graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_D3D11_KHR};
        static inline XrSession xrSession = {};
        static inline XrSpace xrAppSpace = {};
        static inline uint32_t xrViewCount = 0;
        static inline std::vector<XrViewConfigurationView> xrConfigViews;
                
        /** Create a VR swapchain with the same format and sample count as Unreal Engine's swapchain */
        static bool CreateSwapChains(uint32_t sampleCount);
        static inline DXGI_FORMAT xrFormat = DXGI_FORMAT_UNKNOWN;
        static inline constexpr XrPosef xrPoseIdentity = {{0, 0, 0, 1}, {0, 0, 0}};
        static inline std::vector<XrView> xrViews;
        static inline std::vector<XrSwapchain> xrSwapChains;
        static inline std::map<uint32_t, std::vector<ID3D11RenderTargetView*>> xrRTVs;

        /** GetInstanceWithExtensions */
        static inline const char* applicationName = "UnrealVR";
        static inline XrFormFactor xrFormFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
        static inline XrViewConfigurationType xrViewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
        static inline XrInstance xrInstance = {};

        /** SubmitFrame */
        static inline XrFrameState xrFrameState = {};
        static inline std::vector<XrCompositionLayerProjectionView> xrProjectionViews;
        static inline uint32_t xrProjectionViewCount = 0;
    };
}
