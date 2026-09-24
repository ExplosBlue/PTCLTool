#pragma once

#include "gfx/color.h"

#include <array>


namespace Ptcl::Engine {

namespace Combiner {

enum class StageType {
    Color,
    Alpha
};

enum class Input {
    Constant,
    Primary,
    Texture,
    Previous
};

enum class Operand {
    Src,
    SrcG,
    OneMinusSrc
};

enum class Func {
    Replace,
    Modulate,
    Subtract,
    MultAdd
};

enum class Scale {
    One,
    Two
};

struct Stage {
    StageType type;
    std::array<Input, 3> inputs;
    std::array<Operand, 3> operands;
    Func func;
    Scale scale;
};

struct Config {
    std::array<Stage, 2> colorStage;
    std::array<Stage, 2> alphaStage;
};

static constexpr std::array<Config, 28> configurations = []() constexpr {
    using O = Operand;
    using I = Input;
    using F = Func;
    using S = Scale;
    using T = StageType;

    constexpr auto makeStage = [](T type, I in0, I in1, I in2, F func, O op0 = O::Src, O op1 = O::Src, O op2 = O::Src, S scale = S::One) constexpr {
        return Stage{ type, { in0, in1, in2 }, { op0, op1, op2 }, func, scale };
    };

    constexpr auto makeConfig = [](Stage c0, Stage a0, Stage c1, Stage a1) constexpr {
        return Config{ { c0, c1 }, { a0, a1 } };
    };

    return std::array<Config, 28> {{
        // Config 0
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 1
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 2
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd, O::Src, O::OneMinusSrc, O::Src),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 3
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate, O::Src, O::OneMinusSrc, O::Src),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 4
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 5
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 6
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Primary,  F::MultAdd),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 7
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Subtract, O::Src, O::OneMinusSrc, O::Src, S::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 8
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Subtract, O::Src, O::OneMinusSrc, O::Src, S::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 9
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::Src, O::OneMinusSrc, O::Src, S::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd,  O::Src, O::OneMinusSrc, O::Src),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 10
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate, O::Src, O::OneMinusSrc, O::Src),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::Src, O::OneMinusSrc, O::Src, S::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 11
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::Src, O::OneMinusSrc, O::Src, S::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 12
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::Src, O::OneMinusSrc, O::Src, S::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 13
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Primary,  F::MultAdd),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::Src, O::OneMinusSrc, O::Src, S::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 14
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate, O::Src, O::SrcG, O::Src),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 15
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate, O::Src, O::SrcG, O::Src),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 16
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate, O::Src, O::SrcG,        O::Src),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd,  O::Src, O::OneMinusSrc, O::Src),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 17
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate, O::Src, O::OneMinusSrc, O::Src),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate, O::Src, O::SrcG,        O::Src),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 18
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate, O::Src, O::SrcG, O::Src),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 19
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate, O::Src, O::SrcG, O::Src),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 20
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Primary,  F::MultAdd),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate, O::Src, O::SrcG, O::Src),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 21
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::SrcG, O::OneMinusSrc, O::Src, Scale::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 22
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::SrcG, O::OneMinusSrc, O::Src, Scale::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 23
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::SrcG, O::OneMinusSrc, O::Src, Scale::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd,  O::Src,  O::OneMinusSrc, O::Src),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 24
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate, O::Src,  O::OneMinusSrc, O::Src),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::SrcG, O::OneMinusSrc, O::Src, Scale::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 25
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::SrcG, O::OneMinusSrc, O::Src, Scale::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 26
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::SrcG, O::OneMinusSrc, O::Src, Scale::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
        // Config 27
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Primary,  F::MultAdd),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::SrcG, O::OneMinusSrc, O::Src, Scale::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
            ),
    }};
}();

} // namespace Combiner

} // namespace Ptcl::Engine
