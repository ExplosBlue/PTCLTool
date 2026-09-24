#pragma once

#include <array>

#include "gfx/color.h"
#include "math/matrix.h"
#include "math/vector.h"
#include "typedefs.h"


namespace Ptcl {
class Emitter;
} // namespace Ptcl


namespace Ptcl::Engine {


struct PtclInstance;
struct PtclStripe;


// ========================================================================== //


struct PtclStripeHistory {
    Math::Vector3f pos;
    f32 scale{1.0f};
    Math::Matrix34f emitterSRT;
    Math::Vector3f outer;
};


struct PtclStripe {
    PtclInstance* ptcl{nullptr};
    s32 histQStart{0};
    s32 histQEnd{0};
    std::array<PtclStripeHistory, 128> hist{};
    s32 numHistory{0};
    const Ptcl::Emitter* res{nullptr};
    Gfx::Color color{0.0f, 0.0f, 0.0f, 1.0f};
    Math::Vector2f uvOffset;
    Math::Vector2f uvSubOffset;
    Math::Vector2f uvScroll;
    Math::Matrix34f emitterSRT;
    Math::Vector3f interpolateNextDir;
};


// ========================================================================== //


struct PtclInstance {
    const Ptcl::Emitter* res{nullptr};
    PtclStripe* stripe{nullptr};

    Math::Vector3f position;
    Math::Vector3f positionDiff;
    Math::Vector3f velocity;
    Math::Vector3i rotation;
    Math::Vector3i rotationVelocity;
    Math::Matrix34f emitterRT;
    Math::Matrix34f emitterSRT;
    f32 alpha;
    f32 alphaAddSection1;
    f32 alphaAddSection2;
    s32 alphaSection1;
    s32 alphaSection2;
    Math::Vector2f scale;
    Math::Vector2f scaleAddSection1;
    Math::Vector2f scaleAddSection2;
    s32 scaleSection1;
    s32 scaleSection2;
    Math::Vector2f uvOffset;
    Math::Vector2f uvSubOffset;
    Gfx::Color color;
    s32 tickCount;
    s32 life;
    u32 random;
    s32 childEmitCount{0};
    f32 fluctuationAlpha{0.0f};
    f32 fluctuationScale{0.0f};
    Math::Vector3f posDiff;

    PtclInstance() = default;
};


// ========================================================================== //


} // namespace Ptcl::Engine
