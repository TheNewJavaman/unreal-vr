#include "UE4Extensions.h"

#define PI 3.141592653589793f

namespace UE4
{
    FQuat RotatorToQuaternion(const FRotator R)
    {
        constexpr float DEG_TO_RAD = PI / 180.f;
        constexpr float RADS_DIVIDED_BY_2 = DEG_TO_RAD / 2.f;

        const float PitchNoWinding = std::remainder(R.Pitch, 360.0f);
        const float YawNoWinding = std::remainder(R.Yaw, 360.0f);
        const float RollNoWinding = std::remainder(R.Roll, 360.0f);

        const float SP = std::sin(PitchNoWinding * RADS_DIVIDED_BY_2);
        const float SY = std::sin(YawNoWinding * RADS_DIVIDED_BY_2);
        const float SR = std::sin(RollNoWinding * RADS_DIVIDED_BY_2);
        const float CP = std::cos(PitchNoWinding * RADS_DIVIDED_BY_2);
        const float CY = std::cos(YawNoWinding * RADS_DIVIDED_BY_2);
        const float CR = std::cos(RollNoWinding * RADS_DIVIDED_BY_2);

        FQuat RotationQuat;
        RotationQuat.X = CR * SP * SY - SR * CP * CY;
        RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
        RotationQuat.Z = CR * CP * SY - SR * SP * CY;
        RotationQuat.W = CR * CP * CY + SR * SP * SY;

        return RotationQuat;
    }

    void VectorMatrixMultiply(void* Result, const void* Matrix1, const void* Matrix2)
    {
        typedef float Float4x4[4][4];
        const Float4x4& A = *((const Float4x4*)Matrix1);
        const Float4x4& B = *((const Float4x4*)Matrix2);
        Float4x4 Temp;
        Temp[0][0] = A[0][0] * B[0][0] + A[0][1] * B[1][0] + A[0][2] * B[2][0] + A[0][3] * B[3][0];
        Temp[0][1] = A[0][0] * B[0][1] + A[0][1] * B[1][1] + A[0][2] * B[2][1] + A[0][3] * B[3][1];
        Temp[0][2] = A[0][0] * B[0][2] + A[0][1] * B[1][2] + A[0][2] * B[2][2] + A[0][3] * B[3][2];
        Temp[0][3] = A[0][0] * B[0][3] + A[0][1] * B[1][3] + A[0][2] * B[2][3] + A[0][3] * B[3][3];

        Temp[1][0] = A[1][0] * B[0][0] + A[1][1] * B[1][0] + A[1][2] * B[2][0] + A[1][3] * B[3][0];
        Temp[1][1] = A[1][0] * B[0][1] + A[1][1] * B[1][1] + A[1][2] * B[2][1] + A[1][3] * B[3][1];
        Temp[1][2] = A[1][0] * B[0][2] + A[1][1] * B[1][2] + A[1][2] * B[2][2] + A[1][3] * B[3][2];
        Temp[1][3] = A[1][0] * B[0][3] + A[1][1] * B[1][3] + A[1][2] * B[2][3] + A[1][3] * B[3][3];

        Temp[2][0] = A[2][0] * B[0][0] + A[2][1] * B[1][0] + A[2][2] * B[2][0] + A[2][3] * B[3][0];
        Temp[2][1] = A[2][0] * B[0][1] + A[2][1] * B[1][1] + A[2][2] * B[2][1] + A[2][3] * B[3][1];
        Temp[2][2] = A[2][0] * B[0][2] + A[2][1] * B[1][2] + A[2][2] * B[2][2] + A[2][3] * B[3][2];
        Temp[2][3] = A[2][0] * B[0][3] + A[2][1] * B[1][3] + A[2][2] * B[2][3] + A[2][3] * B[3][3];

        Temp[3][0] = A[3][0] * B[0][0] + A[3][1] * B[1][0] + A[3][2] * B[2][0] + A[3][3] * B[3][0];
        Temp[3][1] = A[3][0] * B[0][1] + A[3][1] * B[1][1] + A[3][2] * B[2][1] + A[3][3] * B[3][1];
        Temp[3][2] = A[3][0] * B[0][2] + A[3][1] * B[1][2] + A[3][2] * B[2][2] + A[3][3] * B[3][2];
        Temp[3][3] = A[3][0] * B[0][3] + A[3][1] * B[1][3] + A[3][2] * B[2][3] + A[3][3] * B[3][3];
        memcpy(Result, &Temp, 16 * sizeof(float));
    }
}
