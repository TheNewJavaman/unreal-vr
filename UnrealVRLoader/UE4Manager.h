#pragma once

#include <chrono>
#include <Ue4.hpp>

#include "Vector3.h"

namespace UnrealVR
{
    class UE4Manager
    {
    public:
        /** Subscribes to UE4 events via UnrealModLoader */
        static void AddEvents();

        /**
         * Sets the translational offset of the view target (camera) from its parent
         *
         * TODO: Support scaling the offset in case the game's units do not match real-world centimeters
         */
        static bool AddRelativeLocation(Vector3 relativeLocation);

        /** Sets the absolute rotation of the view target (camera) */
        static bool SetAbsoluteRotation(Vector3 absoluteRotation);

        /** Sets the desktop resolution */
        static bool SetResolution(int width, int height);
        
    private:
        /**
         * Called for each actor in a scene when the scene starts
         *
         * In UnrealModLoader, BeginPlay is hooked for every actor in the scene, so there's no way to tell if the scene
         * that has just loaded is a new one, except by timing it. If there's a delay > MAX_CALLBACK_SECONDS, assume
         * that a new level has started. This solution is messy!
         */
        static void BeginPlayCallback(UE4::AActor* Actor);
        inline static double MAX_CALLBACK_SECONDS = 1.0;
        inline static std::chrono::time_point<std::chrono::system_clock> lastCallback = {};
        inline static UE4::AActor* viewTarget = nullptr;
    };
}
