#include "ptcl/engine/ptclRandom.h"

namespace Ptcl::Engine {


// ========================================================================== //


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
        constexpr f32 cU32ToF32Inverse = 1.0f / 4294967296.f; // 1 / 0x100000000
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
            vec3[i].setX(rnd.getF32() * cTableScale - cTableOffset);
            vec3[i].setY(rnd.getF32() * cTableScale - cTableOffset);
            vec3[i].setZ(rnd.getF32() * cTableScale - cTableOffset);

            normalizedVec3[i].setX(rnd.getF32() * cTableScale - cTableOffset);
            normalizedVec3[i].setY(rnd.getF32() * cTableScale - cTableOffset);
            normalizedVec3[i].setZ(rnd.getF32() * cTableScale - cTableOffset);
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

void PtclRandom::SetSeed(u32 val) {
    constexpr u32 cSeedIndexShift = 16;
    mVec3RndIdx = static_cast<u16>(val);
    mNormalizedVec3RndIdx = static_cast<u16>(val >> cSeedIndexShift);
    mRnd = val;
}

const Math::Vector3f& PtclRandom::getVec3() {
    return vec3Table()[(mVec3RndIdx++) & cNumVec3TblMask];
}

const Math::Vector3f& PtclRandom::getNormalizedVec3() {
    return normalizedVec3Table()[(mNormalizedVec3RndIdx++) & cNumVec3TblMask];
}

u32 PtclRandom::getU32Direct() {
    constexpr u32 cLcgMultiplier = 1103515245;
    constexpr u32 cLcgIncrement  = 12345;

    u32 ret = mRnd;
    mRnd = mRnd * cLcgMultiplier + cLcgIncrement;
    return ret;
}


// ========================================================================== //


} // namespace Ptcl::Engine
