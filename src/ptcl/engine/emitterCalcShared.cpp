#include "ptcl/engine/emitterCalc.h"

#include <cstddef>

#include "gfx/color.h"
#include "math/matrix.h"
#include "math/vector.h"
#include "ptcl/engine/emitterInstance.h"
#include "ptcl/engine/emitterSetInstance.h"
#include "ptcl/engine/ptclInstance.h"
#include "ptcl/engine/renderContract.h"
#include "ptcl/engine/ptclSystem.h"


namespace Ptcl::Engine {


// ========================================================================== //


namespace {

Gfx::Color tint(const Gfx::Color& color, const Gfx::Color& tintColor) {
    return {
        color.r() * tintColor.r(),
        color.g() * tintColor.g(),
        color.b() * tintColor.b(),
        1.0f,
    };
}

} // namespace


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
        .rotationBasis         = res.rotationBasis(),
        .color                 = tint(res.primaryColor(), set.color),
        .secondaryColor        = tint(res.secondaryColor(), set.color),
        .textureWrapS          = res.textureWrapS(),
        .textureWrapT          = res.textureWrapT(),
        .textureFilter         = res.textureFilter(),
        .textureLodLevel       = res.textureLodLevel(),
        .emitterRT             = emitter.mEmitterRT,
    };
}

ParticleDrawBatch& EmitterCalc::beginParentBatch(DrawFrame& frame, const EmitterInstance& emitter) {
    frame.batches.push_back({makeParentState(emitter), {}});
    return frame.batches.back();
}

ParticleDrawBatch& EmitterCalc::beginChildBatch(DrawFrame& frame, const EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    const EmitterState state{
        .textureHandle         = &res.childTextureHandle(),
        .billboardType         = res.childBillboardType(),
        .isEmitterBillboardMtx = res.childFlags().isSet(ChildFlag::IsEmitterBillboardMtx),
        .blendFunction         = res.childBlendFunc(),
        .depthFunction         = res.childDepthFunc(),
        .combinerFunction      = res.childCombinerFunc(),
        .rotationBasis         = res.childRotationBasis(),
        .color                 = tint(res.childPrimaryColor(), set.color),
        .secondaryColor        = tint(res.childSecondaryColor(), set.color),
        .textureWrapS          = res.childTextureWrapS(),
        .textureWrapT          = res.childTextureWrapT(),
        .textureFilter         = res.childTextureFilter(),
        .textureLodLevel       = res.childTextureLodLevel(),
        .emitterRT             = emitter.mEmitterRT,
    };

    frame.batches.push_back({state, {}});
    return frame.batches.back();
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

void EmitterCalc::appendParticleState(ParticleDrawBatch& batch, const EmitterSetInstance& set,
    const EmitterInstance& emitter, ParticleState& draw, const PtclInstance& ptcl, f32 fluctuationAlpha,
    f32 fluctuationScale) {
    populateParticleState(set, emitter, draw, ptcl, fluctuationAlpha, fluctuationScale);
    batch.particles.push_back(draw);
}


// ========================================================================== //


} // namespace Ptcl::Engine
