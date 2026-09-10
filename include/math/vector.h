#pragma once

#include "typedefs.h"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <type_traits>


namespace Math {

// ========================================================================== //


template<typename T>
class Vector2 {
    static_assert(std::is_arithmetic_v<T>, "Vector2 requires a numeric type");
    static constexpr s32 sAxisCount = 2;

public:
    Vector2() :
        x{}, y{} {}

    Vector2(T x, T y) :
        x{x}, y{y} {}

    Vector2(const Vector2&) = default;
    Vector2(Vector2&&) = default;

    Vector2& operator=(const Vector2&) = default;
    Vector2& operator=(Vector2&&) = default;

    bool operator==(const Vector2&) const = default;

    T& operator[](std::size_t i) {
        assert(i < sAxisCount);
        return (&x)[i];
    }

    T operator[](std::size_t i) const {
        assert(i < sAxisCount);
        return (&x)[i];
    }

    T getX() const { return x; }
    T getY() const { return y; }

    void setX(T val) { x = val; }
    void setY(T val) { y = val; }

    void setZero() {
        x = T{};
        y = T{};
    }

    T length() const { return std::sqrt(std::fmaf(y, y, x * x)); }

    T normalize() {
        const T len = length();

        if (len > 0.0f) {
            const T invLen = 1.0f / len;
            x *= invLen;
            y *= invLen;
        }

        return len;
    }

    Vector2 normalized() const {
        Vector2 out = *this;
        out.normalize();
        return out;
    }

    T x;
    T y;
};

using Vector2f = Vector2<f32>;
using Vector2i = Vector2<s32>;


// ========================================================================== //


template<typename T>
class Vector3 {
    static_assert(std::is_arithmetic_v<T>, "Vector3 requires a numeric type");
    static constexpr s32 sAxisCount = 3;

public:
    constexpr Vector3() :
        x{}, y{}, z{} {}

    constexpr Vector3(T x, T y, T z) :
        x{x}, y{y}, z{z} {}

    constexpr Vector3(const Vector3&) = default;
    constexpr Vector3(Vector3&&) = default;

    constexpr Vector3& operator=(const Vector3&) = default;
    constexpr Vector3& operator=(Vector3&&) = default;

    bool operator==(const Vector3&) const = default;

    T& operator[](std::size_t i) {
        assert(i < sAxisCount);
        return (&x)[i];
    }

    T operator[](std::size_t i) const {
        assert(i < sAxisCount);
        return (&x)[i];
    }

    T getX() const { return x; }
    T getY() const { return y; }
    T getZ() const { return z; }

    void setX(T val) { x = val; }
    void setY(T val) { y = val; }
    void setZ(T val) { z = val; }

    void setZero() {
        x = T{};
        y = T{};
        z = T{};
    }

    T length() const { return std::sqrt(std::fmaf(z, z, std::fmaf(y, y, x * x))); }

    T normalize() {
        const T len = length();

        if (len > 0.0f) {
            const T invLen = 1.0f / len;
            x *= invLen;
            y *= invLen;
            z *= invLen;
        }

        return len;
    }

    Vector3 normalized() const {
        Vector3 out = *this;
        out.normalize();
        return out;
    }

    T x;
    T y;
    T z;
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
        x{}, y{}, z{}, w{} {}

    Vector4(T x, T y, T z, T w) :
        x{x}, y{y}, z{z}, w{w} {}

    Vector4(const Vector4&) = default;
    Vector4(Vector4&&) = default;

    Vector4& operator=(const Vector4&) = default;
    Vector4& operator=(Vector4&&) = default;

    bool operator==(const Vector4&) const = default;

    T& operator[](std::size_t i) {
        assert(i < sAxisCount);
        return (&x)[i];
    }

    T operator[](std::size_t i) const {
        assert(i < sAxisCount);
        return (&x)[i];
    }

    T getX() const { return x; }
    T getY() const { return y; }
    T getZ() const { return z; }
    T getW() const { return w; }

    void setX(T val) { x = val; }
    void setY(T val) { y = val; }
    void setZ(T val) { z = val; }
    void setW(T val) { w = val; }

    void setZero() {
        x = T{};
        y = T{};
        z = T{};
        w = T{};
    }

    T length() const {
        return std::sqrt(std::fmaf(w, w, std::fmaf(z, z, std::fmaf(y, y, x * x))));
    }

    T normalize() {
        const T len = length();

        if (len > 0.0f) {
            const T invLen = 1.0f / len;
            x *= invLen;
            y *= invLen;
            z *= invLen;
            w *= invLen;
        }

        return len;
    }

    Vector4 normalized() const {
        Vector4 out = *this;
        out.normalize();
        return out;
    }

    T x;
    T y;
    T z;
    T w;
};

using Vector4f = Vector4<f32>;
using Vector4i = Vector4<s32>;

template<typename T, std::size_t RowCount, std::size_t ColCount>
class Matrix;

namespace VectorCalc {

Vector3f multiply(const Matrix<f32, 3, 4>& m, const Vector3f& v);
Vector3f rotate(const Matrix<f32, 3, 4>& m, const Vector3f& v);

Vector3f add(const Vector3f& a, const Vector3f& b);
Vector3f sub(const Vector3f& a, const Vector3f& b);
Vector3f multScalar(const Vector3f& a, f32 scalar);
Vector3f cross(const Vector3f& a, const Vector3f& b);
void normalize(Vector3f& v);

} // namespace VectorCalc


// ========================================================================== //


} // namespace Math
