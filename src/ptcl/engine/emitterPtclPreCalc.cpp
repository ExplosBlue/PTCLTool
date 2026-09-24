#include "ptcl/engine/emitterCalc.h"
#include "ptcl/engine/emitterInstance.h"
#include "math/vector.h"
#include "ptcl/engine/ptclRenderer.h"


namespace Ptcl::Engine {


// ========================================================================== //


namespace /* Anonymous */ {


f32 adjustAlpha(const PtclInstance& ptcl) {
    f32 alpha = ptcl.alpha;
    const s32 cntS = ptcl.tickCount;
    if (cntS <= ptcl.alphaSection1) {
        alpha = ptcl.alphaAddSection1 + alpha;
    } else if (cntS > ptcl.alphaSection2) {
        alpha = ptcl.alphaAddSection2 + alpha;
    }
    return alpha;
}

Math::Vector2f adjustScale(const PtclInstance& ptcl) {
    Math::Vector2f scale = ptcl.scale;
    const s32 scaleIx = ptcl.tickCount;
    if (scaleIx > ptcl.scaleSection1) {
        if (scaleIx > ptcl.scaleSection2) {
            scale.x = ptcl.scaleAddSection2.x + scale.x;
            scale.y = ptcl.scaleAddSection2.y + scale.y;
        }
    } else {
        scale.x = ptcl.scaleAddSection1.x + scale.x;
        scale.y = ptcl.scaleAddSection1.y + scale.y;
    }
    return scale;
}

Math::Vector3f calcGravityVelocity(const Math::Vector3f& airResist, const Ptcl::Emitter& res, const Math::Matrix34f& rt) {
    const Math::Vector3f& gravity = res.gravity();

    if (res.isDirectional()) {
        return {
            airResist.x + ((gravity.x * rt(0, 0) + gravity.y * rt(1, 0)) + gravity.z * rt(2, 0)),
            airResist.y + ((gravity.x * rt(0, 1) + gravity.y * rt(1, 1)) + gravity.z * rt(2, 1)),
            airResist.z + ((gravity.x * rt(0, 2) + gravity.y * rt(1, 2)) + gravity.z * rt(2, 2))
        };
    }
    return {
        gravity.x + airResist.x,
        gravity.y + airResist.y,
        gravity.z + airResist.z
    };
}


} // namespace /* Anonymous */


// ========================================================================== //


void EmitterCalc::calcFields(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;

    if (res.isFieldRandomEnabled()) {
        calcFieldRandom(emitter, ptcl);
    }

    if (res.isFieldMagnetEnabled()) {
        calcFieldMagnet(emitter, ptcl);
    }

    if (res.isFieldSpinEnabled()) {
        calcFieldSpin(emitter, ptcl);
    }

    if (res.isFieldCollisionEnabled()) {
        calcFieldCollision(emitter, ptcl);
    }

    if (res.isFieldConvergenceEnabled()) {
        calcFieldConvergence(emitter, ptcl);
    }

    if (res.isFieldPosAddEnabled()) {
        calcFieldPosAdd(emitter, ptcl);
    }
}


// ========================================================================== //


void EmitterCalc::calcTexPatAnim(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;

    const s32 patFreq = res.texturePatternFrequency();
    const s32 frameCount = res.texturePatternFrameCount();

    s32 patIdx;
    if (patFreq == 0) {
        patIdx = (ptcl.tickCount * frameCount) / ptcl.life;
    } else {
        patIdx = ptcl.tickCount / patFreq;
    }

    const s32 pat = res.texturePatternTable()[patIdx % frameCount];

    const s32 numX = pat % res.numTextureDivisionX();
    const s32 numY = pat / res.numTextureDivisionX();

    ptcl.uvOffset.x = res.textureUVScale().x * static_cast<f32>(numX);
    ptcl.uvOffset.y = res.textureUVScale().y * static_cast<f32>(numY);
}


// ========================================================================== //


ParticleState EmitterCalc::ptclPreCalcFollowAllSimple(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;

    ptcl.position.x += ptcl.velocity.x;
    ptcl.position.y += ptcl.velocity.y;
    ptcl.position.z += ptcl.velocity.z;

    const Math::Vector3f airResist {
        res.airResistance() * ptcl.velocity.x,
        res.airResistance() * ptcl.velocity.y,
        res.airResistance() * ptcl.velocity.z
    };

    ptcl.alpha = adjustAlpha(ptcl);
    ptcl.scale = adjustScale(ptcl);
    ptcl.velocity = calcGravityVelocity(airResist, res, emitter.mEmitterRT);

    if (res.isTexturePatternAnim()) {
        calcTexPatAnim(emitter, ptcl);
    }

    ptcl.positionDiff = Math::VectorCalc::multiply(emitter.mEmitterSRT, ptcl.position);

    ptcl.tickCount++;

    ParticleState state;
    if (res.isVelLook()) {
        state.followMode = ParticleFollowMode::Direction;
        state.followDirection = Math::VectorCalc::rotate(emitter.mEmitterSRT, ptcl.velocity);
    }

    return state;
}

ParticleState EmitterCalc::ptclPreCalcFollowNoneSimple(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;

    ptcl.position.x += ptcl.velocity.x;
    ptcl.position.y += ptcl.velocity.y;
    ptcl.position.z += ptcl.velocity.z;

    const Math::Vector3f airResist {
        res.airResistance() * ptcl.velocity.x,
        res.airResistance() * ptcl.velocity.y,
        res.airResistance() * ptcl.velocity.z
    };

    ptcl.alpha = adjustAlpha(ptcl);
    ptcl.scale = adjustScale(ptcl);
    ptcl.velocity = calcGravityVelocity(airResist, res, ptcl.emitterRT);

    if (res.isTexturePatternAnim()) {
        calcTexPatAnim(emitter, ptcl);
    }

    ptcl.positionDiff = Math::VectorCalc::multiply(ptcl.emitterSRT, ptcl.position);

    ptcl.tickCount++;

    ParticleState state;
    if (res.isPolygon()) {
        state.followMode = ParticleFollowMode::Basis;
        state.followBasis = ptcl.emitterRT;
    } else if (res.isVelLook()) {
        state.followMode = ParticleFollowMode::Direction;
        state.followDirection = Math::VectorCalc::rotate(ptcl.emitterSRT, ptcl.velocity);
    }

    return state;
}

ParticleState EmitterCalc::ptclPreCalcFollowPosOnlySimple(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;

    ptcl.position.x += ptcl.velocity.x;
    ptcl.position.y += ptcl.velocity.y;
    ptcl.position.z += ptcl.velocity.z;

    Math::Vector3f airResist {
        res.airResistance() * ptcl.velocity.x,
        res.airResistance() * ptcl.velocity.y,
        res.airResistance() * ptcl.velocity.z
    };

    ptcl.alpha = adjustAlpha(ptcl);
    ptcl.scale = adjustScale(ptcl);

    ptcl.emitterSRT(0, 3) = emitter.mEmitterSRT(0, 3);
    ptcl.emitterSRT(1, 3) = emitter.mEmitterSRT(1, 3);
    ptcl.emitterSRT(2, 3) = emitter.mEmitterSRT(2, 3);

    ptcl.emitterRT(0, 3) = emitter.mEmitterRT(0, 3);
    ptcl.emitterRT(1, 3) = emitter.mEmitterRT(1, 3);
    ptcl.emitterRT(2, 3) = emitter.mEmitterRT(2, 3);

    ptcl.velocity = calcGravityVelocity(airResist, res, ptcl.emitterRT);

    if (res.isTexturePatternAnim()) {
        calcTexPatAnim(emitter, ptcl);
    }

    ptcl.positionDiff = Math::VectorCalc::multiply(ptcl.emitterSRT, ptcl.position);

    ptcl.tickCount++;

    ParticleState state;
    if (res.isPolygon()) {
        state.followMode = ParticleFollowMode::Basis;
        state.followBasis = ptcl.emitterRT;
    } else if (res.isVelLook()) {
        state.followMode = ParticleFollowMode::Direction;
        state.followDirection = Math::VectorCalc::rotate(ptcl.emitterSRT, ptcl.velocity);
    }

    return state;
}


// ========================================================================== //


ParticleState EmitterCalc::ptclPreCalcFollowAllComplex(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;

    const auto prevPos = ptcl.position;
    ptcl.position.x += ptcl.velocity.x;
    ptcl.position.y += ptcl.velocity.y;
    ptcl.position.z += ptcl.velocity.z;

    Math::Vector3f airResist {
        res.airResistance() * ptcl.velocity.x,
        res.airResistance() * ptcl.velocity.y,
        res.airResistance() * ptcl.velocity.z
    };

    ptcl.alpha = adjustAlpha(ptcl);
    ptcl.scale = adjustScale(ptcl);
    ptcl.velocity = calcGravityVelocity(airResist, res, emitter.mEmitterRT);

    if (res.isTexturePatternAnim()) {
        calcTexPatAnim(emitter, ptcl);
    }

    if (res.isFluctuationEnabled()) {
        calcFluctuation(emitter, ptcl);
    }

    calcFields(emitter, ptcl);

    ptcl.positionDiff = Math::VectorCalc::multiply(emitter.mEmitterSRT, ptcl.position);

    ptcl.tickCount++;

    ptcl.posDiff.x = ptcl.position.x - prevPos.x;
    ptcl.posDiff.y = ptcl.position.y - prevPos.y;
    ptcl.posDiff.z = ptcl.position.z - prevPos.z;

    ParticleState state;
    if (res.isVelLook()) {
        state.followMode = ParticleFollowMode::Direction;
        state.followDirection = Math::VectorCalc::rotate(emitter.mEmitterSRT, ptcl.posDiff);
    }

    return state;
}

ParticleState EmitterCalc::ptclPreCalcFollowNoneComplex(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;

    const auto prevPos = ptcl.position;
    ptcl.position.x += ptcl.velocity.x;
    ptcl.position.y += ptcl.velocity.y;
    ptcl.position.z += ptcl.velocity.z;

    Math::Vector3f airResist {
        res.airResistance() * ptcl.velocity.x,
        res.airResistance() * ptcl.velocity.y,
        res.airResistance() * ptcl.velocity.z
    };

    ptcl.alpha = adjustAlpha(ptcl);
    ptcl.scale = adjustScale(ptcl);
    ptcl.velocity = calcGravityVelocity(airResist, res, ptcl.emitterRT);

    if (res.isTexturePatternAnim()) {
        calcTexPatAnim(emitter, ptcl);
    }

    if (res.isFluctuationEnabled()) {
        calcFluctuation(emitter, ptcl);
    }

    calcFields(emitter, ptcl);

    ptcl.positionDiff = Math::VectorCalc::multiply(ptcl.emitterSRT, ptcl.position);

    ptcl.tickCount++;

    ptcl.posDiff.x = ptcl.position.x - prevPos.x;
    ptcl.posDiff.y = ptcl.position.y - prevPos.y;
    ptcl.posDiff.z = ptcl.position.z - prevPos.z;

    ParticleState state;
    if (res.isPolygon()) {
        state.followMode = ParticleFollowMode::Basis;
        state.followBasis = ptcl.emitterRT;
    } else if (res.isVelLook()) {
        state.followMode = ParticleFollowMode::Direction;
        state.followDirection = Math::VectorCalc::rotate(ptcl.emitterSRT, ptcl.posDiff);
    }

    return state;
}


ParticleState EmitterCalc::ptclPreCalcFollowPosOnlyComplex(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;

    const auto prevPos = ptcl.position;
    ptcl.position.x += ptcl.velocity.x;
    ptcl.position.y += ptcl.velocity.y;
    ptcl.position.z += ptcl.velocity.z;

    Math::Vector3f airResist {
        res.airResistance() * ptcl.velocity.x,
        res.airResistance() * ptcl.velocity.y,
        res.airResistance() * ptcl.velocity.z
    };

    ptcl.alpha = adjustAlpha(ptcl);
    ptcl.scale = adjustScale(ptcl);

    ptcl.emitterSRT(0, 3) = emitter.mEmitterSRT(0, 3);
    ptcl.emitterSRT(1, 3) = emitter.mEmitterSRT(1, 3);
    ptcl.emitterSRT(2, 3) = emitter.mEmitterSRT(2, 3);

    ptcl.emitterRT(0, 3) = emitter.mEmitterRT(0, 3);
    ptcl.emitterRT(1, 3) = emitter.mEmitterRT(1, 3);
    ptcl.emitterRT(2, 3) = emitter.mEmitterRT(2, 3);

    ptcl.velocity = calcGravityVelocity(airResist, res, ptcl.emitterRT);

    if (res.isTexturePatternAnim()) {
        calcTexPatAnim(emitter, ptcl);
    }

    if (res.isFluctuationEnabled()) {
        calcFluctuation(emitter, ptcl);
    }

    calcFields(emitter, ptcl);

    ptcl.positionDiff = Math::VectorCalc::multiply(ptcl.emitterSRT, ptcl.position);

    ptcl.tickCount++;

    ptcl.posDiff.x = ptcl.position.x - prevPos.x;
    ptcl.posDiff.y = ptcl.position.y - prevPos.y;
    ptcl.posDiff.z = ptcl.position.z - prevPos.z;

    ParticleState state;
    if (res.isPolygon()) {
        state.followMode = ParticleFollowMode::Basis;
        state.followBasis = ptcl.emitterRT;
    } else if (res.isVelLook()) {
        state.followMode = ParticleFollowMode::Direction;
        state.followDirection = Math::VectorCalc::rotate(ptcl.emitterSRT, ptcl.posDiff);
    }

    return state;
}


// ========================================================================== //


} // namespace Ptcl::Engine
