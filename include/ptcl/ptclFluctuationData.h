#pragma once

#include "ptcl/ptclBinary.h"
#include "typedefs.h"

namespace Ptcl {


// ========================================================================== //


class FluctuationData
{
public:
    FluctuationData() = default;

    FluctuationData(const FluctuationData&) = delete;
    FluctuationData& operator=(const FluctuationData&) = delete;

    f32 scale() const;
    void setScale(f32 scale);

    f32 freq() const;
    void setFreq(f32 freq);

    u32 phaseRnd() const;
    void setPhaseRnd(u32 phaseRnd);

    void initFromBinary(const BinFluctuationData& fluctuationData);

private:
    f32 mScale;
    f32 mFreq;
    u32 mPhaseRnd;
};


// ========================================================================== //


} // namespace Ptcl
