#include "Vector3.h"

namespace UnrealVR
{
    Vector3::Vector3()
    {
        X = 0.0f;
        Y = 0.0f;
        Z = 0.0f;
    }

    Vector3::Vector3(float x, float y, float z)
    {
        X = x;
        Y = y;
        Z = z;
    }
}
