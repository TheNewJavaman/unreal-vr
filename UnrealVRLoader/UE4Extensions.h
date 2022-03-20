#pragma once

#include "Ue4.hpp"

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

    struct GetAttachParentActorParams
    {
        AActor* Result = nullptr;
    };
    
    struct GetRootComponentParams
    {
        UObject* RootComponent = nullptr;  
    };

    struct AddActorWorldOffsetParams
    {
        
    };
}
