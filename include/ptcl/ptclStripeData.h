#pragma once

#include "math/vector.h"
#include "ptcl/ptclEnum.h"
#include "typedefs.h"


namespace Ptcl {


// ========================================================================== //


struct StripeData
{
    StripeType type{};
    s32 numHistory{};
    f32 startAlpha{};
    f32 endAlpha{};
    Math::Vector2f uvScrollSpeed{};
    s32 historyStep{};
    f32 dirInterpolate{};
};


// ========================================================================== //


} // namespace Ptcl
