#include "ptcl/ptclSeed.h"

#include <QtLogging>


// ========================================================================== //


PtclSeed::PtclSeed() :
    mValue{0} {}

PtclSeed::PtclSeed(u32 raw) :
    mValue{raw} {}

PtclSeed::Mode PtclSeed::mode() const {
    if (mValue == static_cast<u32>(PtclSeed::Mode::RandomPerEmitter)) {
        return PtclSeed::Mode::RandomPerEmitter;
    }
    if (mValue == static_cast<u32>(PtclSeed::Mode::RandomPerSet)) {
        return PtclSeed::Mode::RandomPerSet;
    }
    return PtclSeed::Mode::ConstantSeed;
}

void PtclSeed::setMode(PtclSeed::Mode mode)
{
    switch (mode) {
    case PtclSeed::Mode::RandomPerEmitter:
        mValue = static_cast<u32>(PtclSeed::Mode::RandomPerEmitter);
        break;
    case PtclSeed::Mode::RandomPerSet:
        mValue = static_cast<u32>(PtclSeed::Mode::RandomPerSet);
        break;
    case PtclSeed::Mode::ConstantSeed:
        mValue = static_cast<u32>(PtclSeed::Mode::ConstantSeed);
        break;
    }
}

u32 PtclSeed::constantSeed() const {
    return (mode() == PtclSeed::Mode::ConstantSeed) ? mValue : 0;
}

void PtclSeed::setConstantSeed(u32 seed) {
    if (seed > 0 && seed < 0xFFFFFFFF) {
        mValue = seed;
    } else {
        qWarning("Invalid constant seed value: must be in the range 1..0xFFFFFFFE");
    }
}

u32 PtclSeed::raw() const {
    return mValue;
}

void PtclSeed::setRaw(u32 raw) {
    mValue = raw;
}


// ========================================================================== //
