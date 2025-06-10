#pragma once

#include "typedefs.h"


// ========================================================================== //


class PtclSeed {
public:
    enum class Mode : u32 {
        RandomPerEmitter    = 0x00000000,
        ConstantSeed        = 0x10000000,
        RandomPerSet        = 0xFFFFFFFF
    };

    PtclSeed();

    explicit PtclSeed(u32 raw);

    Mode mode() const;
    void setMode(Mode mode);

    u32 constantSeed() const;
    void setConstantSeed(u32 seed);

    u32 raw() const;
    void setRaw(u32 raw);

private:
    u32 mValue;
};


// ========================================================================== //
