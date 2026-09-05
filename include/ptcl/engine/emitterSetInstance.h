#pragma once

#include "math/matrix.h"
#include "math/vector.h"
#include "typedefs.h"


namespace Ptcl::Engine {


// ========================================================================== //


struct EmitterSetInstance {
    Math::Vector3f emitterScale{1.0f, 1.0f, 1.0f};

    f32 figureVelocity{1.0f};
    f32 directionalVelocity{1.0f};
    f32 randomVelocity{1.0f};

    Math::Vector3f velocityAdd{0.0f, 0.0f, 0.0f};
    Math::Vector3f directional{0.0f, 0.0f, 0.0f};
    Math::Vector2f emissionParticleScale{1.0f, 1.0f};

    bool hasTransform{false};
    bool isSetDirectional{false};
    bool hasSRT{false};

    Math::Matrix34f SRT{
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f }
    };

    Math::Matrix34f RT{
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f }
    };
};


// ========================================================================== //


} // namespace Ptcl::Engine
