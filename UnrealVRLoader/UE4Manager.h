#pragma once

#include <map>
#include <Ue4.hpp>

#include "Vector3.h"
#include "Vector4.h"

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
        static void AddRelativeLocation(Vector3 relativeLocation);

        /** Adds a rotational offset, relative to the world */
        static void AddWorldRotation(Vector4 quat);
        inline static Vector3 lastRotation = Vector3();
        
    private:
        /** Map of cached (static) UObjects */
        template<class T>
        static bool GetUObject(T** ptr, std::string name);
        inline static std::map<std::string, UE4::UObject*> uobjects;
        
        /** Called once per scene start */
        static void BeginPlaySingleCallback();
        
        /** SetViewTarget */
        inline static UE4::AActor* viewTarget = nullptr;
        inline static UE4::UObject* cameraComponent = nullptr;
        inline static UE4::APlayerController* playerController = nullptr;
        inline static UE4::AActor* originalViewTarget = nullptr;

        /** Resize */
        inline static UE4::UObject* gameUserSettings = nullptr;
    };
}
