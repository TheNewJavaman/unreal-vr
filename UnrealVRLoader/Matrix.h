#pragma once

/**
 * These structs should be interoperable with UE4's math structs. 
 * At runtime, casting between them should be feasible, and any operations to one should have the same effect on the other. 
 */
namespace UnrealVR
{
    struct Matrix
    {
        __declspec(align(16)) float M[4][4];

        static Matrix Identity();

        static Matrix FromTranslation(float deltaX, float deltaY, float deltaZ);

        static Matrix FromRotation(float radRoll, float radPitch, float radYaw);

        Matrix operator*(Matrix b) const;
    };
}
