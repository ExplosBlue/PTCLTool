#pragma once

#include <vector>

#include "ptcl/engine/ptclInstance.h"
#include "typedefs.h"


namespace Ptcl::Engine {


// ========================================================================== //


class EmitterCalc;

class PtclSystem {
public:
    explicit PtclSystem(s32 ptclDataPow2);

    PtclInstance* allocPtcl();

private:
    static void warn(const char* message);

    std::vector<PtclInstance> mPtcls;
    s32 mNumPtclData;
    s32 mNumPtclDataMask;
    s32 mCurPtclIx{0};
    s32 mNumEmittedPtcl{0};
};


// ========================================================================== //


} // namespace Ptcl::Engine