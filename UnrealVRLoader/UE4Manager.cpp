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
                return;
            }
            const auto viewTargetFunc = UE4::UObject::FindObject<UE4::UFunction>(
                "Function Engine.Controller.GetViewTarget"
            );
            if (viewTargetFunc == nullptr)
            {
                Log::Warn("[UnrealVR] Function Engine.Controller.GetViewTarget doesn't exist");
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
            
            const auto locationFunc = UE4::UObject::FindObject<UE4::UFunction>(
                "Function Engine.Actor.K2_SetActorRelativeLocation"
            );
            if (locationFunc == nullptr)
            {
                Log::Warn("[UnrealVR] Function Engine.Actor.K2_SetActorRelativeLocation doesn't exist");
                return;
            }
            struct
            {
                UE4::FVector NewRelativeLocation = UE4::FVector(0.0f, 0.0f, 100.0f);
                bool bSweep = false;
                void* OutSweepHitResult;
                int Teleport = 1;
            } locationParams;
            viewTarget->ProcessEvent(locationFunc, &locationParams);
        }
        lastCallback = now;
    }

    void UE4Manager::SetRelativeLocation(Vector3 relativeLocation)
    {
        if (viewTarget != nullptr)
        {
            
        }
    }

    void UE4Manager::SetAbsoluteRotation(Vector3 absoluteRotation)
    {
        
    }


}
