#pragma once

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11

#include <d3d11.h>
#include <vector>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

namespace UnrealVR
{
    enum class Eye
    {
        Left,
        Right
    };
    
    class VRManager
    {
    public:
        /** Begin initializing OpenXR: starts the runtime, loads D3D11, etc. */
        static bool Init();

        /**
         * Called once the game needs a swapchain
         *
         * TODO: Finish initializing OpenXR once we can send the swapchain back to Unreal Engine
         */
        static bool ContinueInitAndCreateSwapChain(ID3D11Device* device, DXGI_SWAP_CHAIN_DESC* swapChainDesc);

        /** Present a frame to the headset */
        static void SubmitFrame();

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
        static void LoadDebugExtension();

        /** Loads the D3D11 extension for eventual swapchain management */
        static void LoadD3D11Extension();

        static void StartFrame();
        
        static void EndFrame();
        
        inline static const char* applicationName = "UnrealVR";
        inline static XrFormFactor xrFormFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
        inline static XrViewConfigurationType xrViewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
        inline static XrInstance xrInstance = {};

        inline static PFN_xrGetD3D11GraphicsRequirementsKHR xrExtGetD3D11GraphicsRequirements = nullptr;
        inline static PFN_xrCreateDebugUtilsMessengerEXT xrExtCreateDebugUtilsMessenger = nullptr;
        inline static PFN_xrDestroyDebugUtilsMessengerEXT xrExtDestroyDebugUtilsMessenger = nullptr;

        inline static XrDebugUtilsMessengerEXT xrDebug = {};
        inline static XrSystemId xrSystemId = XR_NULL_SYSTEM_ID;
        inline static XrEnvironmentBlendMode xrBlend = {};

        inline static XrGraphicsBindingD3D11KHR graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_D3D11_KHR};
        inline static XrSession xrSession = {};
        inline static XrSpace xrAppSpace = {};
        inline static constexpr XrPosef xrPoseIdentity = {{0, 0, 0, 1}, {0, 0, 0}};
        inline static std::vector<XrView> xrViews;
        inline static XrSwapchain xrSwapchain = {};

        inline static XrFrameState xrFrameState = {};
        inline static XrCompositionLayerBaseHeader* xrLayer = nullptr;
        inline static XrCompositionLayerProjection xrLayerProj = {XR_TYPE_COMPOSITION_LAYER_PROJECTION};
        inline static ID3D11Texture2D* leftEyeTexture = nullptr;
    };
}
