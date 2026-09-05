#pragma once

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
using Matrix33f = Matrix<f32, 3, 3>;

inline void multiply(Matrix34f& out, const Matrix34f& a, const Matrix34f& b) {
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

    out(0, 0) = a11 * b11 + a12 * b21 + a13 * b31;
    out(0, 1) = a11 * b12 + a12 * b22 + a13 * b32;
    out(0, 2) = a11 * b13 + a12 * b23 + a13 * b33;
    out(0, 3) = a11 * b14 + a12 * b24 + a13 * b34 + a14;

    out(1, 0) = a21 * b11 + a22 * b21 + a23 * b31;
    out(1, 1) = a21 * b12 + a22 * b22 + a23 * b32;
    out(1, 2) = a21 * b13 + a22 * b23 + a23 * b33;
    out(1, 3) = a21 * b14 + a22 * b24 + a23 * b34 + a24;

    out(2, 0) = a31 * b11 + a32 * b21 + a33 * b31;
    out(2, 1) = a31 * b12 + a32 * b22 + a33 * b32;
    out(2, 2) = a31 * b13 + a32 * b23 + a33 * b33;
    out(2, 3) = a31 * b14 + a32 * b24 + a33 * b34 + a34;
}


// ========================================================================== //


} // Math
