#include "math/matrix.h"
#include "math/vector.h"


namespace Math::VectorCalc {


Vector3f multiply(const Matrix34f& m, const Vector3f& v) {
    const f32 vx = v.x;
    const f32 vy = v.y;
    const f32 vz = v.z;

    return {
        std::fmaf(m(0, 2), vz, std::fmaf(m(0, 1), vy, std::fmaf(m(0, 0), vx, m(0, 3)))),
        std::fmaf(m(1, 2), vz, std::fmaf(m(1, 1), vy, std::fmaf(m(1, 0), vx, m(1, 3)))),
        std::fmaf(m(2, 2), vz, std::fmaf(m(2, 1), vy, std::fmaf(m(2, 0), vx, m(2, 3))))
    };
}

Vector3f rotate(const Matrix34f& m, const Vector3f& v) {
    const f32 vx = v.x;
    const f32 vy = v.y;
    const f32 vz = v.z;

    return {
        std::fmaf(m(0, 2), vz, std::fmaf(m(0, 1), vy, m(0, 0) * vx)),
        std::fmaf(m(1, 2), vz, std::fmaf(m(1, 1), vy, m(1, 0) * vx)),
        std::fmaf(m(2, 2), vz, std::fmaf(m(2, 1), vy, m(2, 0) * vx))
    };
}


// ========================================================================== //


} // namespace Math::VectorCalc
