#pragma once

#include <cstdint>

namespace UE {
    /**
     * @version 4.0 - 4.27
     */
    class FVector_0 {
    public:
        FVector_0() : X(0), Y(0), Z(0) {}
        FVector_0(const float x, const float y, const float z) : X(x), Y(y), Z(z) {}

        float X;
        float Y;
        float Z;
    };

    /**
     * @version 5.0 - Latest (5.0)
     */
    template<typename T>
    class TVector_0 {
    public:
        TVector_0() : X(T()), Y(T()), Z(T()) {}
        TVector_0(const T x, const T y, const T z) : X(x), Y(y), Z(z) {}

        T X;
        T Y;
        T Z;
    };

    /**
     * @tparam T
     * @version 5.0 - Latest (5.0)
     */
    template<typename T>
    class alignas(16) TPlane_0 : public TVector_0<T> {
    public:
        TPlane_0() : TVector_0<T>(), W(T()) {}
        TPlane_0(const T x, const T y, const T z, const T w) : TVector_0<T>(x, y, z), W(w) {}

        T W;
    };

    /**
     * @version 4.0 - 4.27
     */
    class alignas(16) FPlane_0 : public FVector_0 {
    public:
        FPlane_0() : FVector_0(), W(0) {}
        FPlane_0(const float x, const float y, const float z, const float w) : FVector_0(x, y, z), W(w) {}

        float W;
    };

    /**
     * @version 4.0 - 4.27
     */
    class FMatrix_0 {
    public:
        FMatrix_0() : M {} {}
        FMatrix_0(const FPlane_0 x, const FPlane_0 y, const FPlane_0 z, const FPlane_0 w) : M {
            { x.X, x.Y, x.Z, x.W },
            { y.X, y.Y, y.Z, y.W },
            { z.X, z.Y, z.Z, z.W },
            { w.X, w.Y, w.Z, w.W }
        } {}

        alignas(16) float M[4][4];
    };

    /**
     * @tparam NumRows
     * @tparam NumColumns
     * @version 4.0 - 4.27
     */
    template<uint32_t NumRows, uint32_t NumColumns>
    class TMatrix_0 {
    public:
        TMatrix_0() : M {} {}
        TMatrix_0(const FPlane_0 x, const FPlane_0 y, const FPlane_0 z, const FPlane_0 w) : M {
            { x.X, x.Y, x.Z, x.W },
            { y.X, y.Y, y.Z, y.W },
            { z.X, z.Y, z.Z, z.W },
            { w.X, w.Y, w.Z, w.W }
        } {}

        alignas(16) float M[NumRows][NumColumns];
    };

    /**
     * @tparam T
     * @version 5.0 - Latest (5.0)
     */
    template<typename T>
    class TMatrix_1 {
    public:
        TMatrix_1() : M {} {}
        TMatrix_1(const FPlane_0 x, const FPlane_0 y, const FPlane_0 z, const FPlane_0 w) : M {
            { x.X, x.Y, x.Z, x.W },
            { y.X, y.Y, y.Z, y.W },
            { z.X, z.Y, z.Z, z.W },
            { w.X, w.Y, w.Z, w.W }
        } {}

        alignas(16) T M[4][4];
    };
}
