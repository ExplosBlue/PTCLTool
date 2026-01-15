#pragma once

#include "typedefs.h"

#include <cassert>
#include <cstring>
#include <array>

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
        std::array<std::array<f32, 4>, 3> rows;
        std::array<f32, 12> cells;
    };
};

using Matrix34f = Matrix<f32, 3, 4>;
using Matrix33f = Matrix<f32, 3, 3>;


// ========================================================================== //


} // Math
