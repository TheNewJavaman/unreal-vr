#include "Vector3.h"

namespace UnrealVR
{
    Vector3::Vector3()
    {
        X = 0.0f;
        Y = 0.0f;
        Z = 0.0f;
    }

    Vector3::Vector3(const float x, const float y, const float z)
    {
        X = x;
        Y = y;
        Z = z;
    }

    Vector3 Vector3::operator-(const Vector3 other) const
    {
        return {X - other.X, Y - other.Y, Z - other.Z};
    }
}
