#include "gfx/color.h"


namespace Gfx {


// ========================================================================== //


Color::Color() :
    mR{1.0f}, mG{1.0f}, mB{1.0f}, mA{1.0f} {}

Color::Color(f32 r, f32 g, f32 b, f32 a) :
    mR{r}, mG{g}, mB{b}, mA{a} {}

Color::Color(f32 r, f32 g, f32 b) :
    mR{r}, mG{g}, mB{b}, mA{1.0f} {}


// ========================================================================== //


} // namespace Gfx