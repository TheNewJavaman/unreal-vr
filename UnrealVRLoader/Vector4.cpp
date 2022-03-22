#include "Vector4.h"

namespace UnrealVR
{
    Vector4::Vector4()
    {
        X = 0.0f;
        Y = 0.0f;
        Z = 0.0f;
        W = 0.0f;
    }

    Vector4::Vector4(const float x, const float y, const float z, const float w)
    {
        X = x;
        Y = y;
        Z = z;
        W = w;
    }
}
