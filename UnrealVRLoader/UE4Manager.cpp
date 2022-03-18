#include "UE4Manager.h"

#include <format>
#include <Utilities/Globals.h>

#include "VRManager.h"
#include "UE4Extensions.h"

#define FIND_UE4(ptr, t, name) \
    auto ptr = UE4::UObject::FindObject<t>(name); \
    if (ptr == nullptr) \
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
        const auto playerController = UE4::UGameplayStatics::GetPlayerController(0);
        if (playerController == nullptr)
        {
            Log::Error("[UnrealVR] Couldn't find PlayerController(0)");
            return;
        }
        FIND_UE4(getViewTargetFunc, UE4::UFunction, "Function Engine.Controller.GetViewTarget");
        auto getViewTargetParams = UE4::GetViewTargetParams();
        playerController->ProcessEvent(getViewTargetFunc, &getViewTargetParams);
        if (getViewTargetParams.ViewTarget == nullptr) return;
        if (viewTarget == nullptr)
        {
            if (GameProfile::SelectedGameProfile.IsUsingDeferedSpawn)
            {
                viewTarget = UE4::UGameplayStatics::BeginDeferredActorSpawnFromClass(
                    UE4::AActor::StaticClass(),
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
                    UE4::AActor::StaticClass(),
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
        }
        if (getViewTargetParams.ViewTarget != viewTarget)
        {
            FIND_UE4(attachFunc, UE4::UFunction, "Function Engine.Actor.K2_AttachToActor");
            //auto attachParams = UE4::AttachToActorParams();
            //attachParams.ParentActor = getViewTargetParams.ViewTarget;
            std::vector<UE4::AActor*> attachParams(100);
            attachParams[0] = getViewTargetParams.ViewTarget;
            viewTarget->ProcessEvent(attachFunc, &attachParams);
            FIND_UE4(setViewTargetFunc, UE4::UFunction, "Function Engine.PlayerController.SetViewTargetWithBlend");
            auto setViewTargetParams = UE4::SetViewTargetWithBlendParams();
            setViewTargetParams.NewViewTarget = viewTarget;
            playerController->ProcessEvent(setViewTargetFunc, &setViewTargetParams);
        }
        if (viewTarget != nullptr)
        {
            FIND_UE4(getAttachFunc, UE4::UFunction, "Function Engine.Actor.GetAttachParentActor");
            auto getAttachParams = UE4::GetAttachParentActorParams();
            viewTarget->ProcessEvent(getAttachFunc, &getAttachParams);
        }
    }

    void UE4Manager::Resize()
    {
        uint32_t width, height;
        VRManager::GetRecommendedResolution(&width, &height);
        FIND_UE4(settings, UE4::UObject, "GameUserSettings Engine.Default__GameUserSettings")
        FIND_UE4(setResFunc, UE4::UFunction, "Function Engine.GameUserSettings.SetScreenResolution")
        auto setResParams = UE4::SetScreenResolutionParams();
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
        FIND_UE4(func, UE4::UFunction, "Function Engine.Actor.K2_AddActorLocalOffset");
        auto params = UE4::AddActorLocalOffsetParams();
        params.DeltaLocation = UE4::FVector(relativeLocation.X, relativeLocation.Y, relativeLocation.Z);
        viewTarget->ProcessEvent(func, &params);
    }

    void UE4Manager::SetAbsoluteRotation(Vector3 absoluteRotation)
    {
        if (viewTarget == nullptr) return;
        FIND_UE4(func, UE4::UFunction, "Function Engine.Actor.K2_SetActorRotation");
        auto params = UE4::SetActorRotationParams();
        params.NewRotation = UE4::FRotator(absoluteRotation.X, absoluteRotation.Y, absoluteRotation.Z);
        viewTarget->ProcessEvent(func, &params);
    }
}
