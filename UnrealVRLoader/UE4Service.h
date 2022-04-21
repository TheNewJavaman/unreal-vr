#pragma once

#include <map>
#include <Ue4.hpp>

#include "OpenXRService.h"
#include "UE4Extensions.h"

namespace UnrealVR
{
    enum class Eye;

    class UE4Service
    {
    public:
        /** Implies that all the necessary game objects have been loaded */
        static inline bool GameLoaded = false;

        /** Register events with UnrealModLoader */
        static void AddHooks();

        /** Set the view target to a custom camera actor meant for VR */
        static void SetViewTarget();
        static inline float FOVScale = 1.f;

        /** Set the render resolution to match the VR headset */
        static void Resize(int width, int height);
        static inline bool Resized = false;

        /** Input VR pose (with corrected axes) and forward to Unreal Engine, with several adjustments */
        //static void UpdatePose(UE4::FVector loc, UE4::FQuat rot, UE4::FVector loc2);
        static void UpdatePose(UE4::FQuat rot, Eye eye);
        static inline float CmUnitsScale = 1.f;

    private:
        /** Map of cached, static UObjects */
        template <class T>
        static bool GetUObject(T** ptr, std::string name);
        static inline std::map<std::string, UE4::UObject*> uObjects;

        /**
         * CalculateProjectionMatrix functions
         *
         * Called when evaluating the projection matrix that is sent to the GPU; this is where we use OpenXR's FOV
         */
        typedef UE4::FMatrix (CalculateProjectionMatrixFunc)(void* pFMinimalViewInfo);
        static CalculateProjectionMatrixFunc CalculateProjectionMatrixDetour;
        static inline CalculateProjectionMatrixFunc* CalculateProjectionMatrixTarget = nullptr;
        static inline CalculateProjectionMatrixFunc* CalculateProjectionMatrixOriginal = nullptr;

        /** Called once per scene start */
        static void InitGameStateCallback();

        /** SetViewTarget */
        static inline UE4::APlayerController* playerController = nullptr;
        static inline UE4::AActor* parentViewTarget = nullptr;
        static inline UE4::AActor* childViewTarget = nullptr;
        static inline UE4::UObject* cameraComponent = nullptr;

        /** Resize */
        static inline UE4::UObject* gameUserSettings = nullptr;

        /** UpdatePose */
        static inline UE4::FVector lastLoc = UE4::FVector();
        static inline UE4::FRotator lastRot = UE4::FRotator();
    };
}
