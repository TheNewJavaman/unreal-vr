#pragma once

#include <chrono>
#include <Ue4.hpp>

namespace UnrealVR
{
    class UE4Manager
    {
    public:
        /** Subscribes to UE4 events via UnrealModLoader */
        static void AddEvents();

    private:
        /**
         * Called for each actor in a scene when the scene starts
         *
         * In UnrealModLoader, BeginPlay is hooked for every actor in the scene, so there's no way to tell if the scene
         * that has just loaded is a new one, except by timing it. Iff there's a delay > MAX_CALLBACK_SECONDS, assume
         * that a new level has started. This solution is messy!
         */
        static void BeginPlayCallback(UE4::AActor* Actor);
        inline static double MAX_CALLBACK_SECONDS = 5.0;
        inline static std::chrono::time_point<std::chrono::system_clock> lastCallback = {};
    };
}
