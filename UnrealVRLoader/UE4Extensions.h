#pragma once

#include "Ue4.hpp"

namespace UE4
{
    struct GetViewTargetParams
    {
        AActor* ViewTarget = nullptr;
    };

    struct AttachToActorParams
    {
        AActor* ParentActor = nullptr;
        FName SocketName = FName();
        UINT8 LocationRule = 2;
        UINT8 RotationRule = 2;
        UINT8 ScaleRule = 2;
        bool bWeldSimulatedBodies = false;
    };

    struct SetViewTargetWithBlendParams
    {
        AActor* NewViewTarget = nullptr;
        float BlendTime = 0.0f;
        int BlendFunc = 0;
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
}
