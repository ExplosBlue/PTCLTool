#pragma once

#include "typedefs.h"

#include <cmath>
#include <numbers>

#include "math/matrix.h"
#include "math/vector.h"

namespace Math::Util {


// ========================================================================== //


static constexpr u32 cHalfRoundIdx = 0x80000000;

inline s32 deg2idx(f32 deg) {
    return deg * cHalfRoundIdx / 180.0f;
}

inline s32 rad2idx(f32 rad) {
    return rad * cHalfRoundIdx / std::numbers::pi;
}

inline f32 idx2deg(s32 idx) {
    return idx * (180.0f / cHalfRoundIdx);
}

inline f32 idx2rad(s32 idx) {
    return idx * (std::numbers::pi / cHalfRoundIdx);
}

inline f32 to360(f32 angle) {
    f32 result = std::fmod(angle + 360.0f, 360.0f);
    if (result < 0.0f) {
        result += 360.0f;
    }
    return result;
}

inline f32 to180(f32 angle) {
    angle = std::fmod(angle + 180.0f, 360.0f);
    if (angle < 0.0f) {
        angle += 360.0f;
    }
    return angle - 180.0f;
}

inline f32 rad2deg(f32 rad) {
    return rad * static_cast<f32>(180.0f / std::numbers::pi);
}

inline f32 deg2rad(f32 deg) {
    return deg * static_cast<f32>(std::numbers::pi / 180.0f);
}

inline Vector3f getTranslation(const Matrix34f& mtx) {
    return Vector3f{
        mtx(0, 3),
        mtx(1, 3),
        mtx(2, 3)
    };
}

inline Vector3f getScale(const Matrix34f& mtx) {
    const f32 m00 = mtx(0, 0);
    const f32 m01 = mtx(0, 1);
    const f32 m02 = mtx(0, 2);

    const f32 m10 = mtx(1, 0);
    const f32 m11 = mtx(1, 1);
    const f32 m12 = mtx(1, 2);

    const f32 m20 = mtx(2, 0);
    const f32 m21 = mtx(2, 1);
    const f32 m22 = mtx(2, 2);

    f32 scaleX = std::sqrt(m00 * m00 + m10 * m10 + m20 * m20);
    f32 scaleY = std::sqrt(m01 * m01 + m11 * m11 + m21 * m21);
    f32 scaleZ = std::sqrt(m02 * m02 + m12 * m12 + m22 * m22);

    return Vector3f{scaleX, scaleY, scaleZ};
}

inline Vector3f getRotationEuler(const Matrix34f& mtx) {
    const f32 m00 = mtx(0, 0);
    const f32 m01 = mtx(0, 1);

    const f32 m10 = mtx(1, 0);
    const f32 m11 = mtx(1, 1);

    const f32 m20 = mtx(2, 0);
    const f32 m21 = mtx(2, 1);
    const f32 m22 = mtx(2, 2);

    Vector3f euler;

    if (std::abs(m20) < 0.99999f) {
        euler.setY(rad2deg(std::asin(-m20)));
        euler.setX(rad2deg(std::atan2(m21, m22)));
        euler.setZ(rad2deg(std::atan2(m10, m00)));
    } else {
        euler.setY(rad2deg(std::asin(-m20)));
        euler.setX(0.0f);
        euler.setZ(rad2deg(std::atan2(-m01, m11)));
    }
    return euler;
}

inline Matrix33f eulerToRotationMatrix(const Vector3f& euler) {
    const f32 xRad = deg2rad(euler.getX());
    const f32 yRad = deg2rad(euler.getY());
    const f32 zRad = deg2rad(euler.getZ());

    const f32 cx = std::cos(xRad);
    const f32 sx = std::sin(xRad);

    const f32 cy = std::cos(yRad);
    const f32 sy = std::sin(yRad);

    const f32 cz = std::cos(zRad);
    const f32 sz = std::sin(zRad);

    const f32 m00 = cz * cy;
    const f32 m01 = cz * sy * sx - sz * cx;
    const f32 m02 = cz * sy * cx + sz * sx;

    const f32 m10 = sz * cy;
    const f32 m11 = sz * sy * sx + cz * cx;
    const f32 m12 = sz * sy * cx - cz * sx;

    const f32 m20 = -sy;
    const f32 m21 = cy * sx;
    const f32 m22 = cy * cx;

    return Matrix33f{
        { m00, m01, m02 },
        { m10, m11, m12 },
        { m20, m21, m22 }
    };
}


// ========================================================================== //


} // Math::Util
