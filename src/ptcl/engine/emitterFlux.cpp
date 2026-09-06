#include "ptcl/engine/emitterCalc.h"

#include "ptcl/engine/emitterCalcTables.h"
#include "ptcl/engine/emitterInstance.h"


namespace Ptcl::Engine {


// ========================================================================== //


void EmitterCalc::calcFluctuation(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;

    const f32 frequency = res.fluctuationFrequency();
    const u32 phase = res.isFluctuationPhaseRandom() ? ptcl.random : 0u;

    const s32 frame = static_cast<s32>(static_cast<f32>(ptcl.tickCount) * frequency);

    const u32 tableIdx = (static_cast<u32>(frame) + phase) & 0x7Fu;
    const f32 fluctuation = 1.0f - sFluctuationTable[tableIdx] * res.fluctuationScale();

    if (res.isFluctuationApplyAlpha()) {
        ptcl.fluctuationAlpha = fluctuation;
    }
    if (res.isFluctuationApplyScale()) {
        ptcl.fluctuationScale = fluctuation;
    }
}


// ========================================================================== //


} // namespace Ptcl::Engine
