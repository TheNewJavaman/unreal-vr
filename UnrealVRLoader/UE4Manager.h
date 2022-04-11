#pragma once

#include <map>
#include <Ue4.hpp>

#include "VRManager.h"

namespace UnrealVR
{
    enum class Eye;

    class UE4Manager
    {
    public:
        /** Implies that all the necessary game objects have been loaded */
        static inline bool GameLoaded = false;

        /** Register events with UnrealModLoader */
        static void AddEvents();

        /** Set the view target to a custom camera actor meant for VR */
        static void SetViewTarget();
        static inline float FOVScale = 1.f;

        /** Set the render resolution to match the VR headset */
        static void Resize();

        /** Input VR pose (with corrected axes) and forward to Unreal Engine, with several adjustments */
        //static void UpdatePose(UE4::FVector loc, UE4::FQuat rot, UE4::FVector loc2);
        static void UpdatePose(UE4::FQuat rot, Eye eye);
        static inline float CmUnitsScale = 1.f;

    private:
        /** Map of cached, static UObjects */
        template <class T>
        static bool GetUObject(T** ptr, std::string name);
        inline static std::map<std::string, UE4::UObject*> uObjects;

        /** Called once per scene start */
        static void InitGameStateCallback();

        /** SetViewTarget */
        inline static UE4::APlayerController* playerController = nullptr;
        inline static UE4::AActor* parentViewTarget = nullptr;
        inline static UE4::AActor* childViewTarget = nullptr;
        inline static UE4::UObject* cameraComponent = nullptr;

        /** Resize */
        inline static UE4::UObject* gameUserSettings = nullptr;

        /** UpdatePose */
        inline static UE4::FVector lastLoc = UE4::FVector();
        inline static UE4::FRotator lastRot = UE4::FRotator();
    };
}
