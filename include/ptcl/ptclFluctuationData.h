#ifndef PTCLFLUCTUATIONDATA_H
#define PTCLFLUCTUATIONDATA_H

#include "typedefs.h"
#include "ptclBinary.h"

namespace Ptcl {


class FluctuationData
{
public:
    FluctuationData() = default;

    FluctuationData(const FluctuationData&) = delete;
    FluctuationData& operator=(const FluctuationData&) = delete;

    f32 scale() const;
    void setScale(const f32 scale);

    f32 freq() const;
    void setFreq(const f32 freq);

    u32 phaseRnd() const;
    void setPhaseRnd(const u32 phaseRnd);

    void initFromBinary(const BinFluctuationData& fluctuationData);

private:
    f32 mScale;
    f32 mFreq;
    u32 mPhaseRnd;
};


} // namespace Ptcl

#endif // PTCLFLUCTUATIONDATA_H
