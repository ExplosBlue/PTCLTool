#include "ptcl/engine/emitterSetInstance.h"

#include "ptcl/engine/emitterCalc.h"
#include "ptcl/engine/emitterInstance.h"


namespace Ptcl::Engine {


// ========================================================================== //


void EmitterSetInstance::addEmitter(EmitterInstance& emitter) {
    if (mNumEmitter >= static_cast<s32>(mInstance.size())) {
        return;
    }

    mInstance[mNumEmitter] = &emitter;
    ++mNumEmitter;

    emitter.mEmitterSetCreateID = mEmitterCreateID;
    mNumEmitterFirst = mNumEmitter;
}


void EmitterSetInstance::emitParticle(const Math::Vector3f& pos) {
    for (s32 idx = 0; idx < mNumEmitterFirst; ++idx ) {
        EmitterInstance* emitter = mInstance[idx ];
        if (emitter->mEmitterSetCreateID != mEmitterCreateID) {
            continue;
        }

        PtclInstance* savedHead = emitter->mParticles.front();
        EmitterCalc::emitParticle(*emitter);

        for (auto it = emitter->mParticles.begin(); it != emitter->mParticles.end(); ++it) {
            PtclInstance* ptcl = *it;
            if (ptcl == savedHead) {
                break;
            }
            ptcl->position.x += pos.x;
            ptcl->position.y += pos.y;
            ptcl->position.z += pos.z;
        }
    }
}


// ========================================================================== //


} // namespace Ptcl::Engine