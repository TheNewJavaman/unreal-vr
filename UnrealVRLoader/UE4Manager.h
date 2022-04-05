#pragma once

#include <map>
#include <Ue4.hpp>

namespace UnrealVR
{
    class UE4Manager
    {
    public:
        /** Register events with UnrealModLoader */
        static void AddEvents();
        
        /** Refocus the camera's perspective so that we can manipulate it in the future */
        static void SetViewTarget();

        /** Set the render resolution to match the VR headset */
        static void Resize();
        inline static bool Resized = false;
        
        /**
         * Sets the translational offset of the view target (camera) from its parent
         *
         * TODO: Support scaling the offset in case the game's units do not match real-world centimeters
         */
        static void SetChildRelativeLocation(UE4::FVector relativeLocation);

        static void SetParentRelativeLocation(UE4::FVector relativeLocation);
        
        /** Adds a rotational offset, relative to the world */
        static void SetRelativeRotation(UE4::FQuat q);

    private:
        /** Map of cached (static) UObjects */
        template<class T>
        static bool GetUObject(T** ptr, std::string name);
        inline static std::map<std::string, UE4::UObject*> uObjects;
        
        /** Called once per scene start */
        static void BeginPlaySingleCallback();

        /** SetViewTarget */
        inline static UE4::APlayerController* playerController = nullptr;
        inline static UE4::AActor* parentViewTarget = nullptr;
        inline static UE4::AActor* childViewTarget = nullptr;
        inline static UE4::UObject* cameraComponent = nullptr;

        /** Resize */
        inline static UE4::UObject* gameUserSettings = nullptr;

        inline static UE4::FVector lastParentRelativeLocation = UE4::FVector();
        
        /** SetRelativeRotation */
        inline static UE4::FRotator lastRotation = UE4::FRotator();
        
        
        static float Normalize(float a);
    };
}
