#pragma once

#include <map>
#include <Ue4.hpp>

namespace UnrealVR
{
    class UE4Manager
    {
    public:
        /** Implies that all the necessary game objects have been loaded */
        static inline bool GameLoaded = false;

        /** Register events with UnrealModLoader */
        static void AddEvents();

        /** Set the view target to a custom camera actor meant for VR */
        static void SetViewTarget();

        /** Set the render resolution to match the VR headset */
        static void Resize();

        /** May be changed at runtime via the PipeClient */
        static inline float CmUnitsScale = 1.f;
        
        /** Sets the translational offset of the view target (camera) from its parent, the original view target */
        static void SetChildRelativeLocation(UE4::FVector relativeLocation);

        /** Sets the rotational orientation of the view target, according to the user's preferences */
        static void SetParentRelativeRotation(UE4::FQuat q);

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

        /** SetParentRelativeLocation */
        inline static UE4::FVector lastParentVRLocation = UE4::FVector();

        /** SetParentRelativeRotation */
        inline static UE4::FRotator lastParentVRRotation = UE4::FRotator();

        /** Normalizes angles to (-180, 180] degrees */
        static float Normalize(float a);
    };
}
