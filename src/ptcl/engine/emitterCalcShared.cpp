#include "ptcl/engine/emitterCalc.h"

#include <array>
#include <cstddef>

#include "gfx/color.h"
#include "math/matrix.h"
#include "math/vector.h"
#include "ptcl/engine/emitterInstance.h"
#include "ptcl/engine/emitterSetInstance.h"
#include "ptcl/engine/ptclInstance.h"
#include "ptcl/engine/ptclRenderer.h"
#include "ptcl/engine/ptclSystem.h"


namespace Ptcl::Engine {


// ========================================================================== //


std::size_t ptclRotCombinerIndex(const Ptcl::Emitter& res) {
    return static_cast<std::size_t>(res.colorCalcType()) * 5 + static_cast<std::size_t>(res.rotationType());
}

bool EmitterCalc::stepFadeAlpha(EmitterInstance& emitter, bool& canEmit) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    if (set.mCalcAlphaAnim) {
        emitter.mFadeAlpha -= res.alphaAddInFade();
        canEmit = !res.isStopEmitInFade();

        if (emitter.mFadeAlpha <= 0.0f) {
            mSys->killEmitter(emitter);
            return false;
        }
    } else {
        emitter.mFadeAlpha += res.alphaAddInFade();
        canEmit = true;

        if (emitter.mFadeAlpha > 1.0f) {
            emitter.mFadeAlpha = 1.0f;
        }
    }

    return true;
}

void EmitterCalc::scheduleEmission(EmitterInstance& emitter, s32 frame, bool canEmit) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    if (!canEmit || res.emitEndFrame() <= frame) {
        return;
    }

    if (res.emitStartFrame() > frame) {
        return;
    }

    const s32 scaledInterval = static_cast<s32>((static_cast<s64>(emitter.mEmissionInterval) * emitter.mController.emitRatePerFrame) >> 8);
    if (scaledInterval > emitter.mLoopInterval) {
        emitter.mLoopInterval++;
        return;
    }

    const s32 numPoint = set.mNumParticleEmissionPoint;
    if (numPoint <= 0) {
        sEmitFunctions[static_cast<std::size_t>(res.volumeType())](emitter);
    } else {
        for (s32 point = 0; point < numPoint; ++point) {
            auto* prevHead = emitter.mParticles.front();
            sEmitFunctions[static_cast<std::size_t>(res.volumeType())](emitter);
            for (auto it = emitter.mParticles.begin(); it != emitter.mParticles.end(); ++it) {
                PtclInstance* ptcl = *it;
                if (ptcl == prevHead) {
                    break;
                }
                ptcl->position.x += set.mParticleEmissionPoints[point].x;
                ptcl->position.y += set.mParticleEmissionPoints[point].y;
                ptcl->position.z += set.mParticleEmissionPoints[point].z;
            }
        }
    }

    emitter.mLoopInterval = 0;
    emitter.mEmissionInterval = res.lifeStep()
        - emitter.mRandom.getS32(res.lifeStepRandom());
}

void EmitterCalc::beginEmitterMatrices(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    emitter.mEmitterRT = Math::MatrixCalc::multiply(set.RT, res.transformRT());
    emitter.mEmitterSRT = Math::MatrixCalc::multiply(set.SRT, res.transformSRT());
}

EmitterState EmitterCalc::makeParentState(const EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    return {
        .textureHandle         = &res.textureHandle(),
        .billboardType         = res.billboardType(),
        .isEmitterBillboardMtx = res.isEmitterBillboardMtx(),
        .blendFunction         = res.blendFunction(),
        .depthFunction         = res.depthFunction(),
        .combinerFunction      = res.combinerFunction(),
        .emitterType           = res.type(),
        .rotationBasis         = res.rotationBasis(),
        .color = {
            res.primaryColor().r() * set.color.r(),
            res.primaryColor().g() * set.color.g(),
            res.primaryColor().b() * set.color.b(),
            1.0f,
        },
        .secondaryColor = {
            res.secondaryColor().r() * set.color.r(),
            res.secondaryColor().g() * set.color.g(),
            res.secondaryColor().b() * set.color.b(),
            1.0f,
        },
        .textureWrapS     = res.textureWrapS(),
        .textureWrapT     = res.textureWrapT(),
        .textureFilter    = res.textureFilter(),
        .emitterRT = emitter.mEmitterRT,
    };
}

EmitterState EmitterCalc::emitParentState(PtclRenderer& renderer, const EmitterInstance& emitter) {
    const EmitterState state = makeParentState(emitter);
    renderer.setEmitterState(state);
    return state;
}

void EmitterCalc::emitChildState(PtclRenderer& renderer, const EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    renderer.setEmitterState({
        .textureHandle         = &res.childTextureHandle(),
        .billboardType         = res.childBillboardType(),
        .isEmitterBillboardMtx = res.childFlags().isSet(ChildFlag::IsEmitterBillboardMtx),
        .blendFunction         = res.childBlendFunc(),
        .depthFunction         = res.childDepthFunc(),
        .combinerFunction      = res.childCombinerFunc(),
        .emitterType           = res.type(),
        .rotationBasis         = res.childRotationBasis(),
        .color = {
            res.childPrimaryColor().r() * set.color.r(),
            res.childPrimaryColor().g() * set.color.g(),
            res.childPrimaryColor().b() * set.color.b(),
            1.0f,
        },
        .secondaryColor = {
            res.childSecondaryColor().r() * set.color.r(),
            res.childSecondaryColor().g() * set.color.g(),
            res.childSecondaryColor().b() * set.color.b(),
            1.0f,
        },
        .textureWrapS     = res.childTextureWrapS(),
        .textureWrapT     = res.childTextureWrapT(),
        .textureFilter    = res.childTextureFilter(),
        .emitterRT = emitter.mEmitterRT,
    });
}

void EmitterCalc::populateParticleState(const EmitterSetInstance& set,
    const EmitterInstance& emitter, ParticleState& draw, const PtclInstance& ptcl, f32 fluctuationAlpha,
    f32 fluctuationScale) {
    draw.positionDiff = ptcl.positionDiff;
    draw.scale = {
        (ptcl.scale.x * set.mParticleScaleForCalc.x) * fluctuationScale,
        (ptcl.scale.y * set.mParticleScaleForCalc.y) * fluctuationScale,
    };
    draw.alpha = (ptcl.alpha * set.color.a()) * (fluctuationAlpha * emitter.mFadeAlpha);
    draw.uvOffset = ptcl.uvOffset;
    draw.uvSubOffset = ptcl.uvSubOffset;
    draw.rotation = ptcl.rotation.z;
    draw.color = {draw.matrix(0, 0), draw.matrix(1, 0), draw.matrix(2, 0), 1.0f};
}

void EmitterCalc::emitParticleState(PtclRenderer& renderer, const EmitterSetInstance& set,
    const EmitterInstance& emitter, ParticleState& draw, const PtclInstance& ptcl, f32 fluctuationAlpha,
    f32 fluctuationScale) {
    populateParticleState(set, emitter, draw, ptcl, fluctuationAlpha, fluctuationScale);
    renderer.emitParticle(draw);
}


// ========================================================================== //


} // namespace Ptcl::Engine
