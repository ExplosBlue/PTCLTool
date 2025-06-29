#pragma once

#include "typedefs.h"

#include <cmath>
#include <numbers>


namespace MathUtil {


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
    if (angle >= 180.0f) {
        return angle - 360.0f;
    }
    return angle;
}


// ========================================================================== //


} // MathUtil
