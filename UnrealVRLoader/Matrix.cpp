#include "Matrix.h"

#include <math.h>

namespace UnrealVR
{
    Matrix Matrix::Identity()
    {
        Matrix m;

        m.M[0][0] = 1.f; m.M[0][1] = 0.f;  m.M[0][2] = 0.f;  m.M[0][3] = 0.f;
        m.M[1][0] = 0.f; m.M[1][1] = 1.f;  m.M[1][2] = 0.f;  m.M[1][3] = 0.f;
        m.M[2][0] = 0.f; m.M[2][1] = 0.f;  m.M[2][2] = 1.f;  m.M[2][3] = 0.f;
        m.M[3][0] = 0.f; m.M[3][1] = 0.f;  m.M[3][2] = 0.f;  m.M[3][3] = 1.f;

        return m;
    }

    
    Matrix Matrix::FromTranslation(const float deltaX, const float deltaY, const float deltaZ)
    {
        Matrix m = Identity();
        
        m.M[3][0] = deltaX;
        m.M[3][1] = deltaY;
        m.M[3][2] = deltaZ;
        
        return m;
    }

    Matrix Matrix::FromRotation(const float radRoll, const float radPitch, const float radYaw)
    {
        Matrix m = Identity();
        
        const float SR = sin(radRoll);
        const float SP = sin(radPitch);
        const float SY = sin(radYaw);
        const float CR = cos(radRoll);
        const float CP = cos(radPitch);
        const float CY = cos(radYaw);

        m.M[0][0] = CP * CY;
        m.M[0][1] = CP * SY;
        m.M[0][2] = SP;

        m.M[1][0] = SR * SP * CY - CR * SY;
        m.M[1][1] = SR * SP * SY + CR * CY;
        m.M[1][2] = - SR * CP;

        m.M[2][0] = -(CR * SP * CY + SR * SY);
        m.M[2][1] = CY * SR - CR * SP * SY;
        m.M[2][2] = CR * CP;

        return m;
    }

    Matrix Matrix::operator*(const Matrix b) const
    {
        Matrix c;
        
        c.M[0][0] = M[0][0] * b.M[0][0] + M[0][1] * b.M[1][0] + M[0][2] * b.M[2][0] + M[0][3] * b.M[3][0];
        c.M[0][1] = M[0][0] * b.M[0][1] + M[0][1] * b.M[1][1] + M[0][2] * b.M[2][1] + M[0][3] * b.M[3][1];
        c.M[0][2] = M[0][0] * b.M[0][2] + M[0][1] * b.M[1][2] + M[0][2] * b.M[2][2] + M[0][3] * b.M[3][2];
        c.M[0][3] = M[0][0] * b.M[0][3] + M[0][1] * b.M[1][3] + M[0][2] * b.M[2][3] + M[0][3] * b.M[3][3];

        c.M[1][0] = M[1][0] * b.M[0][0] + M[1][1] * b.M[1][0] + M[1][2] * b.M[2][0] + M[1][3] * b.M[3][0];
        c.M[1][1] = M[1][0] * b.M[0][1] + M[1][1] * b.M[1][1] + M[1][2] * b.M[2][1] + M[1][3] * b.M[3][1];
        c.M[1][2] = M[1][0] * b.M[0][2] + M[1][1] * b.M[1][2] + M[1][2] * b.M[2][2] + M[1][3] * b.M[3][2];
        c.M[1][3] = M[1][0] * b.M[0][3] + M[1][1] * b.M[1][3] + M[1][2] * b.M[2][3] + M[1][3] * b.M[3][3];

        c.M[2][0] = M[2][0] * b.M[0][0] + M[2][1] * b.M[1][0] + M[2][2] * b.M[2][0] + M[2][3] * b.M[3][0];
        c.M[2][1] = M[2][0] * b.M[0][1] + M[2][1] * b.M[1][1] + M[2][2] * b.M[2][1] + M[2][3] * b.M[3][1];
        c.M[2][2] = M[2][0] * b.M[0][2] + M[2][1] * b.M[1][2] + M[2][2] * b.M[2][2] + M[2][3] * b.M[3][2];
        c.M[2][3] = M[2][0] * b.M[0][3] + M[2][1] * b.M[1][3] + M[2][2] * b.M[2][3] + M[2][3] * b.M[3][3];

        c.M[3][0] = M[3][0] * b.M[0][0] + M[3][1] * b.M[1][0] + M[3][2] * b.M[2][0] + M[3][3] * b.M[3][0];
        c.M[3][1] = M[3][0] * b.M[0][1] + M[3][1] * b.M[1][1] + M[3][2] * b.M[2][1] + M[3][3] * b.M[3][1];
        c.M[3][2] = M[3][0] * b.M[0][2] + M[3][1] * b.M[1][2] + M[3][2] * b.M[2][2] + M[3][3] * b.M[3][2];
        c.M[3][3] = M[3][0] * b.M[0][3] + M[3][1] * b.M[1][3] + M[3][2] * b.M[2][3] + M[3][3] * b.M[3][3];

        return c;
    }
}
