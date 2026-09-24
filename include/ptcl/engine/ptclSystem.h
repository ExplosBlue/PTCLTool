#pragma once

#include <cstddef>
#include <list>
#include <memory>
#include <vector>

#include "ptcl/engine/emitterInstance.h"
#include "ptcl/engine/ptclInstance.h"
#include "ptcl/engine/emitterSetInstance.h"
#include "ptcl/engine/ptclRandom.h"
#include "ptcl/engine/ptclRenderer.h"
#include "typedefs.h"

class QOpenGLFunctions_2_0;


namespace Ptcl {
class EmitterSet;
} // namespace Ptcl

namespace Ptcl::Engine {


// ========================================================================== //


class EmitterCalc;
class EmitterInstance;
class EmitterSetInstance;

class PtclSystem {
public:
    static constexpr std::size_t cStripeNumHistory = 128;

public:
    explicit PtclSystem(s32 ptclDataPow2);
    ~PtclSystem() = default;

    PtclInstance* allocPtcl();
    void addEmitter(EmitterInstance& emitter);
    void killEmitter(EmitterInstance& emitter);

    void reset();
    void setEmitterSet(const EmitterSet& emitterSet);

    void initializeRenderer(QOpenGLFunctions_2_0& gl);
    void cleanupRenderer(QOpenGLFunctions_2_0& gl);
    std::size_t render(QOpenGLFunctions_2_0& gl,
        const Math::Matrix44f& viewProjection,
        const Math::Vector3f& right,
        const Math::Vector3f& up,
        const Math::Vector3f& forward,
        s32 viewportWidth, s32 viewportHeight);

    void calc();

    u32 nextEmitterSeed() { return mRandom.getU32(); }
    std::size_t lastDrawCount() const { return mLastDrawCount; }

    const EmitterDrawFrame& lastFrame() const { return mRenderer.frame(); }
    PtclRenderer& renderer() { return mRenderer; }
    const PtclRenderer& renderer() const { return mRenderer; }

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
    std::size_t mLastDrawCount{0};

    std::vector<PtclStripe> mStripePool;
    s32 mNumStripeData;
    s32 mNumStripeDataMask;
    s32 mCurStripeIx{0};

    std::list<PtclStripe*> mStripes;

    std::list<EmitterInstance*> mEmitters;
    EmitterSetInstance mSetInstance{};
    std::vector<std::unique_ptr<EmitterInstance>> mOwnedEmitters;

    PtclRenderer mRenderer;
    PtclRandom mRandom{PtclRandom::getGlobalU32(), 0, 0};
};


// ========================================================================== //


} // namespace Ptcl::Engine
