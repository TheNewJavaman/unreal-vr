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
            UE4::FVector DeltaLocation;
            bool bSweep;
            void* SweepHitResult;
            bool bTeleport;
        } params;
        params.DeltaLocation = {relativeLocation.X, relativeLocation.Y, relativeLocation.Z};
        params.bSweep = false;
        params.bTeleport = true;
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
            UE4::FRotator NewRotation;
            bool bTeleportPhysics;
        } params;
        params.NewRotation = {absoluteRotation.X, absoluteRotation.Y, absoluteRotation.Z};
        params.bTeleportPhysics = true;
        viewTarget->ProcessEvent(func, &params);
        return true;
    }

    bool UE4Manager::SetResolution(int width, int height)
    {
        const auto command = std::format(L"r.SetRes {}x{}f", width, height).c_str();
        UE4::UGameplayStatics::ExecuteConsoleCommand(command, nullptr);
        /*
        const auto settings = UE4::UObject::FindObject<UE4::UClass>(
            "Class Engine.GameUserSettings"
        );
        if (settings == nullptr)
        {
            Log::Warn("[UnrealVR] Class Engine.GameUserSettings doesn't exist");
            return false;
        }
        const auto setFunc = UE4::UObject::FindObject<UE4::UFunction>(
            "Function Engine.GameUserSettings.SetScreenResolution"
        );
        if (setFunc == nullptr)
        {
            Log::Warn("[UnrealVR] Function Engine.GameUserSettings.SetScreenResolution doesn't exist");
            return false;
        }
        struct FIntPoint
        {
            int X;
            int Y;
        };
        struct
        {
            FIntPoint Resolution;
        } setParams;
        setParams.Resolution = {width, height};
        settings->ProcessEvent(setFunc, &setParams);
        const auto applyFunc = UE4::UObject::FindObject<UE4::UFunction>(
            "Function Engine.GameUserSettings.ApplyResolutionSettings"
        );
        if (applyFunc == nullptr)
        {
            Log::Warn("[UnrealVR] Function Engine.GameUserSettings.ApplyResolutionSettings doesn't exist");
            return false;
        }
        struct
        {
            bool bCheckForCommandLineOverrides;
        } applyParams;
        applyParams.bCheckForCommandLineOverrides = false;
        settings->ProcessEvent(applyFunc, &applyParams);
        */
        return true;
    }
}
