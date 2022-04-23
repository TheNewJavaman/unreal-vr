#include "UE4Service.h"

#include <PatternStreams.h>
#include <Utilities/Globals.h> // TODO: Get Russell to use PatternStreams

#include "HookHelper.h"
#include "OpenXRService.h"
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
        RegisterInitGameStateEvent();
        HookCalculateProjectionMatrix();
    }

    void UE4Service::RegisterInitGameStateEvent() {
        Global::GetGlobals()->eventSystem.registerEvent(new Event<>("InitGameState", &InitGameStateCallback));
    }

    void UE4Service::InitGameStateCallback() {
        if (!GameLoaded)
            GameLoaded = true;
        playerController = nullptr;
        parentViewTarget = nullptr;
        childViewTarget = nullptr;
        cameraComponent = nullptr;
        gameUserSettings = nullptr;
        lastRot = UE4::FRotator();
    }

    /** See CalculateProjectionMatrix.asm */
    void UE4Service::HookCalculateProjectionMatrix() {
        const auto match = PS::PatternStream { 0xF6, 0x41, 0x30, 0x01 }
            | PS::PatternInRange({ 0x0F, 0x84, PS::Any, PS::Any, 0x00, 0x00 }, { 0, 80 }, false)
            | PS::PatternInRange({ 0xC3, 0xCC, 0x48, 0x8B, 0xC4 }, { -80, 80 }, true)
            | PS::AddOffset(2)
            | PS::ForEach([](PS::BytePtr& i) {
                Log::Info("[UnrealVR] Found CalculateProjectionMatrixGivenView at %p", i);
            })
            | PS::FirstOrNullptr();
        CalculateProjectionMatrixGivenViewTarget = reinterpret_cast<CalculateProjectionMatrixGivenViewFunc*>(match);
        HookHelper::Add<CalculateProjectionMatrixGivenViewFunc>(
            CalculateProjectionMatrixGivenViewTarget,
            &CalculateProjectionMatrixGivenViewDetour,
            &CalculateProjectionMatrixGivenViewOriginal,
            "CalculateProjectionMatrixGivenView"
        );
        Log::Info("[UnrealVR] Hooked CalculateProjectionMatrixGivenView at %p", match);
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
        const auto m = reinterpret_cast<UE4::FMatrix*>(reinterpret_cast<intptr_t>(InOutProjectionData) + 0x50);
        *m = {
            { 2.f * invRL, 0.f, 0.f, 0.f },
            { 0.f, 2.f * invUD, 0.f, 0.f },
            { sumRL * -invRL, sumUD * -invUD, 0.f, 1.f },
            { 0.f, 0.f, zNear, 0.f }
        };
    }

    void UE4Service::SetViewTarget() {
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

        if (currentViewTarget != childViewTarget) {
            parentViewTarget = currentViewTarget;
            lastRot = UE4::FRotator();

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

        if (cameraComponent != nullptr) {
            // Disable aspect ratio constraint (enables letterboxing, minimizes stretching)
            USING_UOBJECT(setConstraintAspectRatioFunc, UE4::UFunction,
                          "Function Engine.CameraComponent.SetConstraintAspectRatio")
            auto setConstraintAspectRatioParams = UE4::SetConstraintAspectRatioParams();
            cameraComponent->ProcessEvent(setConstraintAspectRatioFunc, &setConstraintAspectRatioParams);

            // Set projection mode to perspective so that VR FOV works properly
            USING_UOBJECT(setProjectionModeFunc, UE4::UFunction, "Function Engine.CameraComponent.SetProjectionMode");
            auto setProjectionModeParams = UE4::SetProjectionModeParams();
            cameraComponent->ProcessEvent(setProjectionModeFunc, &setProjectionModeParams);
        }
    }

    void UE4Service::Resize(const int width, const int height) {
        // Get local settings
        USING_UOBJECT(defaultGameUserSettings, UE4::UObject, "GameUserSettings Engine.Default__GameUserSettings")
        USING_UOBJECT(getGameUserSettingsFunc, UE4::UFunction, "Function Engine.GameUserSettings.GetGameUserSettings")
        auto getGameUserSettingsParams = UE4::GetGameUserSettingsParams();
        defaultGameUserSettings->ProcessEvent(getGameUserSettingsFunc, &getGameUserSettingsParams);
        ASSERT_ASSIGN(getGameUserSettingsParams.Result, gameUserSettings)

        // Set the resolution, also finish initializing VR
        USING_UOBJECT(setScreenResolutionFunc, UE4::UFunction, "Function Engine.GameUserSettings.SetScreenResolution")
        UE4::SetScreenResolutionParams setScreenResolutionParams;
        setScreenResolutionParams.Resolution.X = width;
        setScreenResolutionParams.Resolution.Y = height;
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

        Resized = true;
        Log::Info("[UnrealVR] Resized render resolution to %dx%d", width, height);
    }

    //void UE4Manager::UpdatePose(const UE4::FVector loc, const UE4::FQuat rot, const UE4::FVector loc2)
    void UE4Service::UpdatePose(const UE4::FQuat rot, const Eye eye) {
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

        /*
        // Unrotate IPD
        USING_UOBJECT(quatUnrotateVectorFunc, UE4::UFunction, "Function Engine.KismetMathLibrary.Quat_UnrotateVector")
        UE4::QuatRotateVectorParams quatUnrotateVectorParams;
        quatUnrotateVectorParams.Q = RotatorToQuaternion(
            UE4::FRotator(
                -quatRotatorParams.Result.Pitch,
                -quatRotatorParams.Result.Yaw,
                -quatRotatorParams.Result.Roll
            )
        );
        quatUnrotateVectorParams.V = UE4::FVector(
            (loc.X - loc2.X) * CmUnitsScale / 2.f,
            (loc.Y - loc2.Y) * CmUnitsScale / 2.f,
            (loc.Z - loc2.Z) * CmUnitsScale / 2.f
        );
        mathLibrary->ProcessEvent(quatUnrotateVectorFunc, &quatUnrotateVectorParams);
        */

        // Apply IPD by setting the child's relative location
        USING_UOBJECT(setActorRelativeLocationFunc, UE4::UFunction, "Function Engine.Actor.K2_SetActorRelativeLocation")
        auto setActorRelativeLocationParams = UE4::SetActorRelativeLocationParams();
        setActorRelativeLocationParams.RelativeLocation = UE4::FVector(0.f, eye == Eye::Left ? -3.15f : 3.15f, 0.f);
        childViewTarget->ProcessEvent(setActorRelativeLocationFunc, &setActorRelativeLocationParams);

        /*
        // Convert the yaw control rotation to a quat
        constexpr float C = PI / 180.f / 2.f;
        const float YawNoWinding = std::remainderf(setControlRotationParams.NewRotation.Yaw, 360.0f);
        const auto RotationQuat = UE4::FQuat(0.f, 0.f, sin(C * YawNoWinding), cos(C * YawNoWinding));

        // Rotate loc according to the yaw rotation of the view target
        USING_UOBJECT(quatRotateVectorFunc, UE4::UFunction, "Function Engine.KismetMathLibrary.Quat_RotateVector")
        UE4::QuatRotateVectorParams quatRotateVectorParams;
        quatRotateVectorParams.Q = RotationQuat;
        quatRotateVectorParams.V = eyeCenterLoc;
        //mathLibrary->ProcessEvent(quatRotateVectorFunc, &quatRotateVectorParams);

        // Add eye center delta loc to parent
        USING_UOBJECT(addActorWorldOffsetFunc, UE4::UFunction, "Function Engine.Actor.K2_AddActorWorldOffset")
        auto addActorWorldOffsetParams = UE4::AddActorWorldOffsetParams();
        addActorWorldOffsetParams.DeltaLocation = UE4::FVector(
            (quatRotateVectorParams.V.X - lastLoc.X) * CmUnitsScale,
            (quatRotateVectorParams.V.Y - lastLoc.Y) * CmUnitsScale,
            (quatRotateVectorParams.V.Z - lastLoc.Z) * CmUnitsScale
        );
        //parentViewTarget->ProcessEvent(addActorWorldOffsetFunc, &addActorWorldOffsetParams);
        lastLoc = quatRotateVectorParams.V;
        */
    }
}
