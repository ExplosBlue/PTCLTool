#include "ptcl/engine/emitterInstance.h"

namespace Ptcl::Engine {


// ========================================================================== //


constexpr u32 cInitLoopIntervalRaw = 0x7FFFFFFFu;

EmitterInstance::EmitterInstance(const Ptcl::Emitter& emitterRes, EmitterSetInstance& set, u32 randomSeed,
    u32 vec3Idx, u32 normalizedVec3Idx) :
    mResource{emitterRes},
    mSet{set},
    mRandom{randomSeed, static_cast<u16>(vec3Idx), static_cast<u16>(normalizedVec3Idx)},
    mEmissionInterval{0},
    mLoopInterval{cInitLoopIntervalRaw},
    mTickCount{0},
    mFadeAlpha{1.0f} {
    constexpr u32 cLcgMultiplier = 1103515245;
    constexpr u32 cLcgIncrement = 12345;
    const u32 advancedSeed = randomSeed * cLcgMultiplier + cLcgIncrement;
    mRandom.getU32();
    mEmissionInterval = emitterRes.lifeStep()
        - static_cast<s32>((static_cast<u64>(emitterRes.lifeStepRandom()) * advancedSeed) >> 32);
}

u32 EmitterInstance::random() const {
    return mRandom.rnd();
}

u32 EmitterInstance::vec3Idx() const {
    return mRandom.vec3RndIdx();
}

bool EmitterInstance::alive() const {
    return mIsAlive;
}


// ========================================================================== //


} // namespace Ptcl::Engine
