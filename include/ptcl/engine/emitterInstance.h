#pragma once

#include "ptcl/engine/emitterSetInstance.h"
#include "ptcl/engine/ptclInstance.h"
#include "ptcl/engine/ptclRandom.h"
#include "ptcl/ptclEmitter.h"
#include "typedefs.h"


namespace Ptcl::Engine {


// ========================================================================== //


class EmitterCalc;

struct EmitterController {
    u32 emitRatePerFrame{256};
    u32 lifeScale{256};
};

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
    friend class EmitterSetInstance;
    friend class PtclSystem;

    const Ptcl::Emitter& mResource;
    EmitterSetInstance& mSet;
    u32 mEmitterSetCreateID{0};
    PtclRandom mRandom;
    Math::Matrix34f mEmitterRT{{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}};
    Math::Matrix34f mEmitterSRT{{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}};
    s32 mEmissionInterval;
    s32 mLoopInterval;
    s32 mTickCount;
    f32 mFadeAlpha;
    bool mIsAlive{true};
    PtclInstance* mPtclHead{nullptr};
    EmitterController mController;
    EmitterInstance* mPrev{nullptr};
    EmitterInstance* mNext{nullptr};
    PtclInstance* mChildHead{nullptr};
};


// ========================================================================== //


} // namespace Ptcl::Engine
