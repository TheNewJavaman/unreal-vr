#include "UE4Manager.h"

#include <Utilities/Globals.h>

namespace UnrealVR
{
    void UE4Manager::AddEvents()
    {
        Global::GetGlobals()->eventSystem.registerEvent(
            new Event<UE4::AActor*>("BeginPlay", &BeginPlayCallback)
        );
    }

    void UE4Manager::BeginPlayCallback(UE4::AActor* Actor)
    {
        const auto now = std::chrono::system_clock::now();
        const std::chrono::duration<double> elapsed = now - lastCallback;
        if (elapsed.count() > MAX_CALLBACK_SECONDS)
        {
            const auto playerController = UE4::UGameplayStatics::GetPlayerController(0);
            if (playerController == nullptr)
            {
                Log::Error("[UnrealVR] PlayerController(0) doesn't exist");
                lastCallback = now;
                return;
            }
            const auto viewTargetFunc = UE4::UObject::FindObject<UE4::UFunction>(
                "Function Engine.Controller.GetViewTarget"
            );
            if (viewTargetFunc == nullptr)
            {
                Log::Warn("[UnrealVR] Function Engine.Controller.GetViewTarget doesn't exist");
                lastCallback = now;
                return;
            }
            struct
            {
                UE4::AActor* ViewTarget;
            } viewTargetParams;
            playerController->ProcessEvent(viewTargetFunc, &viewTargetParams);
            viewTarget = viewTargetParams.ViewTarget;
            if (viewTarget == nullptr)
            {
                Log::Warn("[UnrealVR] PlayerController's ViewTarget doesn't exist");
            }
        }
        lastCallback = now;
    }

    bool UE4Manager::AddRelativeLocation(Vector3 relativeLocation)
    {
        if (viewTarget == nullptr)
        {
            return false;
        }
        const auto func = UE4::UObject::FindObject<UE4::UFunction>(
            "Function Engine.Actor.K2_AddActorLocalOffset"
        );
        if (func == nullptr)
        {
            Log::Warn("[UnrealVR] Function Engine.Actor.K2_AddActorLocalOffset doesn't exist");
            return false;
        }
        struct
        {
            UE4::FVector DeltaLocation = UE4::FVector(relativeLocation.X, relativeLocation.Y, relativeLocation.Z);
            bool bSweep = false;
            void* SweepHitResult;
            bool bTeleport = true;
        } params;
        viewTarget->ProcessEvent(func, &params);
        return true;
    }

    bool UE4Manager::SetAbsoluteRotation(Vector3 absoluteRotation)
    {
        if (viewTarget == nullptr)
        {
            return false;
        }
        const auto func = UE4::UObject::FindObject<UE4::UFunction>(
            "Function Engine.Actor.K2_SetActorRotation"
        );
        if (func == nullptr)
        {
            Log::Warn("[UnrealVR] Function Engine.Actor.K2_SetActorRotation doesn't exist");
            return false;
        }
        struct
        {
            UE4::FRotator NewRotation = UE4::FRotator(absoluteRotation.X, absoluteRotation.Y, absoluteRotation.Z);
            bool bTeleportPhysics = true;
        } params;
        viewTarget->ProcessEvent(func, &params);
        return true;
    }
}
