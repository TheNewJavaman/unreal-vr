#pragma once

#include <Ue4.hpp>

#include "Vector3.h"

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

        /** Sets the absolute rotation of the view target (camera) */
        static void SetAbsoluteRotation(Vector3 absoluteRotation);
        
    private:
        /** Called once per scene start */
        static void BeginPlaySingleCallback();
        
        /** SetViewTarget */
        inline static UE4::AActor* viewTarget = nullptr;
    };
}
