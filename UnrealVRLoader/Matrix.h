#pragma once


namespace UnrealVR
{
    /**
     * This struct should be interoperable with Unreal Engine's FMatrix and DirectXMath's XMMatrix. At runtime, casting
     * between them should be feasible, and any operations to one should have the same effect on the other
     *
     * Several of these functions have the same logic as UE, which is intentional. Parity is key!
     */
    struct Matrix
    {
        // TODO: Double-check that the alignment is all correct. Does alignment even matter here?
        __declspec(align(16)) float M[4][4];

        static Matrix Identity();
        
        static Matrix FromTranslation(float deltaX, float deltaY, float deltaZ);

        static Matrix FromRotation(float radRoll, float radPitch, float radYaw);

        Matrix operator*(Matrix b) const;
    };
}
