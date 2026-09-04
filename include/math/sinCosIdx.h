#pragma once

#include "typedefs.h"


namespace Math {


// ========================================================================== //


struct SinCosSample {
    f32 sin;
    f32 sinDelta;
    f32 cos;
    f32 cosDelta;
};

void sinCosIdx(f32* sinOut, f32* cosOut, u32 idx);


// ========================================================================== //


} // namespace Math
