#pragma once

#include "math/matrix.h"
#include "math/vector.h"
#include "ptcl/engine/particleInstance.h"
#include "ptcl/ptclEmitter.h"
#include "typedefs.h"

#include <vector>


namespace Ptcl::Engine {


// ========================================================================== //


struct SetTransform {
    Math::Vector3f emitterScale{1.0f, 1.0f, 1.0f};

    f32 figureVelocity{1.0f};
    f32 directionalVelocity{1.0f};
    f32 randomVelocity{1.0f};

    Math::Vector3f velocityAdd{0.0f, 0.0f, 0.0f};
    Math::Vector3f directional{0.0f, 0.0f, 0.0f};
    Math::Vector2f emitScale{1.0f, 1.0f};

    bool hasTransform{false};
    bool isDirectional{false};
    bool hasSrt{false};

    Math::Matrix34f srt{
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f }
    };
};

class EmitterInstance {
public:
    EmitterInstance(const Ptcl::Emitter& emitterRes, u32 randomSeed,
        u32 vec3Idx, u32 normalizedVec3Idx, s32 emissionInterval,
        SetTransform transform = SetTransform{}
    );

    u32 random() const;
    u32 vec3Idx() const;
    bool alive() const;

private:
    const Ptcl::Emitter& mResource;
    u32 mRandom;
    u16 mVec3RandomIdx;
    u16 mNormalizedVec3RandomIdx;
    s32 mEmissionInterval;
    u32 mLoopIntervalRaw;
    u32 mTickCount;
    f32 mFadeAlpha;
    bool mIsAlive{true};
    SetTransform mTransform;
    std::vector<ParticleInstance> mParticles;
};


// ========================================================================== //


} // namespace Ptcl::Engine
