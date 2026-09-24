#include "ptcl/engine/ptclSystem.h"

#include "ptcl/engine/emitterCalc.h"
#include "ptcl/engine/emitterInstance.h"
#include "ptcl/engine/ptclRenderer.h"

#include <algorithm>
#include <cstdio>


namespace Ptcl::Engine {


// ========================================================================== //


PtclSystem::PtclSystem(s32 ptclDataPow2) {
    mNumPtclData = 1 << ptclDataPow2;
    mNumPtclDataMask = mNumPtclData - 1;
    mPtcls.assign(mNumPtclData, PtclInstance{});

    mNumStripeData = mNumPtclData;
    mNumStripeDataMask = mNumStripeData - 1;
    mStripePool.assign(static_cast<std::size_t>(mNumStripeData), PtclStripe{});

    EmitterCalc::mSys = this;
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
        mCurPtclIx = (mCurPtclIx + 1) & mNumPtclDataMask;

        if (mPtcls[mCurPtclIx].res == nullptr) {
            break;
        }

        if (mNumPtclData <= ++idx) {
            qDebug("PtclSystem: particle pool is full\n");
            return nullptr;
        }
    }

    ++mNumEmittedPtcl;
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
    mStripes.push_front(stripe);

    return stripe;
}

void PtclSystem::removeStripe(PtclStripe& stripe) {
    mStripes.remove(&stripe);
    if (stripe.ptcl != nullptr && stripe.ptcl->stripe == &stripe) {
        stripe.ptcl->stripe = nullptr;
    }
    stripe.ptcl = nullptr;
    stripe.res = nullptr;
    stripe.numHistory = 0;
}

void PtclSystem::reset() {
    mRenderer.beginCalc();
    mRenderer.endCalc();
    mEmitters.clear();
    mStripes.clear();
    mCurPtclIx = 0;
    mNumEmittedPtcl = 0;
    mCurStripeIx = 0;
    mAliveCount = 0;
    mAliveEmitterCount = 0;
    for (auto& ptcl : mPtcls) {
        ptcl.res = nullptr;
        ptcl.stripe = nullptr;
    }
    for (auto& stripe : mStripePool) {
        stripe.ptcl = nullptr;
        stripe.res = nullptr;
    }
}

void PtclSystem::calc() {
    mRenderer.beginCalc();

    mAliveCount = 0;
    mAliveEmitterCount = 0;

    for (auto it = mEmitters.begin(); it != mEmitters.end();) {
        EmitterInstance* emitter = *it++;
        if (emitter->alive()) {
            const s32 count = EmitterCalc::calc(*emitter, mRenderer);
            const s64 total = static_cast<s64>(mAliveCount) + count;
            mAliveCount = static_cast<s32>(std::min<s64>(total, 0x7FFFFFFF));
            if (count != 0) {
                ++mAliveEmitterCount;
            }
        }
    }

    mRenderer.endCalc();
}


// ========================================================================== //


} // namespace Ptcl::Engine
