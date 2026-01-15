#pragma once

#include "typedefs.h"

#include <cassert>
#include <cmath>

namespace Math {


// ========================================================================== //


template<typename T>
class Vector3 {
    static_assert(std::is_arithmetic_v<T>, "Vector3 requires a numeric type");

public:
    Vector3() :
        mX{}, mY{}, mZ{} {}

    Vector3(T x, T y, T z) :
        mX{x}, mY{y}, mZ{z} {}

    Vector3(const Vector3&) = default;
    Vector3(Vector3&&) = default;
    Vector3& operator=(const Vector3&) = default;
    Vector3& operator=(Vector3&&) = default;

    T& operator[](std::size_t i) {
        assert(i < 3);
        return i == 0 ? mX : (i == 1 ? mY : mZ);
    }

    T operator[](std::size_t i) const {
        assert(i < 3);
        return i == 0 ? mX : (i == 1 ? mY : mZ);
    }

    T getX() const { return mX; }
    T getY() const { return mY; }
    T getZ() const { return mZ; }

    void setX(T val) { mX = val; }
    void setY(T val) { mY = val; }
    void setZ(T val) { mZ = val; }

    T length() const { return std::sqrt(mX * mX + mY * mY + mZ * mZ); }

private:
    T mX;
    T mY;
    T mZ;
};

using Vector3f = Vector3<f32>;
using Vector3i = Vector3<s32>;


// ========================================================================== //


} // Math
