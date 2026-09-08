#pragma once

#include <array>

#include "math/vector.h"
#include "typedefs.h"


namespace Ptcl::Engine {


// ========================================================================== //


class EmitterCalc;


class PtclRandom {
public:
    static constexpr u32 cNumVec3Tbl = 512;
    static constexpr u32 cNumVec3TblMask = cNumVec3Tbl - 1;

    using Vec3Tbl = std::array<Math::Vector3f, cNumVec3Tbl>;

public:
    PtclRandom(u32 rnd, u16 vec3Idx, u16 normalizedVec3Idx);

    void SetSeed(u32 val);

    const Math::Vector3f& getVec3();
    const Math::Vector3f& getNormalizedVec3();

    u32 getU32();
    f32 getF32();

    s32 getS32();
    s32 getS32(s32 max);

    s32 getAngle();

    u32 rnd() const;
    u16 vec3RndIdx() const;

    void setRndRaw(u32 val);

    static u32 getGlobalU32();

private:
    static const Vec3Tbl& vec3Table();
    static const Vec3Tbl& normalizedVec3Table();

private:
    u16 mVec3RndIdx;
    u16 mNormalizedVec3RndIdx;
    u32 mRnd;

    friend class EmitterCalc;
};


// ========================================================================== //


} // namespace Ptcl::Engine
