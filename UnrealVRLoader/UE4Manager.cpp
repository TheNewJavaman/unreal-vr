#include "UE4Manager.h"

#include <format>
#include <Utilities/Globals.h>

#include "VRManager.h"
#include "UE4Extensions.h"

#define FIND_UE4(ptr, t, name) \
    auto (ptr) = UE4::UObject::FindObject<t>(name); \
    if ((ptr) == nullptr) \
    { \
        Log::Warn(std::format("[UnrealVR] Couldn't find ({})", name)); \
        return; \
    }

namespace UnrealVR
{
    void UE4Manager::AddEvents()
    {
        Global::GetGlobals()->eventSystem.registerEvent(new Event<>("BeginPlaySingle", &BeginPlaySingleCallback));
    }

    void UE4Manager::BeginPlaySingleCallback()
    {
        Resize();
        viewTarget = nullptr;
    }

    void UE4Manager::SetViewTarget()
    {
        // Get player controller
        const auto playerController = UE4::UGameplayStatics::GetPlayerController(0);
        if (playerController == nullptr)
        {
            Log::Error("[UnrealVR] Couldn't find PlayerController(0)");
            return;
        }

        // Get original view target
        FIND_UE4(getViewTargetFunc, UE4::UFunction, "Function Engine.Controller.GetViewTarget")
        auto getViewTargetParams = UE4::GetViewTargetParams();
        playerController->ProcessEvent(getViewTargetFunc, &getViewTargetParams);
        if (getViewTargetParams.ViewTarget == nullptr) return;

        // Spawn new view target if nonexistent
        if (viewTarget == nullptr)
        {
            // Spawn the view target actor
            FIND_UE4(staticMeshClass, UE4::UClass, "Class Engine.StaticMeshActor")
            const auto transform = UE4::FTransform();
            if (GameProfile::SelectedGameProfile.IsUsingDeferedSpawn)
            {
                viewTarget = UE4::UGameplayStatics::BeginDeferredActorSpawnFromClass(
                    staticMeshClass,
                    transform,
                    UE4::ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
                    nullptr
                );
            }
            else
            {
                const auto params = UE4::FActorSpawnParameters::FActorSpawnParameters();
                viewTarget = UE4::UWorld::GetWorld()->SpawnActor(
                    staticMeshClass,
                    &transform,
                    &params
                );
            }
            if (viewTarget == nullptr)
            {
                Log::Warn("[UnrealVR] Couldn't spawn view target actor");
                return;
            }
            Log::Info("[UnrealVR] Spawned new view target");
            
            // Enable mobility for the actor
            FIND_UE4(setMobilityFunc, UE4::UFunction, "Function Engine.StaticMeshActor.SetMobility")
            auto setMobilityParams = UE4::SetMobilityParams();
            viewTarget->ProcessEvent(setMobilityFunc, &setMobilityParams);
        }

        // Set new view target if needed
        if (getViewTargetParams.ViewTarget != viewTarget)
        {
            // Attach new view target to original (follows positioning, rotation, etc.)
            FIND_UE4(attachFunc, UE4::UFunction, "Function Engine.Actor.K2_AttachToActor")
            auto attachParams = UE4::AttachToActorParams();
            attachParams.ParentActor = getViewTargetParams.ViewTarget;
            viewTarget->ProcessEvent(attachFunc, &attachParams);
            
            // Set new view target
            FIND_UE4(setViewTargetFunc, UE4::UFunction, "Function Engine.PlayerController.SetViewTargetWithBlend")
            auto setViewTargetParams = UE4::SetViewTargetWithBlendParams();
            setViewTargetParams.NewViewTarget = viewTarget;
            playerController->ProcessEvent(setViewTargetFunc, &setViewTargetParams);
        }
    }

    void UE4Manager::Resize()
    {
        uint32_t width, height;
        VRManager::GetRecommendedResolution(&width, &height);
        FIND_UE4(settings, UE4::UObject, "GameUserSettings Engine.Default__GameUserSettings")
        FIND_UE4(setResFunc, UE4::UFunction, "Function Engine.GameUserSettings.SetScreenResolution")
        UE4::SetScreenResolutionParams setResParams;
        setResParams.Resolution.X = static_cast<int>(width);
        setResParams.Resolution.Y = static_cast<int>(height);
        settings->ProcessEvent(setResFunc, &setResParams);
        FIND_UE4(applyFunc, UE4::UFunction, "Function Engine.GameUserSettings.ApplyResolutionSettings")
        auto applyParams = UE4::ApplyResolutionSettingsParams();
        settings->ProcessEvent(applyFunc, &applyParams);
        Resized = true;
        Log::Info("[UnrealVR] Resized render resolution to match VR headset");
    }

    void UE4Manager::AddRelativeLocation(Vector3 relativeLocation)
    {
        if (viewTarget == nullptr) return;
        FIND_UE4(func, UE4::UFunction, "Function Engine.Actor.K2_AddActorLocalOffset")
        auto params = UE4::AddActorLocalOffsetParams();
        params.DeltaLocation = UE4::FVector(relativeLocation.X, relativeLocation.Y, relativeLocation.Z);
        viewTarget->ProcessEvent(func, &params);
    }

    void UE4Manager::SetAbsoluteRotation(Vector3 absoluteRotation)
    {
        if (viewTarget == nullptr) return;
        FIND_UE4(func, UE4::UFunction, "Function Engine.Actor.K2_SetActorRotation")
        auto params = UE4::SetActorRotationParams();
        params.NewRotation = UE4::FRotator(absoluteRotation.X, absoluteRotation.Y, absoluteRotation.Z);
        viewTarget->ProcessEvent(func, &params);
    }
}
