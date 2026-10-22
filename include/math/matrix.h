#pragma once

#include "math/vector.h"
#include "typedefs.h"

#include <cassert>
#include <cstring>
#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <type_traits>

namespace Math {

// ========================================================================== //


template<typename T, std::size_t RowCount, std::size_t ColCount>
class Matrix {
    static_assert(std::is_arithmetic_v<T>, "Matrix requires a numeric type");

public:
    Matrix() {
        cells.fill(T(0));
    }

    Matrix(const Matrix& other) {
        cells = other.cells;
    }

    explicit Matrix(const T* src) {
        std::memcpy(cells.data(), src, sizeof(T) * RowCount * ColCount);
    }

    Matrix(const std::initializer_list<std::initializer_list<T>>& init) {
        assert(init.size() == RowCount);
        std::size_t r = 0;
        for (const auto& rowList : init) {
            assert(rowList.size() == ColCount);
            std::copy(rowList.begin(), rowList.end(), rows[r].begin());
            ++r;
        }
    }    

    T& operator()(std::size_t row, std::size_t col) {
        assert(row < RowCount && col < ColCount);
        return rows[row][col];
    }

    T operator()(std::size_t row, std::size_t col) const {
        assert(row < RowCount && col < ColCount);
        return rows[row][col];
    }

    Matrix& operator=(const Matrix& other) {
        if (this != &other) {
            cells = other.cells;
        }
        return *this;
    }

    T* data() { return cells.data(); }
    const T* data() const { return cells.data(); }

private:
    union {
        std::array<std::array<T, ColCount>, RowCount> rows;
        std::array<T, ColCount * RowCount> cells;
    };
};

using Matrix34f = Matrix<f32, 3, 4>;
using Matrix44f = Matrix<f32, 4, 4>;
using Matrix33f = Matrix<f32, 3, 3>;


namespace MatrixCalc {


inline Matrix34f multiply(const Matrix34f& a, const Matrix34f& b) {
    Matrix34f result{};
    const f32 a11 = a(0, 0);
    const f32 a12 = a(0, 1);
    const f32 a13 = a(0, 2);
    const f32 a14 = a(0, 3);

    const f32 a21 = a(1, 0);
    const f32 a22 = a(1, 1);
    const f32 a23 = a(1, 2);
    const f32 a24 = a(1, 3);

    const f32 a31 = a(2, 0);
    const f32 a32 = a(2, 1);
    const f32 a33 = a(2, 2);
    const f32 a34 = a(2, 3);

    const f32 b11 = b(0, 0);
    const f32 b12 = b(0, 1);
    const f32 b13 = b(0, 2);
    const f32 b14 = b(0, 3);

    const f32 b21 = b(1, 0);
    const f32 b22 = b(1, 1);
    const f32 b23 = b(1, 2);
    const f32 b24 = b(1, 3);

    const f32 b31 = b(2, 0);
    const f32 b32 = b(2, 1);
    const f32 b33 = b(2, 2);
    const f32 b34 = b(2, 3);

    result(0, 0) = a11 * b11 + a12 * b21 + a13 * b31;
    result(0, 1) = a11 * b12 + a12 * b22 + a13 * b32;
    result(0, 2) = a11 * b13 + a12 * b23 + a13 * b33;
    result(0, 3) = a11 * b14 + a12 * b24 + a13 * b34 + a14;

    result(1, 0) = a21 * b11 + a22 * b21 + a23 * b31;
    result(1, 1) = a21 * b12 + a22 * b22 + a23 * b32;
    result(1, 2) = a21 * b13 + a22 * b23 + a23 * b33;
    result(1, 3) = a21 * b14 + a22 * b24 + a23 * b34 + a24;

    result(2, 0) = a31 * b11 + a32 * b21 + a33 * b31;
    result(2, 1) = a31 * b12 + a32 * b22 + a33 * b32;
    result(2, 2) = a31 * b13 + a32 * b23 + a33 * b33;
    result(2, 3) = a31 * b14 + a32 * b24 + a33 * b34 + a34;
    return result;
}

inline Matrix44f multiply(const Matrix44f& a, const Matrix44f& b) {
    Matrix44f result{};
    for (std::size_t row = 0; row < 4; ++row) {
        for (std::size_t col = 0; col < 4; ++col) {
            f32 sum = 0.0f;
            for (std::size_t k = 0; k < 4; ++k) {
                sum += a(row, k) * b(k, col);
            }
            result(row, col) = sum;
        }
    }
    return result;
}

inline Vector3f multiply(const Matrix34f& m, const Vector3f& v) {
    return {
        m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z + m(0, 3),
        m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z + m(1, 3),
        m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z + m(2, 3),
    };
}

inline Vector3f rotate(const Matrix34f& m, const Vector3f& v) {
    return {
        m(0, 0) * v.x + m(1, 0) * v.y + m(2, 0) * v.z,
        m(0, 1) * v.x + m(1, 1) * v.y + m(2, 1) * v.z,
        m(0, 2) * v.x + m(1, 2) * v.y + m(2, 2) * v.z,
    };
}

inline Matrix44f invert(const Matrix44f& a) {
    const f32 m00 = a(0, 0);
    const f32 m01 = a(0, 1);
    const f32 m02 = a(0, 2);
    const f32 m03 = a(0, 3);
    const f32 m10 = a(1, 0);
    const f32 m11 = a(1, 1);
    const f32 m12 = a(1, 2);
    const f32 m13 = a(1, 3);
    const f32 m20 = a(2, 0);
    const f32 m21 = a(2, 1);
    const f32 m22 = a(2, 2);
    const f32 m23 = a(2, 3);
    const f32 m30 = a(3, 0);
    const f32 m31 = a(3, 1);
    const f32 m32 = a(3, 2);
    const f32 m33 = a(3, 3);

    const f32 b00 = m00 * m11 - m01 * m10;
    const f32 b01 = m00 * m12 - m02 * m10;
    const f32 b02 = m00 * m13 - m03 * m10;
    const f32 b03 = m01 * m12 - m02 * m11;
    const f32 b04 = m01 * m13 - m03 * m11;
    const f32 b05 = m02 * m13 - m03 * m12;
    const f32 b06 = m20 * m31 - m21 * m30;
    const f32 b07 = m20 * m32 - m22 * m30;
    const f32 b08 = m20 * m33 - m23 * m30;
    const f32 b09 = m21 * m32 - m22 * m31;
    const f32 b10 = m21 * m33 - m23 * m31;
    const f32 b11 = m22 * m33 - m23 * m32;

    const f32 det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
    if (det == 0.0f) {
        return Matrix44f{
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f},
        };
    }

    const f32 invDet = 1.0f / det;
    Matrix44f result{};

    result(0, 0) = (m11 * b11 - m12 * b10 + m13 * b09) * invDet;
    result(0, 1) = (-m01 * b11 + m02 * b10 - m03 * b09) * invDet;
    result(0, 2) = (m31 * b05 - m32 * b04 + m33 * b03) * invDet;
    result(0, 3) = (-m21 * b05 + m22 * b04 - m23 * b03) * invDet;
    result(1, 0) = (-m10 * b11 + m12 * b08 - m13 * b07) * invDet;
    result(1, 1) = (m00 * b11 - m02 * b08 + m03 * b07) * invDet;
    result(1, 2) = (-m30 * b05 + m32 * b02 - m33 * b01) * invDet;
    result(1, 3) = (m20 * b05 - m22 * b02 + m23 * b01) * invDet;
    result(2, 0) = (m10 * b10 - m11 * b08 + m13 * b06) * invDet;
    result(2, 1) = (-m00 * b10 + m01 * b08 - m03 * b06) * invDet;
    result(2, 2) = (m30 * b04 - m31 * b02 + m33 * b00) * invDet;
    result(2, 3) = (-m20 * b04 + m21 * b02 - m23 * b00) * invDet;
    result(3, 0) = (-m10 * b09 + m11 * b07 - m12 * b06) * invDet;
    result(3, 1) = (m00 * b09 - m01 * b07 + m02 * b06) * invDet;
    result(3, 2) = (-m30 * b03 + m31 * b01 - m32 * b00) * invDet;
    result(3, 3) = (m20 * b03 - m21 * b01 + m22 * b00) * invDet;
    return result;
}

inline Matrix34f invert(const Matrix34f& a) {
    Matrix34f result{};
    const f32 m00 = a(0, 0);
    const f32 m01 = a(0, 1);
    const f32 m02 = a(0, 2);
    const f32 m03 = a(0, 3);
    const f32 m10 = a(1, 0);
    const f32 m11 = a(1, 1);
    const f32 m12 = a(1, 2);
    const f32 m13 = a(1, 3);
    const f32 m20 = a(2, 0);
    const f32 m21 = a(2, 1);
    const f32 m22 = a(2, 2);
    const f32 m23 = a(2, 3);

    const f32 v12 = m20 * m11;
    const f32 v13 = -(m10 * m01 * m22) - (m01 * m12 * m20);
    const f32 v14 = -(m00 * m21 * m12) - (m02 * m10 * m21);
    const f32 det = (-(v12 * m02) - (m00 * m11 * m22)) + v13 + v14;

    if (det == 0.0f) {
        return Matrix34f{
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
        };
    }

    const f32 invDet = 1.0f / det;

    const f32 v18 = -(m11 * m02) - (m01 * m12);
    const f32 v19 = -(m20 * m02) - (m00 * m22);
    const f32 v20 = -(m10 * m02) - (m00 * m12);
    const f32 v21 = -(m20 * m11) - (m10 * m21);
    const f32 v22 = -(m20 * m01) - (m00 * m21);
    const f32 v23 = -(m10 * m01) - (m00 * m11);

    const f32 r00 = (-(m21 * m12) - (m11 * m22)) * invDet;
    const f32 r01 = -((-(m21 * m02) - (m01 * m22)) * invDet);
    const f32 r02 = v18 * invDet;
    const f32 r10 = -((-(m20 * m12) - (m10 * m22)) * invDet);
    const f32 r11 = v19 * invDet;
    const f32 r12 = -(v20 * invDet);
    const f32 r20 = v21 * invDet;
    const f32 r21 = -(v22 * invDet);
    const f32 r22 = v23 * invDet;

    result(0, 0) = r00;
    result(0, 1) = r01;
    result(0, 2) = r02;
    result(0, 3) = -(r02 * m23) - (-(r01 * m13) - -(r00 * m03));
    result(1, 0) = r10;
    result(1, 1) = r11;
    result(1, 2) = r12;
    result(1, 3) = -(r12 * m23) - (-(r11 * m13) - -(r10 * m03));
    result(2, 0) = r20;
    result(2, 1) = r21;
    result(2, 2) = r22;
    result(2, 3) = -(r22 * m23) - (-(r21 * m13) - -(r20 * m03));
    return result;
}


} // MatrixCalc


// ========================================================================== //


} // Math
