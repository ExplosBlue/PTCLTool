#include "gfx/color.h"


namespace Gfx {


// ========================================================================== //


Color::Color() :
    mR{1.0f}, mG{1.0f}, mB{1.0f}, mA{1.0f} {}

Color::Color(f32 r, f32 g, f32 b, f32 a) :
    mR{r}, mG{g}, mB{b}, mA{a} {}

Color::Color(f32 r, f32 g, f32 b) :
    mR{r}, mG{g}, mB{b}, mA{1.0f} {}

QColor Color::toQColor() const {
    return {
        static_cast<s32>(std::round(mR * 255.0f)),
        static_cast<s32>(std::round(mG * 255.0f)),
        static_cast<s32>(std::round(mB * 255.0f)),
        static_cast<s32>(std::round(mA * 255.0f)),
    };
}


// ========================================================================== //


} // namespace Gfx