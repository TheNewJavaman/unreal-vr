#include "UE4Service.h"

#include <PatternStreams.h>
#include <Utilities/Globals.h>

#include "HookHelper.h"
#include "OpenXRService.h"
#include "UE4Extensions.h"

#define USING_UOBJECT(ptr, T, name) \
    T* ptr; \
    if (!GetUObject<T>(&(ptr), name)) return;
#define ASSERT(var) \
    if ((var) == nullptr) \
    { \
        return; \
    }
#define ASSERT_LOG(var, name) \
    if ((var) == nullptr) \
    { \
        Log::Warn("[UnrealVR] %s was a nullptr", name); \
        return; \
    }

namespace UnrealVR {
    template<class T>
    bool UE4Service::GetUObject(T** ptr, std::string name) {
        const auto cached = uObjects.find(name);
        if (cached == uObjects.end()) {
            auto found = UE4::UObject::FindObject<T>(name);
            if (found == nullptr) {
                Log::Warn("[UnrealVR] Couldn't find %s", name);
                return false;
            }
            uObjects.insert(std::pair(name, found));
            *ptr = found;
        } else {
            *ptr = static_cast<T*>(cached->second);
        }
        return true;
    }

    void UE4Service::AddHooks() {
        Global::GetGlobals()->eventSystem.registerEvent(new Event<>("InitGameState", &InitGameStateCallback));
        HookCalculateProjectionMatrix();
        HookTick();
    }

    void UE4Service::InitGameStateCallback() {
        playerController = nullptr;
        parentViewTarget = nullptr;
        childViewTarget = nullptr;
        cameraComponent = nullptr;
        lastRot = UE4::FRotator();
        lastLoc = UE4::FVector();
    }

    /** See CalculateProjectionMatrix.asm */
    void UE4Service::HookCalculateProjectionMatrix() {
        const auto match = PS::PatternStream { 0xF6, 0x41, 0x30, 0x01 }
            | PS::PatternInRange({ 0x0F, 0x84, PS::Any, PS::Any, 0x00, 0x00 }, { 0, 80 }, false)
            | PS::PatternInRange({ 0xC3, 0xCC, 0x48, 0x8B, 0xC4 }, { -80, 80 }, true)
            | PS::AddOffset(2)
            | PS::ForEach([](PS::BytePtr& i) {
                Log::Info("[UnrealVR] Found CalculateProjectionMatrixGivenView at 0x%p", i);
            })
            | PS::FirstOrNullptr();
        CalculateProjectionMatrixGivenViewTarget = reinterpret_cast<CalculateProjectionMatrixGivenViewFunc*>(match);
        HookHelper::Add<CalculateProjectionMatrixGivenViewFunc>(
            CalculateProjectionMatrixGivenViewTarget,
            &CalculateProjectionMatrixGivenViewDetour,
            &CalculateProjectionMatrixGivenViewOriginal,
            "CalculateProjectionMatrixGivenView"
        );
        Log::Info("[UnrealVR] Hooked CalculateProjectionMatrixGivenView at 0x%p", match);
    }

    void __cdecl UE4Service::CalculateProjectionMatrixGivenViewDetour(
        void* ViewInfo,
        UE4::TEnumAsByte<UE4::EAspectRatioAxisConstraint> AspectRatioAxisConstraint,
        void* Viewport,
        void* InOutProjectionData
    ) {
        constexpr float zNear = 10.f;
        const float tanU = std::tan(OpenXRService::EyeFOV.angleUp);
        const float tanD = std::tan(OpenXRService::EyeFOV.angleDown);
        const float tanL = std::tan(OpenXRService::EyeFOV.angleLeft);
        const float tanR = std::tan(OpenXRService::EyeFOV.angleRight);
        const float sumRL = tanR + tanL;
        const float sumUD = tanU + tanD;
        const float invRL = 1.f / (tanR - tanL);
        const float invUD = 1.f / (tanU - tanD);
        const auto m = reinterpret_cast<UE4::FMatrix*>(static_cast<uint8_t*>(InOutProjectionData) + 0x50);
        *m = {
            { 2.f * invRL, 0.f, 0.f, 0.f },
            { 0.f, 2.f * invUD, 0.f, 0.f },
            { sumRL * -invRL, sumUD * -invUD, 0.f, 1.f },
            { 0.f, 0.f, zNear, 0.f }
        };
    }

    void UE4Service::HookTick() {
        const auto match = PS::PatternStream { 0x48, 0x8B, 0xC4, 0x55, 0x53, 0x56, 0x57, 0x51, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57 }
            | PS::PatternInRange({ 0x48, 0x8D, 0xA8, 0x38, 0xFE, 0xFF, 0xFF }, { 0, 80 }, false)
            | PS::PatternInRange({ 0x48, 0x81, 0xEC, 0x88, 0x02, 0x00, 0x00 }, { 0, 80 }, false)
            | PS::ForEach([](PS::BytePtr& i) {
                Log::Info("[UnrealVR] Found Tick at 0x%p", i);
            })
            | PS::FirstOrNullptr();
        TickTarget = reinterpret_cast<TickFunc*>(match);
        HookHelper::Add<TickFunc>(
            TickTarget,
            &TickDetour,
            &TickOriginal,
            "Tick"
        );
        Log::Info("[UnrealVR] Hooked Tick at 0x%p", match);
    }

    void UE4Service::TickDetour(void* pUWorld, int32_t TickType, float DeltaSeconds) {
        OpenXRService::SwitchEyes();
        TickOriginal(pUWorld, TickType, DeltaSeconds);
    }

    void UE4Service::SetViewTarget() {
        // Get player controller
        USING_UOBJECT(gameplayStatics, UE4::UObject, "GameplayStatics Engine.Default__GameplayStatics")
        USING_UOBJECT(getPlayerControllerFunc, UE4::UFunction, "Function Engine.GameplayStatics.GetPlayerController")
        auto getPlayerControllerParams = UE4::GetPlayerControllerParams();
        getPlayerControllerParams.WorldContextObject = UE4::UWorld::GetWorld();
        gameplayStatics->ProcessEvent(getPlayerControllerFunc, &getPlayerControllerParams);
        ASSERT(getPlayerControllerParams.Result)
        playerController = getPlayerControllerParams.Result;

        // Get old view target
        USING_UOBJECT(getViewTargetFunc, UE4::UFunction, "Function Engine.Controller.GetViewTarget")
        auto getViewTargetParams = UE4::GetViewTargetParams();
        playerController->ProcessEvent(getViewTargetFunc, &getViewTargetParams);
        ASSERT_LOG(getViewTargetParams.ViewTarget, "Current view target")
        const auto currentViewTarget = getViewTargetParams.ViewTarget;

        if (childViewTarget == nullptr) {
            // Spawn new view target
            USING_UOBJECT(cameraActorClass, UE4::UClass, "Class Engine.CameraActor")
            if (GameProfile::SelectedGameProfile.IsUsingDeferedSpawn) {
                childViewTarget = UE4::UGameplayStatics::BeginDeferredActorSpawnFromClass(
                    cameraActorClass,
                    UE4::FTransform(),
                    UE4::ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
                    nullptr
                );
            } else {
                const auto transform = UE4::FTransform();
                const auto params = UE4::FActorSpawnParameters();
                childViewTarget = UE4::UWorld::GetWorld()->SpawnActor(
                    cameraActorClass,
                    &transform,
                    &params
                );
            }
            ASSERT_LOG(childViewTarget, "Child view target")

            // Get camera component
            USING_UOBJECT(getComponentByClassFunc, UE4::UFunction, "Function Engine.Actor.GetComponentByClass")
            auto getComponentByClassParams = UE4::GetComponentByClassParams();
            USING_UOBJECT(cameraComponentClass, UE4::UClass, "Class Engine.CameraComponent")
            getComponentByClassParams.ComponentClass = cameraComponentClass;
            childViewTarget->ProcessEvent(getComponentByClassFunc, &getComponentByClassParams);
            ASSERT_LOG(getComponentByClassParams.Result, "Camera component")
            cameraComponent = getComponentByClassParams.Result;
        }

        if (currentViewTarget != childViewTarget && difftime(time(nullptr), viewTargetLastChanged) > 1.) {
            parentViewTarget = currentViewTarget;
            lastRot = UE4::FRotator();
            lastLoc = UE4::FVector();

            /*
            // Get player camera manager
            USING_UOBJECT(getPlayerCameraManagerFunc, UE4::UFunction,
                          "Function Engine.GameplayStatics.GetPlayerCameraManager")
            auto getPlayerCameraManagerParams = UE4::GetPlayerCameraManagerParams();
            getPlayerCameraManagerParams.WorldContextObject = UE4::UWorld::GetWorld();
            gameplayStatics->ProcessEvent(getPlayerCameraManagerFunc, &getPlayerCameraManagerParams);
            ASSERT_LOG(getPlayerCameraManagerParams.Result, "Player camera manager")
            const auto playerCameraManager = getPlayerCameraManagerParams.Result;

            // Get current view target's location, including camera offsets
            USING_UOBJECT(getCameraLocationFunc, UE4::UFunction,
                          "Function Engine.PlayerCameraManager.GetCameraLocation")
            auto getCameraLocationParams = UE4::GetCameraLocationParams();
            playerCameraManager->ProcessEvent(getCameraLocationFunc, &getCameraLocationParams);

            // Get positional offset; this is often present due to camera offsets
            USING_UOBJECT(getActorLocationFunc, UE4::UFunction, "Function Engine.Actor.K2_GetActorLocation")
            auto getActorLocationParams = UE4::GetActorLocationParams();
            childViewTarget->ProcessEvent(getActorLocationFunc, &getActorLocationParams);
            positionalOffset = UE4::FVector(
                getCameraLocationParams.Result.X - getActorLocationParams.Result.X,
                getCameraLocationParams.Result.Y - getActorLocationParams.Result.Y,
                getCameraLocationParams.Result.Z - getActorLocationParams.Result.Z
            );
            */

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

            // To prevent excessive flashing, only set the view target at most once per second
            viewTargetLastChanged = time(nullptr);
        }

        if (cameraComponent != nullptr) {
            // Disable aspect ratio constraint (enables letterboxing, minimizes stretching)
            USING_UOBJECT(setConstraintAspectRatioFunc, UE4::UFunction,
                          "Function Engine.CameraComponent.SetConstraintAspectRatio")
            auto setConstraintAspectRatioParams = UE4::SetConstraintAspectRatioParams();
            cameraComponent->ProcessEvent(setConstraintAspectRatioFunc, &setConstraintAspectRatioParams);

            // Set projection mode to perspective so that VR FOV works properly
            USING_UOBJECT(setProjectionModeFunc, UE4::UFunction, "Function Engine.CameraComponent.SetProjectionMode")
            auto setProjectionModeParams = UE4::SetProjectionModeParams();
            cameraComponent->ProcessEvent(setProjectionModeFunc, &setProjectionModeParams);
        }

        if (!Resized) {
            Resize(static_cast<int>(OpenXRService::EyeWidth), static_cast<int>(OpenXRService::EyeHeight));
        }
    }

    void UE4Service::Resize(const int width, const int height) {
        // Get local settings
        USING_UOBJECT(defaultGameUserSettings, UE4::UObject, "GameUserSettings Engine.Default__GameUserSettings")
        USING_UOBJECT(getGameUserSettingsFunc, UE4::UFunction, "Function Engine.GameUserSettings.GetGameUserSettings")
        auto getGameUserSettingsParams = UE4::GetGameUserSettingsParams();
        defaultGameUserSettings->ProcessEvent(getGameUserSettingsFunc, &getGameUserSettingsParams);
        ASSERT_LOG(getGameUserSettingsParams.Result, "Game user settings")
        const auto gameUserSettings = getGameUserSettingsParams.Result;
        bool anySettingChanged = false;

        // Get resolution
        USING_UOBJECT(getScreenResolutionFunc, UE4::UFunction, "Function Engine.GameUserSettings.GetScreenResolution")
        auto getScreenResolutionParams = UE4::GetScreenResolutionParams();
        gameUserSettings->ProcessEvent(getScreenResolutionFunc, &getScreenResolutionParams);

        if (width != getScreenResolutionParams.Result.X || height != getScreenResolutionParams.Result.Y) {
            // Set the resolution
            USING_UOBJECT(setScreenResolutionFunc, UE4::UFunction,
                          "Function Engine.GameUserSettings.SetScreenResolution")
            UE4::SetScreenResolutionParams setScreenResolutionParams;
            setScreenResolutionParams.Resolution.X = width;
            setScreenResolutionParams.Resolution.Y = height;
            gameUserSettings->ProcessEvent(setScreenResolutionFunc, &setScreenResolutionParams);
            anySettingChanged = true;
        }

        // Get fullscreen mode
        USING_UOBJECT(getFullscreenModeFunc, UE4::UFunction, "Function Engine.GameUserSettings.GetFullscreenMode")
        auto getFullscreenModeParams = UE4::GetFullscreenModeParams();
        gameUserSettings->ProcessEvent(getFullscreenModeFunc, &getFullscreenModeParams);

        if (getFullscreenModeParams.Result != UE4::EWindowMode::Fullscreen) {
            // Set to fullscreen mode for better performance
            USING_UOBJECT(setFullscreenModeFunc, UE4::UFunction, "Function Engine.GameUserSettings.SetFullscreenMode")
            auto setFullscreenModeParams = UE4::SetFullscreenModeParams();
            gameUserSettings->ProcessEvent(setFullscreenModeFunc, &setFullscreenModeParams);
            anySettingChanged = true;
        }

        if (anySettingChanged) {
            // Apply the resolution changes
            USING_UOBJECT(applyResolutionSettingsFunc, UE4::UFunction,
                          "Function Engine.GameUserSettings.ApplyResolutionSettings")
            auto applyResolutionSettingsParams = UE4::ApplyResolutionSettingsParams();
            gameUserSettings->ProcessEvent(applyResolutionSettingsFunc, &applyResolutionSettingsParams);
        }

        Resized = true;
        Log::Info("[UnrealVR] Resized render resolution to %dx%d", width, height);
    }

    void UE4Service::UpdatePose(const UE4::FQuat rot, const UE4::FVector loc, const Eye eye) {
        if (playerController == nullptr || childViewTarget == nullptr) {
            return;
        }

        // Convert rot to an FRotator
        USING_UOBJECT(mathLibrary, UE4::UObject, "KismetMathLibrary Engine.Default__KismetMathLibrary")
        USING_UOBJECT(quatRotatorFunc, UE4::UFunction, "Function Engine.KismetMathLibrary.Quat_Rotator")
        auto quatRotatorParams = UE4::QuatRotatorParams();
        quatRotatorParams.Q = rot;
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
            getControlRotationParams.Result.Yaw + quatRotatorParams.Result.Yaw - lastRot.Yaw,
            getControlRotationParams.Result.Roll + quatRotatorParams.Result.Roll - lastRot.Roll
        );
        playerController->ProcessEvent(setControlRotationFunc, &setControlRotationParams);
        lastRot = quatRotatorParams.Result;

        // Set child rotation
        USING_UOBJECT(setActorRotationFunc, UE4::UFunction, "Function Engine.Actor.K2_SetActorRotation")
        auto setActorRotationParams = UE4::SetActorRotationParams();
        setActorRotationParams.NewRotation = setControlRotationParams.NewRotation;
        childViewTarget->ProcessEvent(setActorRotationFunc, &setActorRotationParams);

        // Unrotate location 
        USING_UOBJECT(quatUnrotateVectorFunc, UE4::UFunction, "Function Engine.KismetMathLibrary.Quat_UnrotateVector")
        auto quatUnrotateVectorParams = UE4::QuatUnrotateVectorParams();
        quatUnrotateVectorParams.Q = rot;
        quatUnrotateVectorParams.V = loc;
        mathLibrary->ProcessEvent(quatUnrotateVectorFunc, &quatUnrotateVectorParams);

        // Apply IPD and positional tracking
        USING_UOBJECT(setActorRelativeLocationFunc, UE4::UFunction, "Function Engine.Actor.K2_SetActorRelativeLocation")
        auto setActorRelativeLocationParams = UE4::SetActorRelativeLocationParams();
        setActorRelativeLocationParams.RelativeLocation = UE4::FVector(
            (quatUnrotateVectorParams.Result.X - lastLoc.X) * 100.f * CmUnitsScale,
            (quatUnrotateVectorParams.Result.Y - lastLoc.Y) * 100.f * CmUnitsScale,
            (quatUnrotateVectorParams.Result.Z - lastLoc.Z) * 100.f * CmUnitsScale
        );
        lastLoc = quatUnrotateVectorParams.Result;
        childViewTarget->ProcessEvent(setActorRelativeLocationFunc, &setActorRelativeLocationParams);
    }
}
