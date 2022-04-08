#include "UE4Manager.h"

#include <format>
#include <Utilities/Globals.h>

#include "VRManager.h"
#include "UE4Extensions.h"

#define USING_UOBJECT(ptr, T, name) \
    T* ptr; \
    if (!GetUObject<T>(&(ptr), name)) return;
#define ASSERT(var, name) \
    if ((var) == nullptr) \
    { \
        Log::Warn("[UnrealVR] Couldn't find %s", name); \
        return; \
    }
#define ASSERT_ASSIGN(src, dst) \
    ASSERT(src, #dst) \
    (dst) = src;

namespace UnrealVR
{
    template <class T>
    bool UE4Manager::GetUObject(T** ptr, std::string name)
    {
        const auto cached = uObjects.find(name);
        if (cached == uObjects.end())
        {
            auto found = UE4::UObject::FindObject<T>(name);
            if (found == nullptr)
            {
                Log::Warn("[UnrealVR] Couldn't find %s", name);
                return false;
            }
            uObjects.insert(std::pair(name, found));
            *ptr = found;
        }
        else
        {
            *ptr = static_cast<T*>(cached->second);
        }
        return true;
    }

    void UE4Manager::AddEvents()
    {
        Global::GetGlobals()->eventSystem.registerEvent(new Event<>("InitGameState", &InitGameStateCallback));
    }

    void UE4Manager::InitGameStateCallback()
    {
        if (!GameLoaded)
        {
            Resize();
            GameLoaded = true;
        }
        playerController = nullptr;
        parentViewTarget = nullptr;
        childViewTarget = nullptr;
        cameraComponent = nullptr;
        gameUserSettings = nullptr;
        lastRotationInput = UE4::FRotator();
    }

    void UE4Manager::SetViewTarget()
    {
        // Get player controller
        USING_UOBJECT(gameplayStatics, UE4::UObject, "GameplayStatics Engine.Default__GameplayStatics")
        USING_UOBJECT(getPlayerControllerFunc, UE4::UFunction, "Function Engine.GameplayStatics.GetPlayerController")
        auto getPlayerControllerParams = UE4::GetPlayerControllerParams();
        getPlayerControllerParams.WorldContextObject = UE4::UWorld::GetWorld();
        gameplayStatics->ProcessEvent(getPlayerControllerFunc, &getPlayerControllerParams);
        ASSERT_ASSIGN(getPlayerControllerParams.Result, playerController)

        // Get old view target
        USING_UOBJECT(getViewTargetFunc, UE4::UFunction, "Function Engine.Controller.GetViewTarget")
        auto getViewTargetParams = UE4::GetViewTargetParams();
        playerController->ProcessEvent(getViewTargetFunc, &getViewTargetParams);
        UE4::AActor* currentViewTarget;
        ASSERT_ASSIGN(getViewTargetParams.ViewTarget, currentViewTarget)

        if (childViewTarget == nullptr)
        {
            // Spawn new view target
            USING_UOBJECT(cameraActorClass, UE4::UClass, "Class Engine.CameraActor")
            if (GameProfile::SelectedGameProfile.IsUsingDeferedSpawn)
            {
                childViewTarget = UE4::UGameplayStatics::BeginDeferredActorSpawnFromClass(
                    cameraActorClass,
                    UE4::FTransform(),
                    UE4::ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
                    nullptr
                );
            }
            else
            {
                const auto transform = UE4::FTransform();
                const auto params = UE4::FActorSpawnParameters::FActorSpawnParameters();
                childViewTarget = UE4::UWorld::GetWorld()->SpawnActor(
                    cameraActorClass,
                    &transform,
                    &params
                );
            }

            // Get camera component
            USING_UOBJECT(getComponentByClassFunc, UE4::UFunction, "Function Engine.Actor.GetComponentByClass")
            auto getComponentByClassParams = UE4::GetComponentByClassParams();
            USING_UOBJECT(cameraComponentClass, UE4::UClass, "Class Engine.CameraComponent")
            getComponentByClassParams.ComponentClass = cameraComponentClass;
            childViewTarget->ProcessEvent(getComponentByClassFunc, &getComponentByClassParams);
            ASSERT_ASSIGN(getComponentByClassParams.Result, cameraComponent)
        }

        if (currentViewTarget != childViewTarget)
        {
            parentViewTarget = currentViewTarget;
            lastRotationInput = UE4::FRotator();

            // Attach new view target to old (follows positioning, rotation, scale)
            USING_UOBJECT(attachToActorFunc, UE4::UFunction, "Function Engine.Actor.K2_AttachToActor")
            auto attachParams = UE4::AttachToActorParams();
            attachParams.ParentActor = currentViewTarget;
            childViewTarget->ProcessEvent(attachToActorFunc, &attachParams);

            // Set new view target
            USING_UOBJECT(setViewTargetFunc, UE4::UFunction, "Function Engine.PlayerController.SetViewTargetWithBlend")
            auto setViewTargetParams = UE4::SetViewTargetWithBlendParams();
            setViewTargetParams.NewViewTarget = childViewTarget;
            playerController->ProcessEvent(setViewTargetFunc, &setViewTargetParams);
        }

        // Set field of view
        // TODO: Unreal Engine doesn't resize FOV after resolution change, causes 56% FOV multiplier (9/16)
        USING_UOBJECT(setFieldOfViewFunc, UE4::UFunction, "Function Engine.CameraComponent.SetFieldOfView")
        UE4::SetFieldOfViewParams setFieldOfViewParams;
        setFieldOfViewParams.InFieldOfView = VRManager::FOV * 16.f / 9.f;
        cameraComponent->ProcessEvent(setFieldOfViewFunc, &setFieldOfViewParams);

        // Disable aspect ratio constraint (enables letterboxing, minimizes stretching)
        USING_UOBJECT(setConstraintAspectRatioFunc, UE4::UFunction,
                      "Function Engine.CameraComponent.SetConstraintAspectRatio")
        auto setConstraintAspectRatioParams = UE4::SetConstraintAspectRatioParams();
        cameraComponent->ProcessEvent(setConstraintAspectRatioFunc, &setConstraintAspectRatioParams);

        // Get control rotation
        USING_UOBJECT(getControlRotationFunc, UE4::UFunction, "Function Engine.Controller.GetControlRotation")
        auto getControlRotationParams = UE4::GetControlRotationParams();
        playerController->ProcessEvent(getControlRotationFunc, &getControlRotationParams);

        // Update rotation to match parent's
        USING_UOBJECT(setActorRotationFunc, UE4::UFunction, "Function Engine.Actor.K2_SetActorRotation")
        auto setActorRotationParams = UE4::SetActorRotationParams();
        setActorRotationParams.NewRotation = UE4::FRotator(getControlRotationParams.Result);
        childViewTarget->ProcessEvent(setActorRotationFunc, &setActorRotationParams);
    }

    void UE4Manager::Resize()
    {
        // Get local settings
        USING_UOBJECT(defaultGameUserSettings, UE4::UObject, "GameUserSettings Engine.Default__GameUserSettings")
        USING_UOBJECT(getGameUserSettingsFunc, UE4::UFunction, "Function Engine.GameUserSettings.GetGameUserSettings")
        auto getGameUserSettingsParams = UE4::GetGameUserSettingsParams();
        defaultGameUserSettings->ProcessEvent(getGameUserSettingsFunc, &getGameUserSettingsParams);
        ASSERT_ASSIGN(getGameUserSettingsParams.Result, gameUserSettings)

        // Set the resolution
        uint32_t width, height;
        VRManager::GetRecommendedResolution(&width, &height);
        USING_UOBJECT(setScreenResolutionFunc, UE4::UFunction, "Function Engine.GameUserSettings.SetScreenResolution")
        UE4::SetScreenResolutionParams setScreenResolutionParams;
        setScreenResolutionParams.Resolution.X = static_cast<int>(width);
        setScreenResolutionParams.Resolution.Y = static_cast<int>(height);
        gameUserSettings->ProcessEvent(setScreenResolutionFunc, &setScreenResolutionParams);

        // Set to fullscreen mode for better performance
        USING_UOBJECT(setFullscreenModeFunc, UE4::UFunction, "Function Engine.GameUserSettings.SetFullscreenMode")
        auto setFullscreenModeParams = UE4::SetFullscreenModeParams();
        gameUserSettings->ProcessEvent(setFullscreenModeFunc, &setFullscreenModeParams);

        // Apply the resolution changes
        // TODO: Does this need to be run every BeginPlaySingle?
        USING_UOBJECT(applyResolutionSettingsFunc, UE4::UFunction,
                      "Function Engine.GameUserSettings.ApplyResolutionSettings")
        auto applyResolutionSettingsParams = UE4::ApplyResolutionSettingsParams();
        gameUserSettings->ProcessEvent(applyResolutionSettingsFunc, &applyResolutionSettingsParams);

        Log::Info("[UnrealVR] Resized render resolution to match VR headset");
    }

    void UE4Manager::SetChildRelativeLocation(const UE4::FVector relativeLocation)
    {
        if (childViewTarget == nullptr) return;

        // Set the view target's relative location
        USING_UOBJECT(setActorRelativeLocationFunc, UE4::UFunction, "Function Engine.Actor.K2_SetActorRelativeLocation")
        auto setActorRelativeLocationParams = UE4::SetActorRelativeLocationParams();
        setActorRelativeLocationParams.RelativeLocation = UE4::FVector(
            relativeLocation.X * CmUnitsScale,
            relativeLocation.Y * CmUnitsScale,
            relativeLocation.Z * CmUnitsScale
        );
        childViewTarget->ProcessEvent(setActorRelativeLocationFunc, &setActorRelativeLocationParams);
    }

    void UE4Manager::SetChildRelativeRotation(const UE4::FQuat q)
    {
        if (childViewTarget == nullptr) return;

        // Convert the quat to a UE4 FRotator
        USING_UOBJECT(mathLibrary, UE4::UObject, "KismetMathLibrary Engine.Default__KismetMathLibrary")
        USING_UOBJECT(quatRotatorFunc, UE4::UFunction, "Function Engine.KismetMathLibrary.Quat_Rotator")
        auto quatRotatorParams = UE4::QuatRotatorParams();
        quatRotatorParams.Q = q;
        mathLibrary->ProcessEvent(quatRotatorFunc, &quatRotatorParams);

        // Get control rotation
        USING_UOBJECT(getControlRotationFunc, UE4::UFunction, "Function Engine.Controller.GetControlRotation")
        auto getControlRotationParams = UE4::GetControlRotationParams();
        playerController->ProcessEvent(getControlRotationFunc, &getControlRotationParams);

        // Set control rotation
        USING_UOBJECT(setControlRotationFunc, UE4::UFunction, "Function Engine.Controller.SetControlRotation")
        UE4::SetControlRotationParams setControlRotationParams;
        setControlRotationParams.NewRotation = UE4::FRotator(
            quatRotatorParams.Result.Pitch,
            getControlRotationParams.Result.Yaw + quatRotatorParams.Result.Yaw - lastRotationInput.Yaw,
            getControlRotationParams.Result.Roll + quatRotatorParams.Result.Roll - lastRotationInput.Roll
        );
        playerController->ProcessEvent(setControlRotationFunc, &setControlRotationParams);
        lastRotationInput = quatRotatorParams.Result;
        
        // Set child rotation
        USING_UOBJECT(setActorRotationFunc, UE4::UFunction, "Function Engine.Actor.K2_SetActorRotation")
        auto setActorRotationParams = UE4::SetActorRotationParams();
        setActorRotationParams.NewRotation = setControlRotationParams.NewRotation;
        childViewTarget->ProcessEvent(setActorRotationFunc, &setActorRotationParams);
    }

    float UE4Manager::Normalize(const float a)
    {
        float b = a;
        while (b <= -180.0f) b += 360.0f;
        while (b > 180.0f) b -= 360.0f;
        return b;
    }
}
