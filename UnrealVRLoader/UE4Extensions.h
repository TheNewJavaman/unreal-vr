#pragma once

#include "Ue4.hpp"

// TODO: Write full structs/classes for undefined types instead of using generic UObjects
// TODO: Sort these structs
namespace UE4
{
    struct GetViewTargetParams
    {
        AActor* ViewTarget = nullptr;
    };

    enum class EAttachmentRule : UINT8
    {
        KeepRelative,
        KeepWorld,
        SnapToTarget
    };

    struct AttachToActorParams
    {
        AActor* ParentActor = nullptr;
        FName SocketName = 0; // NAME_None
        EAttachmentRule LocationRule = EAttachmentRule::SnapToTarget;
        EAttachmentRule RotationRule = EAttachmentRule::SnapToTarget;
        EAttachmentRule ScaleRule = EAttachmentRule::SnapToTarget;
        bool bWeldSimulatedBodies = true;
    };

    enum EViewTargetBlendFunction
    {
        VTBlend_Linear,
        VTBlend_Cubic,
        VTBlend_EaseIn,
        VTBlend_EaseOut,
        VTBlend_EaseInOut,
        VTBlend_MAX,
    };

    struct SetViewTargetWithBlendParams
    {
        AActor* NewViewTarget = nullptr;
        float BlendTime = 0.0f;
        EViewTargetBlendFunction BlendFunc = VTBlend_Linear;
        float BlendExp = 0.0f;
        bool bLockOutgoing = false;
    };

    struct AddActorLocalOffsetParams
    {
        FVector DeltaLocation = FVector();
        bool bSweep = false;
        void* SweepHitResult = nullptr;
        bool bTeleport = true;
    };

    struct SetActorRotationParams
    {
        FRotator NewRotation = FRotator();
        bool bTeleportPhysics = true;
    };

    struct GetGameUserSettingsParams
    {
        UObject* Result = nullptr;
    };

    struct SetScreenResolutionParams
    {
        FIntPoint Resolution = FIntPoint();
    };

    struct ApplyResolutionSettingsParams
    {
        bool bCheckForCommandLineOverrides = false;
    };

    struct AddActorWorldOffsetParams
    {
        FVector DeltaLocation = FVector();
        bool bSweep = false;
        void* SweepHitResult = nullptr;
        bool bTeleport = true;
    };

    struct GetComponentByClassParams
    {
        TSubclassOf<UObject> ComponentClass = TSubclassOf<UObject>(UObject::StaticClass());
        UObject* Result = nullptr;
    };

    struct SetFieldOfViewParams
    {
        float InFieldOfView = 90.0f;
    };

    struct SetConstraintAspectRatioParams
    {
        bool bInConstrainAspectRatio = false;
    };

    namespace EWindowMode
    {
        enum Type
        {
            Fullscreen,
            WindowedFullscreen,
            Windowed,
        };
    }

    struct SetFullscreenModeParams
    {
        EWindowMode::Type InFullscreenMode = EWindowMode::Fullscreen;
    };

    struct SetActorRelativeRotationParams
    {
        FRotator NewRelativeRotation = FRotator();
        bool bSweep = false;
        void* SweepHitResult = nullptr;
        bool bTeleport = true;
    };

    struct QuatRotatorParams
    {
        FQuat Q = FQuat();
        FRotator Result = FRotator();
    };

    struct GetRootComponentParams
    {
        UObject* Result = nullptr;
    };

    struct SetAbsoluteParams
    {
        bool bNewAbsoluteLocation = false;
        bool bNewAbsoluteRotation = false;
        bool bNewAbsoluteScale = false;
    };

    struct GetControlRotationParams
    {
        FRotator Result = FRotator();
    };

    struct GetPlayerCameraManagerParams
    {
        UObject* WorldContextObject = nullptr;
        int PlayerIndex = 0;
        UObject* Result = nullptr;
    };

    struct GetCameraLocationParams
    {
        FVector Result = FVector();
    };
}
