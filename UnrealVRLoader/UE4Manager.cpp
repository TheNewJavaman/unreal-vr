#include "UE4Manager.h"

#include <format>
#include <Utilities/Globals.h>

#include "VRManager.h"
#include "UE4Extensions.h"

#define FIND_UE4(ptr, t, name) \
    const auto ptr = UE4::UObject::FindObject<t>(name); \
    if (ptr == nullptr) \
    { \
        Log::Warn(std::format("[UnrealVR] Couldn't find ({})", name)); \
        return; \
    }

namespace UnrealVR
{
    void UE4Manager::SetViewTarget()
    {
        if (!Resized) Resize();
        const auto playerController = UE4::UGameplayStatics::GetPlayerController(0);
        if (playerController == nullptr)
        {
            Log::Error("[UnrealVR] Couldn't find PlayerController(0)");
            return;
        }
        FIND_UE4(getViewTargetFunc, UE4::UFunction, "Function Engine.Controller.GetViewTarget");
        auto getViewTargetParams = UE4::GetViewTargetParams();
        playerController->ProcessEvent(getViewTargetFunc, &getViewTargetParams);
        if (getViewTargetParams.ViewTarget == nullptr || getViewTargetParams.ViewTarget == viewTarget) return;
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
        Log::Info("[UnrealVR] Spawned view target actor");
        FIND_UE4(attachFunc, UE4::UFunction, "Function Engine.Actor.K2_AttachToActor");
        auto attachParams = UE4::AttachToActorParams();
        attachParams.ParentActor = getViewTargetParams.ViewTarget;
        viewTarget->ProcessEvent(attachFunc, &attachParams);
        Log::Info("[UnrealVR] Attached new view target to original");
        FIND_UE4(setViewTargetFunc, UE4::UFunction, "Function Engine.PlayerController.SetViewTargetWithBlend");
        auto setViewTargetParams = UE4::SetViewTargetWithBlendParams();
        setViewTargetParams.NewViewTarget = viewTarget;
        playerController->ProcessEvent(setViewTargetFunc, &setViewTargetParams);
        Log::Info("[UnrealVR] Set new view target as primary");
    }

    void UE4Manager::Resize()
    {
        uint32_t width, height;
        VRManager::GetRecommendedResolution(&width, &height);
        const auto command = std::format(
            L"r.SetRes {}x{}f",
            static_cast<int>(width),
            static_cast<int>(height)
        ).c_str();
        UE4::UGameplayStatics::ExecuteConsoleCommand(command, nullptr);
        Resized = true;
    }

    void UE4Manager::AddRelativeLocation(Vector3 relativeLocation)
    {
        if (viewTarget == nullptr) return;
        FIND_UE4(func, UE4::UFunction, "Function Engine.Actor.K2_AddActorLocalOffset");
        auto params = UE4::AddActorLocalOffsetParams();
        params.DeltaLocation = {relativeLocation.X, relativeLocation.Y, relativeLocation.Z};
        viewTarget->ProcessEvent(func, &params);
    }

    void UE4Manager::SetAbsoluteRotation(Vector3 absoluteRotation)
    {
        if (viewTarget == nullptr) return;
        FIND_UE4(func, UE4::UFunction, "Function Engine.Actor.K2_SetActorRotation");
        auto params = UE4::SetActorRotationParams();
        params.NewRotation = {absoluteRotation.X, absoluteRotation.Y, absoluteRotation.Z};
        viewTarget->ProcessEvent(func, &params);
    }
}
