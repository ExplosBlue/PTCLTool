#pragma once

#include <vector>

#include "ptcl/engine/ptclInstance.h"
#include "typedefs.h"


namespace Ptcl::Engine {


// ========================================================================== //


class EmitterCalc;
class EmitterInstance;
class PtclRenderer;

class PtclSystem {
public:
    static constexpr std::size_t cStripeNumHistory = 128;

public:
    explicit PtclSystem(s32 ptclDataPow2);

    PtclInstance* allocPtcl();
    void addEmitter(EmitterInstance& emitter);
    void killEmitter(EmitterInstance& emitter);
    void calc(PtclRenderer& renderer);

    s32 lastFrameAliveCount() const { return mAliveCount; }
    s32 lastFrameEmitterCount() const { return mAliveEmitterCount; }

    PtclStripe* allocAndConnectStripe(EmitterInstance& emitter, PtclInstance& ptcl);
    void removeStripe(PtclStripe& stripe);

private:
    static void warn(const char* message);

    std::vector<PtclInstance> mPtcls;
    s32 mNumPtclData;
    s32 mNumPtclDataMask;
    s32 mCurPtclIx{0};
    s32 mNumEmittedPtcl{0};

    s32 mAliveCount{0};
    s32 mAliveEmitterCount{0};

    std::vector<PtclStripe> mStripes;
    s32 mNumStripeData;
    s32 mNumStripeDataMask;
    s32 mCurStripeIx{0};

    PtclStripe* mStripeFirst{nullptr};

    EmitterInstance* mEmittersFirst{nullptr};
    EmitterInstance* mEmittersLast{nullptr};
};


// ========================================================================== //


} // namespace Ptcl::Engine