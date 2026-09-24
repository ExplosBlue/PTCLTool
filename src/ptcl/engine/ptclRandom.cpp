#include "ptcl/engine/ptclRandom.h"

namespace Ptcl::Engine {


// ========================================================================== //


constexpr f32 cU32ToF32Inverse = 1.0f / 4294967296.0f; // 1 / 0x100000000


class MtRandom {
public:
    MtRandom(u32 seed) {
        constexpr u32 cInitMultiplier = 1812433253;
        constexpr u32 cInitShift      = 30;

        mX = (seed ^ (seed >> cInitShift)) * cInitMultiplier + 1;
        mY = (mX   ^ (mX   >> cInitShift)) * cInitMultiplier + 2;
        mZ = (mY   ^ (mY   >> cInitShift)) * cInitMultiplier + 3;
        mW = (mZ   ^ (mZ   >> cInitShift)) * cInitMultiplier + 4;
    }

    u32 getU32() {
        constexpr u32 cTemperLeftShift  = 11;
        constexpr u32 cTemperRightShift = 19;
        constexpr u32 cTemperMidShift   = 8;

        u32 t = (mX ^ (mX << cTemperLeftShift));
        mX = mY;
        mY = mZ;
        mZ = mW;
        mW = (mW ^ (mW >> cTemperRightShift)) ^ (t ^ (t >> cTemperMidShift));
        return mW;
    }

    f32 getF32() {
        return static_cast<f32>(getU32()) * cU32ToF32Inverse;
    }

private:
    u32 mX;
    u32 mY;
    u32 mZ;
    u32 mW;
};

MtRandom& globalRandom() {
    constexpr u32 cGlobalSeed = 0;
    static MtRandom sGlobal(cGlobalSeed);
    return sGlobal;
}

struct Tables {
    Tables() {
        constexpr u32  cTableSeed   = 12345679;
        constexpr f32  cTableScale  = 2.0f;
        constexpr f32  cTableOffset = 1.0f;

        MtRandom rnd(cTableSeed);
        for (u32 i = 0; i < PtclRandom::cNumVec3Tbl; ++i) {
            vec3[i].setX(cTableOffset - rnd.getF32() * cTableScale);
            vec3[i].setY(cTableOffset - rnd.getF32() * cTableScale);
            vec3[i].setZ(cTableOffset - rnd.getF32() * cTableScale);

            normalizedVec3[i].setX(cTableOffset - rnd.getF32() * cTableScale);
            normalizedVec3[i].setY(cTableOffset - rnd.getF32() * cTableScale);
            normalizedVec3[i].setZ(cTableOffset - rnd.getF32() * cTableScale);
            normalizedVec3[i].normalize();
        }
    }

   PtclRandom::Vec3Tbl vec3;
   PtclRandom::Vec3Tbl normalizedVec3;
};

const Tables& tables() {
    static const Tables sTables;
    return sTables;
}


// ========================================================================== //


u32 PtclRandom::getGlobalU32() {
    return globalRandom().getU32();
}

const PtclRandom::Vec3Tbl& PtclRandom::vec3Table() {
    return tables().vec3;
}

const PtclRandom::Vec3Tbl& PtclRandom::normalizedVec3Table() {
    return tables().normalizedVec3;
}

PtclRandom::PtclRandom(u32 rnd, u16 vec3Idx, u16 normalizedVec3Idx) :
    mVec3RndIdx{vec3Idx},
    mNormalizedVec3RndIdx{normalizedVec3Idx},
    mRnd{rnd} {}

u32 PtclRandom::rnd() const {
    return mRnd;
}

u16 PtclRandom::vec3RndIdx() const {
    return mVec3RndIdx;
}

void PtclRandom::setRndRaw(u32 val) {
    mRnd = val;
}

void PtclRandom::SetSeed(u32 val) {
    constexpr u32 cSeedIndexShift = 16;
    mVec3RndIdx = static_cast<u16>(val);
    mNormalizedVec3RndIdx = static_cast<u16>(val >> cSeedIndexShift);
    mRnd = val;
}

PtclRandom::EmitterSeed PtclRandom::makeEmitterSeed(
    const PtclSeed& seed, u32 generatedSeed) {
    const bool isConstant = seed.mode() == PtclSeed::Mode::ConstantSeed;
    const u32 value = isConstant ? seed.constantSeed() : generatedSeed;
    return {
        value,
        static_cast<u16>(value),
        isConstant ? u16{} : static_cast<u16>(value >> 16),
    };
}

const Math::Vector3f& PtclRandom::getVec3() {
    return vec3Table()[(mVec3RndIdx++) & cNumVec3TblMask];
}

const Math::Vector3f& PtclRandom::getNormalizedVec3() {
    return normalizedVec3Table()[(mNormalizedVec3RndIdx++) & cNumVec3TblMask];
}

u32 PtclRandom::getU32() {
    constexpr u32 cLcgMultiplier = 1103515245;
    constexpr u32 cLcgIncrement  = 12345;

    u32 ret = mRnd;
    mRnd = mRnd * cLcgMultiplier + cLcgIncrement;
    return ret;
}

f32 PtclRandom::getF32() {
    return static_cast<f32>(getU32()) * cU32ToF32Inverse;
}

s32 PtclRandom::getS32() {
    return static_cast<s32>(getU32() >> 24);
}

s32 PtclRandom::getS32(s32 max) {
    return static_cast<s32>((static_cast<u64>(getU32()) * max) >> 32);
}

s32 PtclRandom::getAngle() {
    return static_cast<s32>(getU32() >> 1) - 0x3FFFFFFF;
}


// ========================================================================== //


} // namespace Ptcl::Engine
