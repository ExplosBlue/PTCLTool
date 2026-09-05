#include "ptcl/engine/emitterCalc.h"

#include "ptcl/engine/emitterInstance.h"


namespace Ptcl::Engine {


// ========================================================================== //


constexpr s32 sInfiniteLifetime = 0x7FFFFFFF;


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
    ptcl.random = emitter.mRandom.getU32Direct();

    if (res.ptclLife() == sInfiniteLifetime) {
        ptcl.life = sInfiniteLifetime;
    } else {
        const s32 ptclLifeRnd = emitter.mRandom.getS32(res.ptclLifeRandom());
        const s64 life = static_cast<s64>(res.ptclLife()) - ptclLifeRnd;

        // The 256 here is originally `(life * *mController->_8) >> 8` in engine.
        // However, this value is always fixed at 256 (in NSMB2).
        ptcl.life = static_cast<s32>((life * 256) >> 8);
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
        ptcl.alphaSection1 = (res.alphaAnim().alphaSection1 * maxTime) / 10;
        ptcl.alphaSection2 = (res.alphaAnim().alphaSection2 * maxTime) / 10;

        ptcl.alphaAddSection1 = res.alphaAnim().diffAlpha21 / static_cast<f32>(ptcl.alphaSection1);
        ptcl.alphaAddSection2 = res.alphaAnim().diffAlpha32 / static_cast<f32>(maxTime - ptcl.alphaSection2);

        ptcl.alpha = res.alphaAnim().initAlpha - ptcl.alphaAddSection1;

        // Scale
        ptcl.scaleSection1 = (res.scaleAnim().scaleSection1 * maxTime) / 10;
        ptcl.scaleSection2 = (res.scaleAnim().scaleSection2 * maxTime) / 10;

        const f32 invScaleSec1 = 1.0f / static_cast<f32>(ptcl.scaleSection1);
        const f32 invScaleSec2 = 1.0f / static_cast<f32>(maxTime - ptcl.scaleSection2);

        const f32 scaleAddSec1X = res.scaleAnim().diffScale21.x * invScaleSec1;
        const f32 scaleAddSec1Y = res.scaleAnim().diffScale21.y * invScaleSec1;

        const f32 scaleAddSec2X = res.scaleAnim().diffScale32.x * invScaleSec2;
        const f32 scaleAddSec2Y = res.scaleAnim().diffScale32.y * invScaleSec2;

        const f32 scaleX = res.scaleAnim().initScale.x - scaleAddSec1X;
        const f32 scaleY = res.scaleAnim().initScale.y - scaleAddSec1Y;

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
        multiply(ptcl.emitterRT, set.RT, res.transformRT());
        multiply(ptcl.emitterSRT, set.SRT, res.transformSRT());
    }

    if (res.numTexturePattern() <= 1) {
        ptcl.uvOffset.x = 0.0f;
        ptcl.uvOffset.y = 0.0f;
        ptcl.uvSubOffset = res.textureUVScale();
    } else {
        const s32 num = emitter.mRandom.getS32(res.numTexturePattern());
        const s32 numX = num % res.numTextureDivisionX();
        const s32 numY = num / res.numTextureDivisionX();

        const f32 texVScale = res.textureUVScale().x;
        const f32 texUScale = res.textureUVScale().y;

        ptcl.uvSubOffset.x = texUScale;
        ptcl.uvSubOffset.y = texVScale;

        ptcl.uvOffset.x = texUScale * static_cast<f32>(numX);
        ptcl.uvOffset.y = texVScale * static_cast<f32>(numY);
    }

    addParticle(emitter, ptcl);
}


void EmitterCalc::addParticle(EmitterInstance& emitter, PtclInstance& ptcl) {
    auto* head = emitter.mPtclHead;

    if (head != nullptr) {
        head->prev = &ptcl;
        emitter.mPtclHead = &ptcl;
        ptcl.next = head;
    } else {
        emitter.mPtclHead = &ptcl;
        ptcl.next = nullptr;
    }
    ptcl.prev = nullptr;
}


// ========================================================================== //


} // namespace Ptc::Engine
