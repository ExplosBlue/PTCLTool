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

void unlinkPtcl(PtclInstance*& head, PtclInstance* ptcl) {
    ptcl->res = nullptr;
    if (head == ptcl) {
        head = ptcl->next;
        if (head != nullptr) {
            head->prev = nullptr;
        }
    } else {
        if (ptcl->next != nullptr) {
            ptcl->next->prev = ptcl->prev;
        }
        if (ptcl->prev != nullptr) {
            ptcl->prev->next = ptcl->next;
        }
    }
}


// ========================================================================== //


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

    if (!canEmit || frame < res.emitStartFrame() || frame > res.emitEndFrame()) {
        return;
    }

    const u32 ratePerFrame = emitter.mController.emitRatePerFrame;
    const s32 interval = static_cast<s32>(static_cast<u32>(emitter.mEmissionInterval) * ratePerFrame) >> 8;

    if (interval > static_cast<s32>(emitter.mLoopIntervalRaw)) {
        emitter.mLoopIntervalRaw++;
        return;
    }

    const s32 numPoint = set.mNumParticleEmissionPoint;
    if (numPoint <= 0) {
        sEmitFunctions[static_cast<std::size_t>(res.volumeType())](emitter);
    } else {
        for (s32 point = 0; point < numPoint; ++point) {
            auto* prevHead = emitter.mPtclHead;
            sEmitFunctions[static_cast<std::size_t>(res.volumeType())](emitter);
            for (auto* ptcl = emitter.mPtclHead; ptcl != prevHead; ptcl = ptcl->next) {
                ptcl->position.x += set.mParticleEmissionPoints[point].x;
                ptcl->position.y += set.mParticleEmissionPoints[point].y;
                ptcl->position.z += set.mParticleEmissionPoints[point].z;
            }
        }
    }
    emitter.mLoopIntervalRaw = 0;
    emitter.mEmissionInterval = emitter.mRandom.getS32(res.lifeStepRandom()) + res.lifeStep();
}

void EmitterCalc::beginEmitterMatrices(EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    Math::MatrixCalc::multiply(emitter.mEmitterRT, set.RT, res.transformRT());
    Math::MatrixCalc::multiply(emitter.mEmitterSRT, set.SRT, res.transformSRT());
}

void EmitterCalc::emitParentState(PtclRenderer& renderer, const EmitterInstance& emitter) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    renderer.setEmitterState({
        .textureHandle         = &res.textureHandle(),
        .billboardType         = res.billboardType(),
        .isEmitterBillboardMtx = res.isEmitterBillboardMtx(),
        .blendFunction         = res.blendFunction(),
        .depthFunction         = res.depthFunction(),
        .combinerFunction      = res.combinerFunction(),
        .colorCalcType         = res.colorCalcType(),
        .isFogEnabled          = res.isFogEnabled(),
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
    });
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
        .colorCalcType         = ColorCalcType::None,
        .isFogEnabled          = res.isFogEnabled(),
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

void EmitterCalc::emitParticleState(PtclRenderer& renderer, const EmitterSetInstance& set,
    const EmitterInstance& emitter, ParticleState& draw, const PtclInstance& ptcl) {
    draw.positionDiff = ptcl.positionDiff;
    draw.scale = {
        ptcl.scale.x * set.mParticleScaleForCalc.x,
        ptcl.scale.y * set.mParticleScaleForCalc.y,
    };
    draw.alpha = (ptcl.alpha * set.color.a()) * emitter.mFadeAlpha;
    draw.uvOffset = ptcl.uvOffset;
    draw.uvSubOffset = ptcl.uvSubOffset;
    draw.color = {draw.matrix(0, 0), draw.matrix(1, 0), draw.matrix(2, 0), 1.0f};

    renderer.emitParticle(draw);
}


// ========================================================================== //


} // namespace Ptcl::Engine
