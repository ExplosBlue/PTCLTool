#pragma once

#include "math/vector.h"
#include "ptcl/ptclEnum.h"
#include "typedefs.h"


namespace Ptcl {


// ========================================================================== //


struct StripeData
{
    StripeType type{StripeType::Billboard};
    s32 numHistory{60};
    f32 startAlpha{1.0f};
    f32 endAlpha{1.0f};
    Math::Vector2f uvScrollSpeed{0.0f, 0.0f};
    s32 historyStep{1};
    f32 dirInterpolate{1.0f};
};


// ========================================================================== //


} // namespace Ptcl
