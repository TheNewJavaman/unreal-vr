#include "UE4Manager.h"

#include <format>
#include <Utilities/Globals.h>

#include "VRManager.h"
#include "UE4Extensions.h"

#define USING_UOBJECT(ptr, t, name) \
    if ((ptr) == nullptr) \
    { \
        (ptr) = UE4::UObject::FindObject<t>(name); \
        if ((ptr) == nullptr) { \
            Log::Warn(std::format("[UnrealVR] Couldn't find ({})", name)); \
            return; \
        } \
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
        playerController = nullptr;
        viewTarget = nullptr;
    }

    void UE4Manager::SetViewTarget()
    {
        // Get player controller
        if (playerController == nullptr)
        {
            playerController = UE4::UGameplayStatics::GetPlayerController(0);
            if (playerController == nullptr)
            {
                Log::Error("[UnrealVR] Couldn't find PlayerController(0)");
                return;
            }
        }

        // Get original view target
        USING_UOBJECT(getViewTargetFunc, UE4::UFunction, "Function Engine.Controller.GetViewTarget")
        auto getViewTargetParams = UE4::GetViewTargetParams();
        playerController->ProcessEvent(getViewTargetFunc, &getViewTargetParams);
        if (getViewTargetParams.ViewTarget == nullptr) return;

        // Spawn new view target if nonexistent
        if (viewTarget == nullptr)
        {
            // Spawn the view target actor
            USING_UOBJECT(staticMeshActorClass, UE4::UClass, "Class Engine.StaticMeshActor")
            if (GameProfile::SelectedGameProfile.IsUsingDeferedSpawn)
            {
                viewTarget = UE4::UGameplayStatics::BeginDeferredActorSpawnFromClass(
                    staticMeshActorClass,
                    UE4::FTransform(),
                    UE4::ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
                    nullptr
                );
            }
            else
            {
                const auto transform = UE4::FTransform();
                const auto params = UE4::FActorSpawnParameters::FActorSpawnParameters();
                viewTarget = UE4::UWorld::GetWorld()->SpawnActor(
                    staticMeshActorClass,
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
            USING_UOBJECT(setMobilityFunc, UE4::UFunction, "Function Engine.StaticMeshActor.SetMobility")
            auto setMobilityParams = UE4::SetMobilityParams();
            viewTarget->ProcessEvent(setMobilityFunc, &setMobilityParams);
        }

        // Set new view target if needed
        if (getViewTargetParams.ViewTarget != viewTarget)
        {
            // Attach new view target to original (follows positioning, rotation, etc.)
            USING_UOBJECT(attachToActorFunc, UE4::UFunction, "Function Engine.Actor.K2_AttachToActor")
            auto attachParams = UE4::AttachToActorParams();
            attachParams.ParentActor = getViewTargetParams.ViewTarget;
            viewTarget->ProcessEvent(attachToActorFunc, &attachParams);

            // Set new view target
            USING_UOBJECT(setViewTargetFunc, UE4::UFunction, "Function Engine.PlayerController.SetViewTargetWithBlend")
            auto setViewTargetParams = UE4::SetViewTargetWithBlendParams();
            setViewTargetParams.NewViewTarget = viewTarget;
            playerController->ProcessEvent(setViewTargetFunc, &setViewTargetParams);
        }
    }

    void UE4Manager::Resize()
    {
        uint32_t width, height;
        VRManager::GetRecommendedResolution(&width, &height);
        USING_UOBJECT(gameUserSettings, UE4::UObject, "GameUserSettings Engine.Default__GameUserSettings")
        USING_UOBJECT(setScreenResolutionFunc, UE4::UFunction, "Function Engine.GameUserSettings.SetScreenResolution")
        UE4::SetScreenResolutionParams setResParams;
        setResParams.Resolution.X = static_cast<int>(width);
        setResParams.Resolution.Y = static_cast<int>(height);
        gameUserSettings->ProcessEvent(setScreenResolutionFunc, &setResParams);
        USING_UOBJECT(applyResolutionSettingsFunc, UE4::UFunction,
                      "Function Engine.GameUserSettings.ApplyResolutionSettings")
        auto applyParams = UE4::ApplyResolutionSettingsParams();
        gameUserSettings->ProcessEvent(applyResolutionSettingsFunc, &applyParams);
        Resized = true;
        Log::Info("[UnrealVR] Resized render resolution to match VR headset");
    }

    void UE4Manager::AddRelativeLocation(Vector3 relativeLocation)
    {
        if (viewTarget == nullptr) return;
        USING_UOBJECT(addActorLocalOffsetFunc, UE4::UFunction, "Function Engine.Actor.K2_AddActorLocalOffset")
        auto params = UE4::AddActorLocalOffsetParams();
        params.DeltaLocation = UE4::FVector(relativeLocation.X, relativeLocation.Y, relativeLocation.Z);
        viewTarget->ProcessEvent(addActorLocalOffsetFunc, &params);
    }

    void UE4Manager::SetAbsoluteRotation(Vector3 absoluteRotation)
    {
        if (viewTarget == nullptr) return;
        USING_UOBJECT(setActorRotationFunc, UE4::UFunction, "Function Engine.Actor.K2_SetActorRotation")
        auto params = UE4::SetActorRotationParams();
        params.NewRotation = UE4::FRotator(absoluteRotation.X, absoluteRotation.Y, absoluteRotation.Z);
        viewTarget->ProcessEvent(setActorRotationFunc, &params);
    }
}
