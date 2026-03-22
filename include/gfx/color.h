#pragma once

#include "typedefs.h"

#include <QColor>

namespace Gfx {


// ========================================================================== //


class Color {
public:
    Color();
    Color(f32 r, f32 g, f32 b, f32 a);
    Color(f32 r, f32 g, f32 b);

    f32 r() const { return mR; }
    f32 g() const { return mG; }
    f32 b() const { return mB; }
    f32 a() const { return mA; }

    bool operator==(const Color&) const = default;

private:
    f32 mR;
    f32 mG;
    f32 mB;
    f32 mA;
};


// ========================================================================== //


} // namespace Gfx