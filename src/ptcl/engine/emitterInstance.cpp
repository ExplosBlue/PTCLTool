#include "ptcl/engine/emitterInstance.h"

#include <utility>

namespace Ptcl::Engine {


// ========================================================================== //


constexpr u32 cInitLoopIntervalRaw = 0x7FFFFFFFu;

EmitterInstance::EmitterInstance(const Ptcl::Emitter& emitterRes, u32 randomSeed,
    u32 vec3Idx, u32 normalizedVec3Idx, s32 emissionInterval, SetTransform transform) :
    mResource{emitterRes},
    mRandom{randomSeed},
    mVec3RandomIdx{static_cast<u16>(vec3Idx)},
    mNormalizedVec3RandomIdx{static_cast<u16>(normalizedVec3Idx)},
    mEmissionInterval{emissionInterval},
    mLoopIntervalRaw{cInitLoopIntervalRaw},
    mTickCount{0},
    mFadeAlpha{1.0f},
    mTransform{std::move(transform)} {}

u32 EmitterInstance::random() const {
    return mRandom;
}

u32 EmitterInstance::vec3Idx() const {
    return mVec3RandomIdx;
}

bool EmitterInstance::alive() const {
    return mIsAlive;
}


// ========================================================================== //


} // namespace Ptcl::Engine
