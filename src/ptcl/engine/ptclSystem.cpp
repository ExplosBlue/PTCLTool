#include "ptcl/engine/ptclSystem.h"

#include "ptcl/engine/emitterCalc.h"
#include "ptcl/engine/emitterInstance.h"
#include "ptcl/ptclEmitterSet.h"

#include <QDebug>

#include <stdexcept>
#include <utility>


namespace Ptcl::Engine {


// ========================================================================== //


PtclSystem::PtclSystem(PtclSystemConfig config) {
    if (config.regularParticleCapacity <= 0 || config.compactParticleCapacity <= 0) {
        throw std::invalid_argument("Invalid PtclSystem particle pool capacity");
    }

    mNumPtclData = config.regularParticleCapacity;
    mNumCompactPtclData = config.compactParticleCapacity;
    mPtcls.assign(mNumPtclData, PtclInstance{});
    mCompactPtcls.assign(mNumCompactPtclData, PtclInstance{});

    mNumStripeData = mNumPtclData;
    mNumStripeDataMask = mNumStripeData - 1;
    mStripePool.assign(static_cast<std::size_t>(mNumStripeData), PtclStripe{});

    EmitterCalc::mSys = this;
}

PtclSystem::~PtclSystem() {
    if (EmitterCalc::mSys == this) {
        EmitterCalc::mSys = nullptr;
    }
}

PtclInstance* PtclSystem::allocCompactPtcl() {
    s32 idx = 0;

    while (true) {
        mCompactCurPtclIx = (mCompactCurPtclIx + 1) % mNumCompactPtclData;
        if (mCompactPtcls[mCompactCurPtclIx].res == nullptr) {
            return &mCompactPtcls[mCompactCurPtclIx];
        }
        if (mNumCompactPtclData <= ++idx) {
            qDebug("PtclSystem: compact particle pool is full\n");
            return nullptr;
        }
    }
}

void PtclSystem::addEmitter(EmitterInstance& emitter) {
    mEmitters.push_back(&emitter);
}

void PtclSystem::killEmitter(EmitterInstance& emitter) {
    if (emitter.mIsAlive == false) {
        return;
    }

    mEmitters.remove(&emitter);

    for (PtclInstance* ptcl : emitter.mParticles) {
        if (ptcl->stripe != nullptr) {
            removeStripe(*ptcl->stripe);
        }
        ptcl->res = nullptr;
    }
    emitter.mParticles.clear();

    for (PtclInstance* child : emitter.mChildParticles) {
        if (child->stripe != nullptr) {
            removeStripe(*child->stripe);
        }
        child->res = nullptr;
    }
    emitter.mChildParticles.clear();

    emitter.mIsAlive = false;
}

PtclInstance* PtclSystem::allocPtcl() {
    s32 idx = 0;

    while (true) {
        mCurPtclIx = (mCurPtclIx + 1) % mNumPtclData;

        if (mPtcls[mCurPtclIx].res == nullptr) {
            break;
        }

        if (mNumPtclData <= ++idx) {
            qDebug("PtclSystem: particle pool is full\n");
            return nullptr;
        }
    }

    return &mPtcls[mCurPtclIx];
}

PtclStripe* PtclSystem::allocAndConnectStripe(EmitterInstance& emitter, PtclInstance& ptcl) {
    s32 idx = 0;

    while (true) {
        mCurStripeIx = (mCurStripeIx + 1) & mNumStripeDataMask;

        PtclStripe* stripe = &mStripePool[mCurStripeIx];
        if (stripe->ptcl == nullptr) {
            break;
        }

        if (mNumStripeData <= ++idx) {
            removeStripe(*stripe);
            break;
        }
    }

    PtclStripe* stripe = &mStripePool[mCurStripeIx];
    stripe->ptcl = &ptcl;
    stripe->histQStart = 0;
    stripe->histQEnd = 0;
    stripe->numHistory = 0;
    stripe->res = &emitter.mResource;
    stripe->color = {0.0f, 0.0f, 0.0f, 1.0f};
    stripe->uvOffset = {0.0f, 0.0f};
    stripe->uvSubOffset = {0.0f, 0.0f};
    stripe->uvScroll = {0.0f, 0.0f};
    stripe->interpolateNextDir = {0.0f, 0.0f, 0.0f};
    stripe->hist[0].outer = {0.0f, 0.0f, 0.0f};

    return stripe;
}

void PtclSystem::removeStripe(PtclStripe& stripe) {
    if (stripe.ptcl != nullptr && stripe.ptcl->stripe == &stripe) {
        stripe.ptcl->stripe = nullptr;
    }
    stripe.ptcl = nullptr;
    stripe.res = nullptr;
    stripe.numHistory = 0;
}

void PtclSystem::reset() {
    mFrame.clear();
    mEmitters.clear();
    mOwnedEmitters.clear();
    mSetInstance = {};
    mCurPtclIx = 0;
    mCompactCurPtclIx = 0;
    mCurStripeIx = 0;
    mFrameIndex = 0;
    for (auto& ptcl : mPtcls) {
        ptcl.res = nullptr;
        ptcl.stripe = nullptr;
    }
    for (auto& ptcl : mCompactPtcls) {
        ptcl.res = nullptr;
        ptcl.stripe = nullptr;
    }
    for (auto& stripe : mStripePool) {
        stripe.ptcl = nullptr;
        stripe.res = nullptr;
    }
}

void PtclSystem::setEmitterSet(const EmitterSet& emitterSet) {
    reset();

    u32 setSeed = 0;
    bool hasSetSeed = false;
    for (const auto& resource : emitterSet.emitters()) {
        const auto& seedConfig = resource->randomSeed();
        u32 generatedSeed = 0;
        if (seedConfig.mode() == PtclSeed::Mode::RandomPerSet) {
            if (!hasSetSeed) {
                setSeed = nextEmitterSeed();
                hasSetSeed = true;
            }
            generatedSeed = setSeed;
        } else if (seedConfig.mode() == PtclSeed::Mode::RandomPerEmitter) {
            generatedSeed = nextEmitterSeed();
        }

        const auto emitterSeed = PtclRandom::makeEmitterSeed(seedConfig, generatedSeed);
        auto instance = std::make_unique<EmitterInstance>(
            *resource, mSetInstance, emitterSeed.value,
            emitterSeed.vec3Index, emitterSeed.normalizedVec3Index);

        mSetInstance.addEmitter(*instance);
        addEmitter(*instance);
        mOwnedEmitters.push_back(std::move(instance));
    }
}

void PtclSystem::calc() {
    mFrame.clear();

    for (auto it = mEmitters.begin(); it != mEmitters.end();) {
        EmitterInstance* emitter = *it++;
        if (emitter->alive()) {
            EmitterCalc::calc(*emitter, mFrame);
        }
    }

    ++mFrameIndex;
}

bool PtclSystem::hasVisibleDraw() const {
    return !mFrame.stripes.empty() || liveParticleCount() > 0;
}

s32 PtclSystem::liveParticleCount() const {
    s32 count = 0;

    for (const auto& batch : mFrame.batches) {
        count += static_cast<s32>(batch.particles.size());
    }

    for (const auto& batch : mFrame.compactBatches) {
        count += static_cast<s32>(batch.particles.size());
    }

    return count;
}


// ========================================================================== //


} // namespace Ptcl::Engine
