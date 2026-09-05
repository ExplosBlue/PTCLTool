#pragma once

#include "ptcl/engine/emitterSetInstance.h"
#include "ptcl/engine/ptclInstance.h"
#include "ptcl/engine/ptclRandom.h"
#include "ptcl/ptclEmitter.h"
#include "typedefs.h"


namespace Ptcl::Engine {


// ========================================================================== //


class EmitterCalc;
class EmitterInstance {
public:
    EmitterInstance(const Ptcl::Emitter& emitterRes, EmitterSetInstance& set, u32 randomSeed,
        u32 vec3Idx, u32 normalizedVec3Idx, s32 emissionInterval
    );

    u32 random() const;
    u32 vec3Idx() const;
    bool alive() const;

private:
    friend class EmitterCalc;

    const Ptcl::Emitter& mResource;
    EmitterSetInstance& mSet;
    PtclRandom mRandom;
    s32 mEmissionInterval;
    u32 mLoopIntervalRaw;
    u32 mTickCount;
    f32 mFadeAlpha;
    bool mIsAlive{true};
    PtclInstance* mPtclHead{nullptr};
};


// ========================================================================== //


} // namespace Ptcl::Engine
