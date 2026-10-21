#pragma once

#include <cstddef>
#include <list>
#include <memory>
#include <vector>

#include "ptcl/engine/emitterInstance.h"
#include "ptcl/engine/emitterSetInstance.h"
#include "ptcl/engine/ptclInstance.h"
#include "ptcl/engine/ptclRandom.h"
#include "ptcl/engine/renderContract.h"
#include "typedefs.h"

namespace Ptcl {
class EmitterSet;
} // namespace Ptcl

namespace Ptcl::Engine {


// ========================================================================== //


struct PtclSystemConfig {
    s32 regularParticleCapacity{256};
    s32 compactParticleCapacity{1024};
};

class PtclSystem {
public:
    static constexpr std::size_t cStripeNumHistory = 128;

    explicit PtclSystem(PtclSystemConfig config = {});
    ~PtclSystem();

    PtclInstance* allocPtcl();
    PtclInstance* allocCompactPtcl();
    void addEmitter(EmitterInstance& emitter);
    void killEmitter(EmitterInstance& emitter);

    void reset();
    void setEmitterSet(const EmitterSet& emitterSet);

    void calc();

    u32 nextEmitterSeed() { return mRandom.getU32(); }

    const DrawFrame& lastFrame() const { return mFrame; }

    PtclStripe* allocAndConnectStripe(EmitterInstance& emitter, PtclInstance& ptcl);
    void removeStripe(PtclStripe& stripe);

private:
    std::vector<PtclInstance> mPtcls;
    s32 mNumPtclData;
    s32 mCurPtclIx{0};

    std::vector<PtclInstance> mCompactPtcls;
    s32 mCompactCurPtclIx{0};
    s32 mNumCompactPtclData;

    std::vector<PtclStripe> mStripePool;
    s32 mNumStripeData;
    s32 mNumStripeDataMask;
    s32 mCurStripeIx{0};

    std::list<EmitterInstance*> mEmitters;
    EmitterSetInstance mSetInstance{};
    std::vector<std::unique_ptr<EmitterInstance>> mOwnedEmitters;

    DrawFrame mFrame;
    PtclRandom mRandom{PtclRandom::getGlobalU32(), 0, 0};
};


// ========================================================================== //


} // namespace Ptcl::Engine
