#include "ptcl/ptclFluctuationData.h"

namespace Ptcl {

f32 FluctuationData::scale() const {
    return mScale;
}

void FluctuationData::setScale(const f32 scale) {
    mScale = scale;
}

f32 FluctuationData::freq() const {
    return mFreq;
}

void FluctuationData::setFreq(const f32 freq) {
    mFreq = freq;
}

u32 FluctuationData::phaseRnd() const {
    return mPhaseRnd;
}

void FluctuationData::setPhaseRnd(const u32 phaseRnd) {
    mPhaseRnd = phaseRnd;
}

void FluctuationData::initFromBinary(const BinFluctuationData& fluctuationData) {
    mScale = fluctuationData.fluctuationScale;
    mFreq = fluctuationData.fluctuationFreq;
    mPhaseRnd = fluctuationData.fluctuationPhaseRnd;
}


} // namespace Ptcl
