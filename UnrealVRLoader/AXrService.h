#pragma once

#include "AService.h"

namespace UnrealVr {
    struct Vector3 {
        float x, y, z;
    };

    struct Vector4 {
        float x, y, z, w;
    };

    struct Directional2D {
        float left, up, right, down;
    };

    /**
     * Describes the configuration for a view
     *
     * UnrealVR's coordinate system (follows the Unreal Engine specification):
     *        | +z
     *        |
     *        |
     *        +-------- +y
     *       /
     *      /
     *     / -x
     */
    struct Pose {
        /** Location/delta from origin in meters */
        Vector3 position;

        /** Quaternion */
        Vector4 rotation;

        /** Visible field of view in each direction in radians */
        Directional2D fov;
    };

    enum class Eye : uint8_t {
        Left = 0,
        Right = 1
    };
    
    /**
     * Defines required functionality for virtual reality runtimes
     */
    class AXrService : public AService {
    public:
        virtual Directional2D GetPose(Eye eye);
        virtual ErrorCode SubmitFrame(const std::shared_ptr<APresentParams>& presentParams, Eye eye);
    };
}
