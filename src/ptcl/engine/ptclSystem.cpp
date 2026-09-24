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
    mStripes.assign(static_cast<std::size_t>(mNumStripeData), PtclStripe{});

    EmitterCalc::mSys = this;
}

void PtclSystem::addEmitter(EmitterInstance& emitter) {
    emitter.mPrev = mEmittersLast;
    if (mEmittersLast == nullptr) {
        mEmittersFirst = &emitter;
    } else {
        mEmittersLast->mNext = &emitter;
    }
    mEmittersLast = &emitter;
}

void PtclSystem::killEmitter(EmitterInstance& emitter) {
    if (emitter.mIsAlive == false) {
        return;
    }

    EmitterInstance* prev = emitter.mPrev;
    EmitterInstance* next = emitter.mNext;

    if (prev == nullptr) {
        mEmittersFirst = next;
    } else {
        prev->mNext = next;
    }

    if (next == nullptr) {
        mEmittersLast = prev;
    } else {
        next->mPrev = prev;
    }

    for (PtclInstance* ptcl = emitter.mPtclHead; ptcl != nullptr;) {
        PtclInstance* nxt = ptcl->next;
        if (ptcl->stripe != nullptr) {
            removeStripe(*ptcl->stripe);
        }
        ptcl->res = nullptr;
        ptcl->prev = nullptr;
        ptcl->next = nullptr;
        ptcl = nxt;
    }
    emitter.mPtclHead = nullptr;

    for (PtclInstance* child = emitter.mChildHead; child != nullptr;) {
        PtclInstance* nxt = child->next;
        if (child->stripe != nullptr) {
            removeStripe(*child->stripe);
        }
        child->res = nullptr;
        child->prev = nullptr;
        child->next = nullptr;
        child = nxt;
    }
    emitter.mChildHead = nullptr;

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

        PtclStripe* stripe = &mStripes[mCurStripeIx];
        if (stripe->ptcl == nullptr) {
            break;
        }

        if (mNumStripeData <= ++idx) {
            removeStripe(*stripe);
            break;
        }
    }

    PtclStripe* stripe = &mStripes[mCurStripeIx];
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
    stripe->prev = nullptr;
    stripe->next = mStripeFirst;

    if (mStripeFirst != nullptr) {
        mStripeFirst->prev = stripe;
    }
    mStripeFirst = stripe;

    return stripe;
}

void PtclSystem::removeStripe(PtclStripe& stripe) {
    if (stripe.prev != nullptr) {
        stripe.prev->next = stripe.next;
    } else {
        mStripeFirst = stripe.next;
    }
    if (stripe.next != nullptr) {
        stripe.next->prev = stripe.prev;
    }
    if (stripe.ptcl != nullptr && stripe.ptcl->stripe == &stripe) {
        stripe.ptcl->stripe = nullptr;
    }
    stripe.ptcl = nullptr;
    stripe.res = nullptr;
    stripe.prev = nullptr;
    stripe.next = nullptr;
    stripe.numHistory = 0;
}

void PtclSystem::reset() {
    mRenderer.beginCalc();
    mRenderer.endCalc();
    mEmittersFirst = nullptr;
    mEmittersLast = nullptr;
    mStripeFirst = nullptr;
    mCurPtclIx = 0;
    mNumEmittedPtcl = 0;
    mCurStripeIx = 0;
    mAliveCount = 0;
    mAliveEmitterCount = 0;
    for (auto& ptcl : mPtcls) {
        ptcl.res = nullptr;
        ptcl.prev = nullptr;
        ptcl.next = nullptr;
        ptcl.stripe = nullptr;
    }
    for (auto& stripe : mStripes) {
        stripe.ptcl = nullptr;
        stripe.res = nullptr;
        stripe.prev = nullptr;
        stripe.next = nullptr;
    }
}

void PtclSystem::calc() {
    mRenderer.beginCalc();

    mAliveCount = 0;
    mAliveEmitterCount = 0;

    EmitterInstance* emitter = mEmittersFirst;
    while (emitter != nullptr) {
        EmitterInstance* next = emitter->mNext;
        if (emitter->alive()) {
            const s32 count = EmitterCalc::calc(*emitter, mRenderer);
            const s64 total = static_cast<s64>(mAliveCount) + count;
            mAliveCount = static_cast<s32>(std::min<s64>(total, 0x7FFFFFFF));
            if (count != 0) {
                ++mAliveEmitterCount;
            }
        }
        emitter = next;
    }

    mRenderer.endCalc();
}


// ========================================================================== //


} // namespace Ptcl::Engine
