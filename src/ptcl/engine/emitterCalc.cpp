#include "ptcl/engine/emitterCalc.h"

#include "math/sinCosIdx.h"
#include "ptcl/engine/emitterInstance.h"
#include "ptcl/engine/emitterCalcTables.h"
#include "ptcl/engine/ptclSystem.h"


namespace Ptcl::Engine {


// ========================================================================== //


constexpr s32 sInfiniteLifetime = 0x7FFFFFFF;
constexpr u32 cHalfU32 = 0x7FFFFFFFu;

static s32 emitCount(const Ptcl::Emitter& res) {
    return static_cast<s32>((static_cast<s64>(res.emitRate()) * 256) >> 8);
}

PtclSystem* EmitterCalc::mSys = nullptr;


// ========================================================================== //


void EmitterCalc::emitCommon(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const f32 velRand = 1.0f - (res.initialVelocityRandom() * set.randomVelocity) * emitter.mRandom.getF32();
    const f32 dirVel = res.initialVelocity() * set.directionalVelocity;

    if (set.isSetDirectional) {
        ptcl.velocity.x = (ptcl.velocity.x + set.directional.x * dirVel) * velRand;
        ptcl.velocity.y = (ptcl.velocity.y + set.directional.y * dirVel) * velRand;
        ptcl.velocity.z = (ptcl.velocity.z + set.directional.z * dirVel) * velRand;
    } else {
        ptcl.velocity.x = (ptcl.velocity.x + res.velocityDirection().x * dirVel) * velRand;
        ptcl.velocity.y = (ptcl.velocity.y + res.velocityDirection().y * dirVel) * velRand;
        ptcl.velocity.z = (ptcl.velocity.z + res.velocityDirection().z * dirVel) * velRand;
    }

    const Math::Vector3f& randVec3 = emitter.mRandom.getVec3();
    ptcl.velocity.x += randVec3.x * res.spreadVector().x;
    ptcl.velocity.y += randVec3.y * res.spreadVector().y;
    ptcl.velocity.z += randVec3.z * res.spreadVector().z;

    const auto& velAdd = set.velocityAdd;
    ptcl.velocity.x += velAdd.x * set.RT(0, 0) + velAdd.y * set.RT(1, 0) + velAdd.z * set.RT(2, 0);
    ptcl.velocity.y += velAdd.x * set.RT(0, 1) + velAdd.y * set.RT(1, 1) + velAdd.z * set.RT(2, 1);
    ptcl.velocity.z += velAdd.x * set.RT(0, 2) + velAdd.y * set.RT(1, 2) + velAdd.z * set.RT(2, 2);

    ptcl.tickCount = 0;
    ptcl.random = emitter.mRandom.getU32();

    if (res.ptclLife() == sInfiniteLifetime) {
        ptcl.life = sInfiniteLifetime;
    } else {
        const s32 ptclLifeRnd = emitter.mRandom.getS32(res.ptclLifeRandom());
        const s64 life = static_cast<s64>(res.ptclLife()) - ptclLifeRnd;
        ptcl.life = static_cast<s32>((life * emitter.mController.lifeScale) >> 8);
    }

    const f32 scaleRnd = 1.0f - res.scaleRand() * emitter.mRandom.getF32();
    const f32 initScaleX = set.emissionParticleScale.x * scaleRnd;
    const f32 initScaleY = set.emissionParticleScale.y * scaleRnd;

    const s32 maxTime = ptcl.life - 1;

    if (ptcl.life == 1) {
        // Alpha
        ptcl.alphaSection1 = -1;
        ptcl.alphaSection2 = 0x7FFFFFFF;
        ptcl.alphaAddSection1 = 0.0f;
        ptcl.alphaAddSection2 = 0.0f;
        ptcl.alpha = res.alphaAnim().initAlpha;

        // Scale
        ptcl.scaleSection1 = -1;
        ptcl.scaleSection2 = 0x7FFFFFFF;
        ptcl.scaleAddSection1.setZero();
        ptcl.scaleAddSection2.setZero();

        ptcl.scale.x = res.scaleAnim().initScale.x * initScaleX;
        ptcl.scale.y = res.scaleAnim().initScale.y * initScaleY;
    } else {
        // Alpha
        const s32 rawAlphaSec1 = res.alphaAnim().isFlatStart ? -127 : res.alphaAnim().alphaSection1;
        ptcl.alphaSection1 = (rawAlphaSec1 * maxTime) / 100;
        ptcl.alphaSection2 = (res.alphaAnim().alphaSection2 * maxTime) / 100;

        ptcl.alphaAddSection1 = res.alphaAnim().diffAlpha21 / static_cast<f32>(ptcl.alphaSection1);
        ptcl.alphaAddSection2 = res.alphaAnim().diffAlpha32 / static_cast<f32>(maxTime - ptcl.alphaSection2);

        ptcl.alpha = res.alphaAnim().initAlpha - ptcl.alphaAddSection1;

        // Scale
        const s32 rawScaleSec1 = res.scaleAnim().isFlatStart ? -127 : res.scaleAnim().scaleSection1;
        ptcl.scaleSection1 = (rawScaleSec1 * maxTime) / 100;
        ptcl.scaleSection2 = (res.scaleAnim().scaleSection2 * maxTime) / 100;

        const f32 invScaleSec1 = 1.0f / static_cast<f32>(ptcl.scaleSection1);
        const f32 invScaleSec2 = 1.0f / static_cast<f32>(maxTime - ptcl.scaleSection2);

        const f32 scaleAddSec1X = res.scaleAnim().diffScale21.x * invScaleSec1;
        const f32 scaleAddSec1Y = res.scaleAnim().diffScale21.y * invScaleSec1;

        const f32 scaleAddSec2X = res.scaleAnim().diffScale32.x * invScaleSec2;
        const f32 scaleAddSec2Y = res.scaleAnim().diffScale32.y * invScaleSec2;

        const f32 scaleX = res.scaleAnim().initScale.x - scaleAddSec1X * initScaleX;
        const f32 scaleY = res.scaleAnim().initScale.y - scaleAddSec1Y * initScaleY;

        ptcl.scaleAddSection1.x = scaleAddSec1X * initScaleX;
        ptcl.scaleAddSection1.y = scaleAddSec1Y * initScaleY;

        ptcl.scaleAddSection2.x = scaleAddSec2X * initScaleX;
        ptcl.scaleAddSection2.y = scaleAddSec2Y * initScaleY;

        ptcl.scale.x = scaleX * initScaleX;
        ptcl.scale.y = scaleY * initScaleY;
    }

    ptcl.rotation.x = res.initialRotation().x + emitter.mRandom.getS32() * res.initialRotationRandom().x;
    ptcl.rotation.y = res.initialRotation().y + emitter.mRandom.getS32() * res.initialRotationRandom().y;
    ptcl.rotation.z = res.initialRotation().z + emitter.mRandom.getS32() * res.initialRotationRandom().z;

    ptcl.rotationVelocity.x = res.rotationVelocity().x + emitter.mRandom.getS32() * res.rotationVelocityRandom().x;
    ptcl.rotationVelocity.y = res.rotationVelocity().y + emitter.mRandom.getS32() * res.rotationVelocityRandom().y;
    ptcl.rotationVelocity.z = res.rotationVelocity().z + emitter.mRandom.getS32() * res.rotationVelocityRandom().z;

    if (!res.isFollow()) {
        ptcl.emitterRT = Math::MatrixCalc::multiply(set.RT, res.transformRT());
        ptcl.emitterSRT = Math::MatrixCalc::multiply(set.SRT, res.transformSRT());
    }

    if (res.numTexturePattern() <= 1) {
        ptcl.uvOffset.x = 0.0f;
        ptcl.uvOffset.y = 0.0f;
        ptcl.uvSubOffset = res.textureUVScale();
    } else {
        const s32 num = emitter.mRandom.getS32(res.numTexturePattern());
        const s32 numX = num % res.numTextureDivisionX();
        const s32 numY = num / res.numTextureDivisionX();

        const f32 texUScale = res.textureUVScale().x;
        const f32 texVScale = res.textureUVScale().y;

        ptcl.uvSubOffset.x = texUScale;
        ptcl.uvSubOffset.y = texVScale;

        ptcl.uvOffset.x = texUScale * static_cast<f32>(numX);
        ptcl.uvOffset.y = texVScale * static_cast<f32>(numY);
    }

    addParticle(emitter, ptcl);
}

void EmitterCalc::addParticle(EmitterInstance& emitter, PtclInstance& ptcl) {
    emitter.mParticles.push_front(&ptcl);
}

PtclInstance* EmitterCalc::emitParticle(EmitterInstance& emitter) {
    const auto type = static_cast<std::size_t>(emitter.mResource.volumeType());
    return sEmitFunctions[type](emitter);
}

PtclInstance* EmitterCalc::emitPoint(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const s32 emitRate = emitCount(res);

    if (emitRate <= 0) {
        return nullptr;
    }

    const f32 figureVelocity = res.figureVelocity() * set.figureVelocity;

    PtclInstance* lastPtcl = nullptr;
    for (s32 idx = 0; idx < emitRate; ++idx) {
        auto* ptcl = EmitterCalc::mSys->allocPtcl();
        if (!ptcl) { continue; }

        ptcl->res = &res;
        ptcl->stripe = nullptr;

        ptcl->position.setZero();

        const Math::Vector3f& randVec3 = emitter.mRandom.getNormalizedVec3();
        ptcl->velocity.x = randVec3.x * figureVelocity;
        ptcl->velocity.y = randVec3.y * figureVelocity;
        ptcl->velocity.z = randVec3.z * figureVelocity;

        emitCommon(emitter, *ptcl);
        lastPtcl = ptcl;
    }

    return lastPtcl;
}

PtclInstance* EmitterCalc::emitCircle(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const s32 emitRate = emitCount(res);

    if (emitRate <= 0) {
        return nullptr;
    }

    const f32 figureVelocity = res.figureVelocity() * set.figureVelocity;
    const f32 scaleX = res.volumeRadius().x * set.emitterScale.x;
    const f32 scaleZ = res.volumeRadius().z * set.emitterScale.z;

    const u32 arcStart = static_cast<u32>(res.volumeArcStart());
    const u32 arcWidth = static_cast<u32>(res.volumeArcWidth());

    PtclInstance* lastPtcl = nullptr;
    for (s32 idx = 0; idx < emitRate; ++idx) {
        auto* ptcl = EmitterCalc::mSys->allocPtcl();
        if (ptcl == nullptr) { continue; }

        ptcl->res = &res;
        ptcl->stripe = nullptr;

        f32 sinV, cosV;
        const u32 randOffset = emitter.mRandom.getU32();
        Math::sinCosIdx(&sinV, &cosV, (randOffset % arcStart) + arcWidth);

        ptcl->position.x = sinV * scaleX;
        ptcl->position.y = 0.0f;
        ptcl->position.z = cosV * scaleZ;

        ptcl->velocity.x = sinV * figureVelocity;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = cosV * figureVelocity;

        emitCommon(emitter, *ptcl);
        lastPtcl = ptcl;
    }

    return lastPtcl;
}

PtclInstance* EmitterCalc::emitCircleSameDivide(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const s32 emitRate = emitCount(res);

    if (emitRate <= 0) {
        return nullptr;
    }

    const f32 figureVelocity = res.figureVelocity() * set.figureVelocity;
    const f32 scaleX = res.volumeRadius().x * set.emitterScale.x;
    const f32 scaleZ = res.volumeRadius().z * set.emitterScale.z;

    const u32 arcWidth = static_cast<u32>(res.volumeArcWidth());
    const u32 arcStart = static_cast<u32>(res.volumeArcStart());

    u32 currentRotation = arcWidth;

    PtclInstance* lastPtcl = nullptr;

    for (s32 idx = 0; idx < emitRate; ++idx) {
        auto* ptcl = EmitterCalc::mSys->allocPtcl();
        if (ptcl == nullptr) { continue; }

        ptcl->res = &res;
        ptcl->stripe = nullptr;

        f32 sinV, cosV;
        Math::sinCosIdx(&sinV, &cosV, currentRotation);

        ptcl->position.x = sinV * scaleX;
        ptcl->position.y = 0.0f;
        ptcl->position.z = cosV * scaleZ;

        ptcl->velocity.x = sinV * figureVelocity;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = cosV * figureVelocity;

        emitCommon(emitter, *ptcl);
        lastPtcl = ptcl;

        currentRotation += arcStart;
    }

    return lastPtcl;
}

PtclInstance* EmitterCalc::emitFillCircle(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const s32 emitRate = emitCount(res);

    if (emitRate <= 0) {
        return nullptr;
    }

    const f32 figureVelocity = res.figureVelocity() * set.figureVelocity;
    const f32 scaleX = res.volumeRadius().x * set.emitterScale.x;
    const f32 scaleZ = res.volumeRadius().z * set.emitterScale.z;

    const u32 arcWidth = static_cast<u32>(res.volumeArcWidth());
    const u32 arcStart = static_cast<u32>(res.volumeArcStart());

    PtclInstance* lastPtcl = nullptr;

    for (s32 idx = 0; idx < emitRate; ++idx) {
        auto* ptcl = EmitterCalc::mSys->allocPtcl();
        if (ptcl == nullptr) { continue; }

        ptcl->res = &res;
        ptcl->stripe = nullptr;

        const u32 rotation = (emitter.mRandom.getU32() % arcStart) + arcWidth;
        const f32 radius = emitter.mRandom.getF32();

        f32 sinV, cosV;
        Math::sinCosIdx(&sinV, &cosV, rotation);

        ptcl->position.x = (sinV * radius) * scaleX;
        ptcl->position.y = 0.0f;
        ptcl->position.z = (cosV * radius) * scaleZ;

        ptcl->velocity.x = sinV * figureVelocity;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = cosV * figureVelocity;

        emitCommon(emitter, *ptcl);
        lastPtcl = ptcl;
    }

    return lastPtcl;
}

PtclInstance* EmitterCalc::emitSphere(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const s32 emitRate = emitCount(res);

    if (emitRate <= 0) {
        return nullptr;
    }

    const f32 figureVelocity = res.figureVelocity() * set.figureVelocity;
    const f32 scaleX = res.volumeRadius().x * set.emitterScale.x;
    const f32 scaleY = res.volumeRadius().y * set.emitterScale.y;
    const f32 scaleZ = res.volumeRadius().z * set.emitterScale.z;

    const u32 arcStart = static_cast<u32>(res.volumeArcStart());
    const u32 arcWidth = static_cast<u32>(res.volumeArcWidth());

    PtclInstance* lastPtcl = nullptr;

    for (s32 idx = 0; idx < emitRate; ++idx) {
        auto* ptcl = EmitterCalc::mSys->allocPtcl();
        if (ptcl == nullptr) { continue; }

        ptcl->res = &res;
        ptcl->stripe = nullptr;

        const u32 angleAIdx = (emitter.mRandom.getU32() % arcStart) + arcWidth;
        const u32 angleBIdx = emitter.mRandom.getAngle();

        f32 sinA, cosA, sinB, cosB;
        Math::sinCosIdx(&sinA, &cosA, angleAIdx);
        Math::sinCosIdx(&sinB, &cosB, angleBIdx);

        ptcl->position.x =  sinA * cosB * scaleX;
        ptcl->position.y = -sinB        * scaleY;
        ptcl->position.z =  cosA * cosB * scaleZ;

        ptcl->velocity.x =  sinA * cosB * figureVelocity;
        ptcl->velocity.y = -sinB        * figureVelocity;
        ptcl->velocity.z =  cosA * cosB * figureVelocity;

        emitCommon(emitter, *ptcl);
        lastPtcl = ptcl;
    }

    return lastPtcl;
}

PtclInstance* EmitterCalc::emitSphereSameDivide(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const s32 emitRate = emitCount(res);

    if (emitRate <= 0) {
        return nullptr;
    }

    const f32 figureVelocity = res.figureVelocity() * set.figureVelocity;
    const f32 scaleX = res.volumeRadius().x * set.emitterScale.x;
    const f32 scaleY = res.volumeRadius().y * set.emitterScale.y;
    const f32 scaleZ = res.volumeRadius().z * set.emitterScale.z;

    const Math::Vector3f* direction = sSameDivideSphereTable[res.volumeTblIndex()];
    PtclInstance* lastPtcl = nullptr;

    for (s32 idx = 0; idx < emitRate; ++idx) {
        auto* ptcl = EmitterCalc::mSys->allocPtcl();
        if (ptcl == nullptr) { continue; }

        ptcl->res = &res;
        ptcl->stripe = nullptr;

        ptcl->position.x = direction->x * scaleX;
        ptcl->position.y = direction->y * scaleY;
        ptcl->position.z = direction->z * scaleZ;

        ptcl->velocity.x = direction->x * figureVelocity;
        ptcl->velocity.y = direction->y * figureVelocity;
        ptcl->velocity.z = direction->z * figureVelocity;

        ++direction;

        emitCommon(emitter, *ptcl);
        lastPtcl = ptcl;
    }

    return lastPtcl;
}

PtclInstance* EmitterCalc::emitFillSphere(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const s32 emitRate = emitCount(res);

    if (emitRate <= 0) {
        return nullptr;
    }

    const f32 figureVelocity = res.figureVelocity() * set.figureVelocity;
    const f32 scaleX = res.volumeRadius().x * set.emitterScale.x;
    const f32 scaleY = res.volumeRadius().y * set.emitterScale.y;
    const f32 scaleZ = res.volumeRadius().z * set.emitterScale.z;

    const u32 arcWidth = static_cast<u32>(res.volumeArcWidth());
    const u32 arcStart = static_cast<u32>(res.volumeArcStart());

    PtclInstance* lastPtcl = nullptr;

    for (s32 idx = 0; idx < emitRate; ++idx) {
        auto* ptcl = EmitterCalc::mSys->allocPtcl();
        if (ptcl == nullptr) { continue; }

        ptcl->res = &res;
        ptcl->stripe = nullptr;

        const u32 angleAIdx = (emitter.mRandom.getU32() % arcStart) + arcWidth;
        const u32 angleBIdx = emitter.mRandom.getAngle();
        const f32 rad = emitter.mRandom.getF32();

        f32 sinA, cosA, sinB, cosB;
        Math::sinCosIdx(&sinA, &cosA, angleAIdx);
        Math::sinCosIdx(&sinB, &cosB, angleBIdx);

        ptcl->position.x =  sinA * cosB * rad * scaleX;
        ptcl->position.y = -sinB *        rad * scaleY;
        ptcl->position.z =  cosA * cosB * rad * scaleZ;

        ptcl->velocity.x =  sinA * cosB * figureVelocity;
        ptcl->velocity.y = -sinB *        figureVelocity;
        ptcl->velocity.z =  cosA * cosB * figureVelocity;

        emitCommon(emitter, *ptcl);
        lastPtcl = ptcl;
    }

    return lastPtcl;
}

PtclInstance* EmitterCalc::emitCylinder(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const s32 emitRate = emitCount(res);

    if (emitRate <= 0) {
        return nullptr;
    }

    const f32 figureVelocity = res.figureVelocity() * set.figureVelocity;
    const f32 scaleX = res.volumeRadius().x * set.emitterScale.x;
    const f32 scaleY = res.volumeRadius().y * set.emitterScale.y;
    const f32 scaleZ = res.volumeRadius().z * set.emitterScale.z;

    const u32 arcWidth = static_cast<u32>(res.volumeArcWidth());
    const u32 arcStart = static_cast<u32>(res.volumeArcStart());

    PtclInstance* lastPtcl = nullptr;

    for (s32 idx = 0; idx < emitRate; ++idx) {
        auto* ptcl = EmitterCalc::mSys->allocPtcl();
        if (ptcl == nullptr) { continue; }

        ptcl->res = &res;
        ptcl->stripe = nullptr;

        f32 sinV, cosV;
        Math::sinCosIdx(&sinV, &cosV, (emitter.mRandom.getU32() % arcStart) + arcWidth);
        const auto randVec3 = emitter.mRandom.getVec3();

        ptcl->position.x = sinV       * scaleX;
        ptcl->position.y = randVec3.y * scaleY;
        ptcl->position.z = cosV       * scaleZ;

        ptcl->velocity.x = sinV * figureVelocity;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = cosV * figureVelocity;

        emitCommon(emitter, *ptcl);
        lastPtcl = ptcl;
    }

    return lastPtcl;
}

PtclInstance* EmitterCalc::emitFillCylinder(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const s32 emitRate = emitCount(res);

    if (emitRate <= 0) {
        return nullptr;
    }

    const f32 figureVelocity = res.figureVelocity() * set.figureVelocity;
    const f32 scaleX = res.volumeRadius().x * set.emitterScale.x;
    const f32 scaleY = res.volumeRadius().y * set.emitterScale.y;
    const f32 scaleZ = res.volumeRadius().z * set.emitterScale.z;

    const u32 arcWidth = static_cast<u32>(res.volumeArcWidth());
    const u32 arcStart = static_cast<u32>(res.volumeArcStart());

    PtclInstance* lastPtcl = nullptr;

    for (s32 idx = 0; idx < emitRate; ++idx) {
        auto* ptcl = EmitterCalc::mSys->allocPtcl();
        if (ptcl == nullptr) { continue; }

        ptcl->res = &res;
        ptcl->stripe = nullptr;

        f32 sinV, cosV;
        Math::sinCosIdx(&sinV, &cosV, (emitter.mRandom.getU32() % arcStart) + arcWidth);

        const f32 radius = emitter.mRandom.getF32();
        const auto randVec3 = emitter.mRandom.getVec3();

        ptcl->position.x = (radius * sinV) * scaleX;
        ptcl->position.y = randVec3.y      * scaleY;
        ptcl->position.z = (radius * cosV) * scaleZ;

        ptcl->velocity.x = sinV * figureVelocity;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = cosV * figureVelocity;

        emitCommon(emitter, *ptcl);
        lastPtcl = ptcl;
    }

    return lastPtcl;
}

PtclInstance* EmitterCalc::emitBox(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const s32 emitRate = emitCount(res);

    if (emitRate <= 0) {
        return nullptr;
    }

    const f32 figureVelocity = res.figureVelocity() * set.figureVelocity;
    const f32 scaleX = res.volumeRadius().x * set.emitterScale.x;
    const f32 scaleY = res.volumeRadius().y * set.emitterScale.y;
    const f32 scaleZ = res.volumeRadius().z * set.emitterScale.z;

    PtclInstance* last = nullptr;

    for (s32 idx = 0; idx < emitRate; ++idx) {
        auto* ptcl = EmitterCalc::mSys->allocPtcl();
        if (ptcl == nullptr) { continue; }

        ptcl->res = &res;
        ptcl->stripe = nullptr;

        const u32 coord = emitter.mRandom.getU32();
        const u32 sign = emitter.mRandom.getU32();

        const auto& randVec3 = emitter.mRandom.getVec3();

        static constexpr u32 cBoxCoordX = 0xffffffff / 3 * 1;
        static constexpr u32 cBoxCoordY = 0xffffffff / 3 * 2;

        if (coord < cBoxCoordX) {
            ptcl->position.x = randVec3.x * scaleX;
            ptcl->position.y = randVec3.y * scaleY;
            ptcl->position.z = (sign >= cHalfU32) ? -scaleZ : scaleZ;
        } else if (coord < cBoxCoordY) {
            ptcl->position.x = randVec3.x * scaleX;
            ptcl->position.y = (sign >= cHalfU32) ? -scaleY : scaleY;
            ptcl->position.z = randVec3.z * scaleZ;
        } else {
            ptcl->position.x = (sign >= cHalfU32) ? -scaleX : scaleX;
            ptcl->position.y = randVec3.y * scaleY;
            ptcl->position.z = randVec3.z * scaleZ;
        }

        const Math::Vector3f direction = ptcl->position.normalized();
        ptcl->velocity.x = direction.x * figureVelocity;
        ptcl->velocity.y = direction.y * figureVelocity;
        ptcl->velocity.z = direction.z * figureVelocity;

        emitCommon(emitter, *ptcl);
        last = ptcl;
    }

    return last;
}

PtclInstance* EmitterCalc::emitFillBox(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const s32 emitRate = emitCount(res);

    if (emitRate <= 0) {
        return nullptr;
    }

    const f32 figureVelocity = res.figureVelocity() * set.figureVelocity;
    const f32 scaleX = res.volumeRadius().x * set.emitterScale.x;
    const f32 scaleY = res.volumeRadius().y * set.emitterScale.y;
    const f32 scaleZ = res.volumeRadius().z * set.emitterScale.z;

    PtclInstance* lastPtcl = nullptr;

    for (s32 idx = 0; idx < emitRate; ++idx) {
        auto* ptcl = EmitterCalc::mSys->allocPtcl();
        if (ptcl == nullptr) { continue; }

        ptcl->res = &res;
        ptcl->stripe = nullptr;

        f32 px = scaleX * (emitter.mRandom.getF32() * 2.0f - 1.0f);
        f32 py = scaleY * (emitter.mRandom.getF32() * 2.0f - 1.0f);
        f32 pz = scaleZ * (emitter.mRandom.getF32() * 2.0f - 1.0f);

        ptcl->position.x = px;
        ptcl->position.y = py;
        ptcl->position.z = pz;

        const Math::Vector3f direction = ptcl->position.normalized();
        ptcl->velocity.x = direction.x * figureVelocity;
        ptcl->velocity.y = direction.y * figureVelocity;
        ptcl->velocity.z = direction.z * figureVelocity;

        emitCommon(emitter, *ptcl);
        lastPtcl = ptcl;
    }

    return lastPtcl;
}

PtclInstance* EmitterCalc::emitLine(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const s32 emitRate = emitCount(res);

    if (emitRate <= 0) {
        return nullptr;
    }

    const f32 figureVel = res.figureVelocity() * set.figureVelocity;
    const f32 scaleZ = res.volumeRadius().z * set.emitterScale.z;

    PtclInstance* last = nullptr;

    for (s32 idx = 0; idx < emitRate; ++idx) {
        auto* ptcl = EmitterCalc::mSys->allocPtcl();
        if (ptcl == nullptr) { continue; }

        ptcl->res = &res;
        ptcl->stripe = nullptr;

        ptcl->position.x = 0.0f;
        ptcl->position.y = 0.0f;
        ptcl->position.z = emitter.mRandom.getF32() * scaleZ;

        ptcl->velocity.x = 0.0f;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = figureVel;

        emitCommon(emitter, *ptcl);
        last = ptcl;
    }

    return last;
}

PtclInstance* EmitterCalc::emitLineSameDivide(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const s32 emitRate = emitCount(res);

    if (emitRate <= 0) {
        return nullptr;
    }

    const f32 figureVel = res.figureVelocity() * set.figureVelocity;
    const f32 scaleZ = res.volumeRadius().z * set.emitterScale.z;

    f32 t, step;
    if (emitRate == 1) {
        t    = 0.5f;
        step = 0.0f;
    } else {
        t    = 0.0f;
        step = 1.0f / static_cast<f32>(emitRate - 1);
    }

    PtclInstance* lastPtcl = nullptr;

    for (s32 idx = 0; idx < emitRate; ++idx) {
        auto* ptcl = EmitterCalc::mSys->allocPtcl();
        if (ptcl == nullptr) { continue; }

        ptcl->res = &res;
        ptcl->stripe = nullptr;

        ptcl->position.x = 0.0f;
        ptcl->position.y = 0.0f;
        ptcl->position.z = t * scaleZ;

        t += step;

        ptcl->velocity.x = 0.0f;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = figureVel;

        emitCommon(emitter, *ptcl);
        lastPtcl = ptcl;
    }

    return lastPtcl;
}

PtclInstance* EmitterCalc::emitRectangle(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const s32 emitRate = emitCount(res);

    if (emitRate <= 0) {
        return nullptr;
    }

    const f32 figureVelocity = res.figureVelocity() * set.figureVelocity;
    const f32 scaleX = res.volumeRadius().x * set.emitterScale.x;
    const f32 scaleZ = res.volumeRadius().z * set.emitterScale.z;

    PtclInstance* lastPtcl = nullptr;

    for (s32 idx = 0; idx < emitRate; ++idx) {
        auto* ptcl = EmitterCalc::mSys->allocPtcl();
        if (ptcl == nullptr) { continue; }

        ptcl->res = &res;
        ptcl->stripe = nullptr;

        const u32 coord = emitter.mRandom.getU32();
        const u32 sign = emitter.mRandom.getU32();
        const auto& randVec3 = emitter.mRandom.getVec3();

        if (coord >= cHalfU32) {
            ptcl->position.x = (sign >= cHalfU32) ? -scaleX : scaleX;
            ptcl->position.y = 0.0f;
            ptcl->position.z = randVec3.z * scaleZ;
        } else {
            ptcl->position.x = randVec3.x * scaleX;
            ptcl->position.y = 0.0f;
            ptcl->position.z = (sign >= cHalfU32) ? -scaleZ : scaleZ;
        }

        Math::Vector3f direction = ptcl->position.normalized();
        ptcl->velocity.x = direction.x * figureVelocity;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = direction.z * figureVelocity;

        emitCommon(emitter, *ptcl);
        lastPtcl = ptcl;
    }

    return lastPtcl;
}


// ========================================================================== //


s32 EmitterCalc::calc(EmitterInstance& emitter, PtclRenderer& renderer) {
    const std::size_t typeIdx = static_cast<std::size_t>(emitter.mResource.type());

    if (typeIdx >= sCalcFunctions.size() || sCalcFunctions[typeIdx] == nullptr) {
        std::fprintf(stderr, "EmitterCalc: emitter type %d is not ported yet\n", static_cast<int>(emitter.mResource.type()));
        return 0;
    }

    return sCalcFunctions[typeIdx](emitter, renderer);
}


// ========================================================================== //


} // namespace Ptcl::Engine
