#pragma once

#include "gfx/color.h"
#include "math/matrix.h"
#include "math/vector.h"
#include "typedefs.h"


namespace Ptcl::Engine {


// ========================================================================== //


struct PtclInstance {
    Math::Vector3f position;
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
    PtclInstance* prev{nullptr};
    PtclInstance* next{nullptr};

    PtclInstance() = default;
};


// ========================================================================== //


} // namespace Ptcl::Engine
