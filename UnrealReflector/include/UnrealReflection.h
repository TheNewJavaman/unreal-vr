#pragma once

#include <cstdint>

template<typename Raw_t>
class ACacheable {
public:
    typedef Raw_t* Target_t;
    Target_t target = nullptr;
    virtual bool RefreshCache() = 0;
};

template<typename Target_t, typename Parent_t>
class ACacheableMember : public ACacheable<Target_t> {
public:
    ACacheableMember(Parent_t* parent) : parent(parent) {}
    Parent_t* parent;
};

/**
 * Definitions for useful Unreal Engine structs, classes, enums, etc.
 *
 * Most definitions are incomplete; do not use a raw class instantiation (i.e., without "new") if the definition is
 * marked as "Members incomplete". Additionally, assume that all classes are functionally incomplete.
 */
namespace UE {
    class UFunction;

    class UObject {
    public:
        class ProcessEvent_t : public ACacheableMember<void __cdecl(UObject*, UFunction*, void*), UObject> {
        public:
            ProcessEvent_t(UObject* parent) : ACacheableMember(parent) {}
            bool RefreshCache() override;
            void operator ()(UFunction* Function, void* Params);
        } ProcessEvent = { this };
    };

    class UFunction : public UObject {};

    class FVector {
    public:
        FVector() : X(0), Y(0), Z(0) {}
        FVector(const float x, const float y, const float z) : X(x), Y(y), Z(z) {}

        float X;
        float Y;
        float Z;
    };

    alignas(16) class FPlane : public FVector {
    public:
        FPlane() : FVector(), W(0) {}
        FPlane(const float x, const float y, const float z, const float w) : FVector(x, y, z), W(w) {}

        float W;
    };

    class FMatrix {
    public:
        FMatrix() : M {} {}

        FMatrix(const FPlane x, const FPlane y, const FPlane z, const FPlane w) : M {
            { x.X, x.Y, x.Y, x.W },
            { y.X, y.Y, y.Y, y.W },
            { z.X, z.Y, z.Y, z.W },
            { w.X, w.Y, w.Y, w.W }
        } {}

        alignas(16) float M[4][4];
    };

    /** Members incomplete */
    class FSceneViewProjectionData {};

    /** Members incomplete */
    class FViewport {};

    template<typename TEnum>
    class TEnumAsByte {
    public:
        uint8_t Value;
    };

    enum class EAspectRatioAxisConstraint {
        AspectRatio_MaintainYFOV,
        AspectRatio_MaintainXFOV,
        AspectRatio_MajorAxisFOV,
        AspectRatio_MAX
    };

    /** Members incomplete */
    class FMinimalViewInfo {
    public:
        class CalculateProjectionMatrixGivenView_t {
        public:
            typedef void (__cdecl Target_t)(
                FMinimalViewInfo*,
                FViewport* Viewport,
                TEnumAsByte<EAspectRatioAxisConstraint>,
                FSceneViewProjectionData* InOutProjectionData
            );
        } CalculateProjectionMatrixGivenView;
    };
}