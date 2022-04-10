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
}
