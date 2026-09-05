#include "ptcl/engine/ptclSystem.h"

#include "ptcl/engine/emitterCalc.h"

#include <cstdio>


namespace Ptcl::Engine {


// ========================================================================== //


PtclSystem::PtclSystem(s32 ptclDataPow2) {
    mNumPtclData = 1 << ptclDataPow2;
    mNumPtclDataMask = mNumPtclData - 1;
    mPtcls.assign(mNumPtclData, PtclInstance{});

    EmitterCalc::mSys = this;
}

PtclInstance* PtclSystem::allocPtcl() {
    s32 idx = 0;

    while (true) {
        mCurPtclIx = (mCurPtclIx + 1) & mNumPtclDataMask;

        if (mPtcls[mCurPtclIx].res == nullptr)
            break;

        if (mNumPtclData <= ++idx) {
            warn("PtclSystem: particle pool is full");
            return nullptr;
        }
    }

    ++mNumEmittedPtcl;
    return &mPtcls[mCurPtclIx];
}

void PtclSystem::warn(const char* message) {
    std::fprintf(stderr, "%s\n", message);
}


// ========================================================================== //


} // namespace Ptcl::Engine