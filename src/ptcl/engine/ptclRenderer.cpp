#include "ptcl/engine/ptclRenderer.h"


namespace Ptcl::Engine {


// ========================================================================== //


void PtclRenderer::beginCalc() {
    mFrame.batches.clear();
    mFrame.stripes.clear();
}

void PtclRenderer::setEmitterState(const EmitterState& state) {
    mFrame.batches.push_back({state, {}});
}

void PtclRenderer::emitParticle(const ParticleState& particle) {
    if (mFrame.batches.empty()) {
        return;
    }
    mFrame.batches.back().particles.push_back(particle);
}

void PtclRenderer::emitStripe(const EmitterState& state, const StripeDraw& stripe) {
    mFrame.stripes.push_back({state, stripe});
}

void PtclRenderer::endCalc() {
}


// ========================================================================== //


} // namespace Ptcl::Engine
