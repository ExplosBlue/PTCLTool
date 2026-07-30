#include "math/quaternion.h"

#include <algorithm>
#include <cmath>
#include <numbers>


namespace Math {


// ========================================================================== //


constexpr f32 Quaternion::sEpsilon;

f32 Quaternion::lengthSquared() const {
    return mW * mW + mX * mX + mY * mY + mZ * mZ;
}

f32 Quaternion::length() const {
    return std::sqrt(lengthSquared());
}

Quaternion Quaternion::operator/(f32 s) const {
    return {mW / s, mX / s, mY / s, mZ / s};
}

Quaternion Quaternion::normalized() const {
    const f32 len = length();
    if (len < sEpsilon) { return {}; }
    return *this / len;
}

Quaternion Quaternion::fromMatrix(const Matrix33f& mtx) {
    const f32 m00 = mtx(0, 0), m01 = mtx(0, 1), m02 = mtx(0, 2);
    const f32 m10 = mtx(1, 0), m11 = mtx(1, 1), m12 = mtx(1, 2);
    const f32 m20 = mtx(2, 0), m21 = mtx(2, 1), m22 = mtx(2, 2);

    const f32 trace = m00 + m11 + m22;

    Quaternion q;

    if (trace > 0.0f) {
        const f32 s = 0.5f / std::sqrt(trace + 1.0f);
        q = Quaternion{
            0.25f / s,
            (m21 - m12) * s,
            (m02 - m20) * s,
            (m10 - m01) * s
        };
    } else if (m00 > m11 && m00 > m22) {
        const f32 s = 2.0f * std::sqrt(1.0f + m00 - m11 - m22);
        q = Quaternion{
            (m21 - m12) / s,
            s / 4.0f,
            (m01 + m10) / s,
            (m02 + m20) / s
        };
    } else if (m11 > m22) {
        const f32 s = 2.0f * std::sqrt(1.0f + m11 - m00 - m22);
        q = Quaternion{
            (m02 - m20) / s,
            (m01 + m10) / s,
            s / 4.0f,
            (m12 + m21) / s
        };
    } else {
        const f32 s = 2.0f * std::sqrt(1.0f + m22 - m00 - m11);
        q = Quaternion{
            (m10 - m01) / s,
            (m02 + m20) / s,
            (m12 + m21) / s,
            s / 4.0f
        };
    }

    return q.normalized();
}

Matrix33f Quaternion::toMatrix() const {
    const f32 xx = mX * mX, yy = mY * mY, zz = mZ * mZ;
    const f32 xy = mX * mY, xz = mX * mZ, xw = mX * mW;
    const f32 yz = mY * mZ, yw = mY * mW, zw = mZ * mW;

    return Matrix33f{
        {1.0f - 2.0f * (yy + zz), 2.0f * (xy - zw),        2.0f * (xz + yw)       },
        {2.0f * (xy + zw),        1.0f - 2.0f * (xx + zz), 2.0f * (yz - xw)       },
        {2.0f * (xz - yw),        2.0f * (yz + xw),        1.0f - 2.0f * (xx + yy)}
    };
}

Quaternion Quaternion::fromEulerXYZ(const Vector3f& eulerDeg) {
    constexpr f32 sDegToRad = static_cast<f32>(std::numbers::pi / 180.0);
    const f32 xRad = eulerDeg.getX() * sDegToRad;
    const f32 yRad = eulerDeg.getY() * sDegToRad;
    const f32 zRad = eulerDeg.getZ() * sDegToRad;

    const f32 cx = std::cos(xRad * 0.5f);
    const f32 sx = std::sin(xRad * 0.5f);
    const f32 cy = std::cos(yRad * 0.5f);
    const f32 sy = std::sin(yRad * 0.5f);
    const f32 cz = std::cos(zRad * 0.5f);
    const f32 sz = std::sin(zRad * 0.5f);

    return {
        cz * cy * cx + sz * sy * sx,
        cz * cy * sx - sz * sy * cx,
        cz * sy * cx + sz * cy * sx,
        sz * cy * cx - cz * sy * sx
    };
}

Vector3f Quaternion::toEulerXYZ() const {
    constexpr f32 sRadToDeg = static_cast<f32>(180.0 / std::numbers::pi);

    const f32 yw = mY * mW, xz = mX * mZ;
    const f32 sy = std::clamp(2.0f * (yw - xz), -1.0f, 1.0f);

    Vector3f euler;

    if (std::abs(sy) < 0.99999f) {
        euler.setY(static_cast<f32>(std::asin(sy)) * sRadToDeg);

        const f32 yz = mY * mZ, xw = mX * mW;
        const f32 xx = mX * mX, yy = mY * mY;
        euler.setX(static_cast<f32>(std::atan2(yz + xw, 0.5f - (xx + yy))) * sRadToDeg);

        const f32 xy = mX * mY, zw = mZ * mW;
        const f32 zz = mZ * mZ;
        euler.setZ(static_cast<f32>(std::atan2(xy + zw, 0.5f - (yy + zz))) * sRadToDeg);
    } else {
        euler.setY(static_cast<f32>(std::asin(sy)) * sRadToDeg);
        euler.setX(0.0f);

        const f32 xy = mX * mY, zw = mZ * mW;
        const f32 xx = mX * mX, zz = mZ * mZ;
        euler.setZ(static_cast<f32>(std::atan2(zw - xy, 0.5f - (xx + zz))) * sRadToDeg);
    }

    return euler;
}


// ========================================================================== //


} // namespace Math
