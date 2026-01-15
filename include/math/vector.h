#pragma once

#include "typedefs.h"

#include <cassert>
#include <cmath>

namespace Math {

// ========================================================================== //


template<typename T>
class Vector2 {
    static_assert(std::is_arithmetic_v<T>, "Vector2 requires a numeric type");
    static constexpr s32 sAxisCount = 2;

public:
    Vector2() :
        mX{}, mY{} {}

    Vector2(T x, T y) :
        mX{x}, mY{y} {}

    Vector2(const Vector2&) = default;
    Vector2(Vector2&&) = default;
    Vector2& operator=(const Vector2&) = default;
    Vector2& operator=(Vector2&&) = default;

    T& operator[](std::size_t i) {
        assert(i < sAxisCount);
        return (&mX)[i];
    }

    T operator[](std::size_t i) const {
        assert(i < sAxisCount);
        return (&mX)[i];
    }

    T getX() const { return mX; }
    T getY() const { return mY; }

    void setX(T val) { mX = val; }
    void setY(T val) { mY = val; }

    T length() const { return std::sqrt(mX * mX + mY * mY); }

private:
    T mX;
    T mY;
};

using Vector2f = Vector2<f32>;
using Vector2i = Vector2<s32>;


// ========================================================================== //


template<typename T>
class Vector3 {
    static_assert(std::is_arithmetic_v<T>, "Vector3 requires a numeric type");
    static constexpr s32 sAxisCount = 3;

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
        assert(i < sAxisCount);
        return (&mX)[i];
    }

    T operator[](std::size_t i) const {
        assert(i < sAxisCount);
        return (&mX)[i];
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


template<typename T>
class Vector4 {
    static_assert(std::is_arithmetic_v<T>, "Vector4 requires a numeric type");
    static constexpr s32 sAxisCount = 4;

public:
    Vector4() :
        mX{}, mY{}, mZ{}, mW{} {}

    Vector4(T x, T y, T z, T w) :
        mX{x}, mY{y}, mZ{z}, mW{w} {}

    Vector4(const Vector4&) = default;
    Vector4(Vector4&&) = default;
    Vector4& operator=(const Vector4&) = default;
    Vector4& operator=(Vector4&&) = default;

    T& operator[](std::size_t i) {
        assert(i < sAxisCount);
        return (&mX)[i];
    }

    T operator[](std::size_t i) const {
        assert(i < sAxisCount);
        return (&mX)[i];
    }

    T getX() const { return mX; }
    T getY() const { return mY; }
    T getZ() const { return mZ; }
    T getW() const { return mW; }

    void setX(T val) { mX = val; }
    void setY(T val) { mY = val; }
    void setZ(T val) { mZ = val; }
    void setW(T val) { mW = val; }

    T length() const { return std::sqrt(mX * mX + mY * mY + mZ * mZ + mW * mW); }

private:
    T mX;
    T mY;
    T mZ;
    T mW;
};

using Vector4f = Vector4<f32>;
using Vector4i = Vector4<s32>;


// ========================================================================== //


} // Math
