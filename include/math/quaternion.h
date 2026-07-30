#pragma once

#include "math/matrix.h"
#include "math/vector.h"

#include "typedefs.h"


namespace Math {


// ========================================================================== //


class Quaternion {
public:
    static constexpr f32 sEpsilon = 1e-8f;

    constexpr Quaternion() :
        mW{1.0f}, mX{}, mY{}, mZ{} {}

    constexpr Quaternion(f32 w, f32 x, f32 y, f32 z) :
        mW{w}, mX{x}, mY{y}, mZ{z} {}

    Quaternion(const Quaternion&) = default;
    Quaternion(Quaternion&&) = default;

    Quaternion& operator=(const Quaternion&) = default;
    Quaternion& operator=(Quaternion&&) = default;

    f32 lengthSquared() const;
    f32 length() const;

    Quaternion operator/(f32 s) const;

    Quaternion normalized() const;

    static Quaternion fromMatrix(const Matrix33f& mtx);
    Matrix33f toMatrix() const;

    static Quaternion fromEulerXYZ(const Vector3f& eulerDeg);
    Vector3f toEulerXYZ() const;

private:
    f32 mW;
    f32 mX;
    f32 mY;
    f32 mZ;
};


// ========================================================================== //


} // namespace Math
