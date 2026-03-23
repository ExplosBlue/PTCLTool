#pragma once

#include "typedefs.h"

#include <QColor>

namespace Gfx {


// ========================================================================== //


class Color {
public:
    enum class Channel {
        R = 0,
        G = 1,
        B = 2,
        A = 3
    };

public:
    Color();
    Color(f32 r, f32 g, f32 b, f32 a);
    Color(f32 r, f32 g, f32 b);

    f32 r() const { return mR; }
    f32 g() const { return mG; }
    f32 b() const { return mB; }
    f32 a() const { return mA; }

    QColor toQColor() const;

    f32 operator[](Channel channel) const {
        switch (channel) {
            case Channel::R: return mR;
            case Channel::G: return mG;
            case Channel::B: return mB;
            case Channel::A: return mA;
            default: throw std::out_of_range("Color channel out of range");
        }
    }

    f32& operator[](Channel channel) {
        switch (channel) {
        case Channel::R: return mR;
        case Channel::G: return mG;
        case Channel::B: return mB;
        case Channel::A: return mA;
        default: throw std::out_of_range("Color channel out of range");
        }
    }

    bool operator==(const Color&) const = default;

private:
    f32 mR;
    f32 mG;
    f32 mB;
    f32 mA;
};


// ========================================================================== //


} // namespace Gfx