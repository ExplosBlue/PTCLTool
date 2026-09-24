#include "ptcl/engine/emitterCalc.h"

#include "ptcl/engine/emitterInstance.h"
#include "ptcl/engine/emitterSetInstance.h"
#include "ptcl/engine/ptclInstance.h"
#include "ptcl/engine/ptclRenderer.h"
#include "ptcl/engine/ptclSystem.h"

#include <cstddef>


namespace Ptcl::Engine {


// ========================================================================== //


s32 EmitterCalc::calcSimple(EmitterInstance& emitter, PtclRenderer& renderer) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    bool canEmit;
    if (!stepFadeAlpha(emitter, canEmit)) {
        return 0;
    }

    const s32 frame = emitter.mTickCount - set.mStartFrame;
    scheduleEmission(emitter, frame, canEmit);

    if (!(res.emitEndFrame() > frame && canEmit) && emitter.mParticles.empty()) {
        mSys->killEmitter(emitter);
        return 0;
    }

    ++emitter.mTickCount;
    if (emitter.mParticles.empty()) {
        return 0;
    }

    beginEmitterMatrices(emitter);
    emitParentState(renderer, emitter);

    s32 aliveCount = 0;

    for (auto it = emitter.mParticles.begin(); it != emitter.mParticles.end();) {
        PtclInstance* ptcl = *it++;
        if (ptcl->life > ptcl->tickCount) {
            ParticleState draw = sSimplePreCalcFunctions[static_cast<std::size_t>(res.followType())](emitter, *ptcl);
            sPtclCalcFunctions[ptclRotCombinerIndex(res)](&draw.matrix, emitter, *ptcl);

            emitParticleState(renderer, set, emitter, draw, *ptcl);
            ++aliveCount;
        } else {
            emitter.mParticles.remove(ptcl);
        }
    }

    return aliveCount;
}


// ========================================================================== //


} // namespace Ptcl::Engine
