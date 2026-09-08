#include "ptcl/engine/emitterCalc.h"

#include <cmath>

#include "math/sinCosIdx.h"
#include "ptcl/engine/emitterInstance.h"


namespace Ptcl::Engine {


// ========================================================================== //


static void interpolate3Colors(const Ptcl::Emitter& res, PtclInstance& ptcl) {
    const auto& color = res.color0();

    if (res.isColorRandom()) {
        ptcl.color = color[ptcl.random % 3];
        return;
    }

    const s32 numRepeat = res.colorNumRepeat();
    s32 freq = (numRepeat != 0) ? ptcl.life / numRepeat : 0;
    if (freq == 0) {
        freq = ptcl.life;
    }

    const s32 localCount = (ptcl.tickCount - 1) % freq;
    const s32 sec1 = (res.colorSection1() * freq) / 100;
    const s32 sec2 = (res.colorSection2() * freq) / 100;
    const s32 sec3 = (res.colorSection3() * freq) / 100;

    if (localCount < sec1) {
        ptcl.color = color[0];
    } else if (localCount < sec2) {
        const f32 t = static_cast<f32>(localCount - sec1) / static_cast<f32>(sec2 - sec1);

        ptcl.color = {
            std::fmaf(color[1].r() - color[0].r(), t, color[0].r()),
            std::fmaf(color[1].g() - color[0].g(), t, color[0].g()),
            std::fmaf(color[1].b() - color[0].b(), t, color[0].b()),
        };
    } else if (localCount < sec3) {
        const f32 t = static_cast<f32>(localCount - sec2) / static_cast<f32>(sec3 - sec2);

        ptcl.color = {
            std::fmaf(color[2].r() - color[1].r(), t, color[1].r()),
            std::fmaf(color[2].g() - color[1].g(), t, color[1].g()),
            std::fmaf(color[2].b() - color[1].b(), t, color[1].b()),
        };
    } else {
        ptcl.color = color[2];
    }
}

void EmitterCalc::calcRotNone(Math::Matrix34f& matrix, [[maybe_unused]] PtclInstance& ptcl) {
    matrix(0, 1) = 0.0f; matrix(0, 2) = 0.0f; matrix(0, 3) = 1.0f;
    matrix(1, 1) = 0.0f; matrix(1, 2) = 1.0f; matrix(1, 3) = 0.0f;
    matrix(2, 1) = 1.0f; matrix(2, 2) = 0.0f; matrix(2, 3) = 0.0f;
}

void EmitterCalc::calcRotX(Math::Matrix34f& matrix, PtclInstance& ptcl) {
    ptcl.rotation.x = ptcl.rotation.x + ptcl.rotationVelocity.x;

    f32 sinX, cosX;
    Math::sinCosIdx(&sinX, &cosX, static_cast<u32>(ptcl.rotation.x));

    matrix(0, 1) =  0.0f; matrix(0, 2) = 0.0f; matrix(0, 3) = 1.0f;
    matrix(1, 1) = -sinX; matrix(1, 2) = cosX; matrix(1, 3) = 0.0f;
    matrix(2, 1) =  cosX; matrix(2, 2) = sinX; matrix(2, 3) = 0.0f;
}

void EmitterCalc::calcRotY(Math::Matrix34f& matrix, PtclInstance& ptcl) {
    ptcl.rotation.y = ptcl.rotation.y + ptcl.rotationVelocity.y;

    f32 sinY, cosY;
    Math::sinCosIdx(&sinY, &cosY, static_cast<u32>(ptcl.rotation.y));

    matrix(0, 1) = sinY; matrix(0, 2) = 0.0f; matrix(0, 3) =  cosY;
    matrix(1, 1) = 0.0f; matrix(1, 2) = 1.0f; matrix(1, 3) =  0.0f;
    matrix(2, 1) = cosY; matrix(2, 2) = 0.0f; matrix(2, 3) = -sinY;
}

void EmitterCalc::calcRotZ(Math::Matrix34f& matrix, PtclInstance& ptcl) {
    ptcl.rotation.z = ptcl.rotation.z + ptcl.rotationVelocity.z;

    f32 sinZ, cosZ;
    Math::sinCosIdx(&sinZ, &cosZ, static_cast<u32>(ptcl.rotation.z));

    matrix(0, 1) = 0.0f; matrix(0, 2) = -sinZ; matrix(0, 3) = cosZ;
    matrix(1, 1) = 0.0f; matrix(1, 2) =  cosZ; matrix(1, 3) = sinZ;
    matrix(2, 1) = 1.0f; matrix(2, 2) =  0.0f; matrix(2, 3) = 0.0f;
}

void EmitterCalc::calcRotXYZ(Math::Matrix34f& matrix, PtclInstance& ptcl) {
    ptcl.rotation.x = ptcl.rotation.x + ptcl.rotationVelocity.x;
    ptcl.rotation.y = ptcl.rotation.y + ptcl.rotationVelocity.y;
    ptcl.rotation.z = ptcl.rotation.z + ptcl.rotationVelocity.z;

    f32 sinX, cosX, sinY, cosY, sinZ, cosZ;
    Math::sinCosIdx(&sinX, &cosX, static_cast<u32>(ptcl.rotation.x));
    Math::sinCosIdx(&sinY, &cosY, static_cast<u32>(ptcl.rotation.y));
    Math::sinCosIdx(&sinZ, &cosZ, static_cast<u32>(ptcl.rotation.z));

    matrix(0, 1) = std::fmaf(  cosX * cosZ,  sinY, sinX * sinZ);
    matrix(0, 2) = std::fmaf(-(sinX * sinY), cosZ, cosX * sinZ);
    matrix(0, 3) = cosY * cosZ;

    matrix(1, 1) = -std::fmaf(sinX,        cosZ, sinY * (cosX * sinZ));
    matrix(1, 2) =  std::fmaf(sinX * sinY, sinZ, cosX * cosZ);
    matrix(1, 3) =  cosY * sinZ;

    matrix(2, 1) =  cosX * cosY;
    matrix(2, 2) =  sinX * cosY;
    matrix(2, 3) = -sinY;
}


// ========================================================================== //


Math::Matrix34f* EmitterCalc::ptclCalcRotNoneColorNoWork(Math::Matrix34f* buffAddr, [[maybe_unused]] EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    buff(0, 0) = 0.0f;
    buff(1, 0) = 0.0f;
    buff(2, 0) = 0.0f;

    calcRotNone(buff, ptcl);

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::ptclCalcRotXColorNoWork(Math::Matrix34f* buffAddr, [[maybe_unused]] EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    buff(0, 0) = 0.0f;
    buff(1, 0) = 0.0f;
    buff(2, 0) = 0.0f;

    calcRotX(buff, ptcl);

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::ptclCalcRotYColorNoWork(Math::Matrix34f* buffAddr, [[maybe_unused]] EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    buff(0, 0) = 0.0f;
    buff(1, 0) = 0.0f;
    buff(2, 0) = 0.0f;

    calcRotY(buff, ptcl);

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::ptclCalcRotZColorNoWork(Math::Matrix34f* buffAddr, [[maybe_unused]] EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    buff(0, 0) = 0.0f;
    buff(1, 0) = 0.0f;
    buff(2, 0) = 0.0f;

    calcRotZ(buff, ptcl);

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::ptclCalcRotXYZColorNoWork(Math::Matrix34f* buffAddr, [[maybe_unused]] EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    buff(0, 0) = 0.0f;
    buff(1, 0) = 0.0f;
    buff(2, 0) = 0.0f;

    calcRotXYZ(buff, ptcl);

    return ++buffAddr;
}


// ========================================================================== //


Math::Matrix34f* EmitterCalc::ptclCalcRotNonePassColor1(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const auto& color = res.color0()[0];
    buff(0, 0) = color.r() * set.color.r();
    buff(1, 0) = color.g() * set.color.g();
    buff(2, 0) = color.b() * set.color.b();

    calcRotNone(buff, ptcl);

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::ptclCalcRotXPassColor1(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const auto& color = res.color0()[0];
    buff(0, 0) = color.r() * set.color.r();
    buff(1, 0) = color.g() * set.color.g();
    buff(2, 0) = color.b() * set.color.b();

    calcRotX(buff, ptcl);

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::ptclCalcRotYPassColor1(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const auto& color = res.color0()[0];
    buff(0, 0) = color.r() * set.color.r();
    buff(1, 0) = color.g() * set.color.g();
    buff(2, 0) = color.b() * set.color.b();

    calcRotY(buff, ptcl);

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::ptclCalcRotZPassColor1(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const auto& color = res.color0()[0];
    buff(0, 0) = color.r() * set.color.r();
    buff(1, 0) = color.g() * set.color.g();
    buff(2, 0) = color.b() * set.color.b();

    calcRotZ(buff, ptcl);

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::ptclCalcRotXYZPassColor1(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const auto& color = res.color0()[0];
    buff(0, 0) = color.r() * set.color.r();
    buff(1, 0) = color.g() * set.color.g();
    buff(2, 0) = color.b() * set.color.b();

    calcRotXYZ(buff, ptcl);

    return ++buffAddr;
}


// ========================================================================== //


Math::Matrix34f* EmitterCalc::ptclCalcRotNoneInterpolate3Colors(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    interpolate3Colors(res, ptcl);

    buff(0, 0) = ptcl.color.r() * set.color.r();
    buff(1, 0) = ptcl.color.g() * set.color.g();
    buff(2, 0) = ptcl.color.b() * set.color.b();

    calcRotNone(buff, ptcl);

    buff(0, 2) = 1.0f;
    buff(0, 3) = 0.0f;

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::ptclCalcRotXInterpolate3Colors(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    interpolate3Colors(res, ptcl);

    buff(0, 0) = ptcl.color.r() * set.color.r();
    buff(1, 0) = ptcl.color.g() * set.color.g();
    buff(2, 0) = ptcl.color.b() * set.color.b();

    calcRotX(buff, ptcl);

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::ptclCalcRotYInterpolate3Colors(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    interpolate3Colors(res, ptcl);

    buff(0, 0) = ptcl.color.r() * set.color.r();
    buff(1, 0) = ptcl.color.g() * set.color.g();
    buff(2, 0) = ptcl.color.b() * set.color.b();

    calcRotY(buff, ptcl);

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::ptclCalcRotZInterpolate3Colors(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    interpolate3Colors(res, ptcl);

    buff(0, 0) = ptcl.color.r() * set.color.r();
    buff(1, 0) = ptcl.color.g() * set.color.g();
    buff(2, 0) = ptcl.color.b() * set.color.b();

    calcRotZ(buff, ptcl);

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::ptclCalcRotXYZInterpolate3Colors(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    interpolate3Colors(res, ptcl);

    buff(0, 0) = ptcl.color.r() * set.color.r();
    buff(1, 0) = ptcl.color.g() * set.color.g();
    buff(2, 0) = ptcl.color.b() * set.color.b();

    calcRotXYZ(buff, ptcl);

    return ++buffAddr;
}


// ========================================================================== //


} // namespace Ptcl::Engine
