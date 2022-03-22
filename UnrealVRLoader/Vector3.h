#pragma once

namespace UnrealVR
{
    class Vector3
    {
    public:
        Vector3();
        Vector3(float x, float y, float z);

        float X;
        float Y;
        float Z;

        Vector3 operator-(Vector3 other) const;
    };
}
