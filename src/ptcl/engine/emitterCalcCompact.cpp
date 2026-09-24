#include "ptcl/engine/emitterCalc.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

#include <QDebug>

#include "ptcl/engine/emitterInstance.h"
#include "ptcl/engine/emitterSetInstance.h"
#include "ptcl/engine/ptclRenderer.h"
#include "ptcl/engine/ptclSystem.h"


namespace Ptcl::Engine {


// ========================================================================== //


ParticleState EmitterCalc::ptclPreCalcCompact(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;

    ParticleState state = sSimplePreCalcFunctions[1](emitter, ptcl);
    state.followMode = ParticleFollowMode::None;

    ptcl.uvOffset.setZero();
    ptcl.uvSubOffset = res.textureUVScale();
    return state;
}

s32 EmitterCalc::calcCompact(EmitterInstance& emitter, PtclRenderer& renderer) {
    const auto& res = emitter.mResource;

    bool canEmit = true;
    if (!stepFadeAlpha(emitter, canEmit)) {
        return 0;
    }

    const s32 currentTime = emitter.mTickCount + 1;
    ++emitter.mTickCount;
    if (emitter.mSet.mCalcAlphaAnim && res.isStopEmitInFade()
        && emitter.mCompactStopFrame < 0) {
        emitter.mCompactStopFrame = currentTime;
    }
    const s32 frame = currentTime - emitter.mSet.mStartFrame;

    if (frame < 0) {
        return 0;
    }

    beginEmitterMatrices(emitter);

    const s32 rate = std::max<s32>(0, res.emitRate());
    const s32 step = std::max<s32>(1, res.lifeStep() + 1);
    const s32 stopFrame = (emitter.mCompactStopFrame < 0) ? 0x7fffffff : emitter.mCompactStopFrame;
    const s32 emissionEnd = (res.emitEndFrame() == 0x7fffffff) ? stopFrame : std::min(res.emitEndFrame(), stopFrame);
    const s32 emissionFrame = std::min(frame, emissionEnd - emitter.mSet.mStartFrame);

    if (canEmit && rate > 0 && emissionFrame >= 0) {
        const s32 emissionIndex = emissionFrame + 1;
        const s32 emissionSteps = emissionIndex / step + 1;
        const s32 targetCount = std::min<s32>(1024, rate * emissionSteps);
        const s32 newCount = std::max<s32>(0, targetCount - emitter.mCompactEmittedCount);

        if (newCount > 0) {
            if (emitPoint(emitter) != nullptr) {
                for (auto it = emitter.mParticles.begin(); it != emitter.mParticles.end(); ++it) {
                    PtclInstance* ptcl = *it;
                    if (ptcl->res != &res || ptcl->tickCount != 0) {
                        continue;
                    }
                    ptcl->emitterSRT = emitter.mEmitterSRT;
                    ptcl->emitterRT = emitter.mEmitterRT;
                    ptcl->uvOffset.setZero();
                    ptcl->uvSubOffset = res.textureUVScale();
                }
            }
        }
        emitter.mCompactEmittedCount = std::max(emitter.mCompactEmittedCount, targetCount);
    }

    if (res.emitEndFrame() != 0x7fffffff && frame > res.emitEndFrame() && emitter.mParticles.empty()) {
        mSys->killEmitter(emitter);
        return 0;
    }

    CompactDrawBatch batch;
    batch.state = makeParentState(emitter);
    batch.particles.reserve(emitter.mParticles.size());

    s32 aliveCount = 0;
    for (auto it = emitter.mParticles.begin(); it != emitter.mParticles.end();) {
        PtclInstance* ptcl = *it++;
        if (ptcl->life <= ptcl->tickCount) {
            emitter.mParticles.remove(ptcl);
            continue;
        }

        ParticleState draw = ptclPreCalcCompact(emitter, *ptcl);
        const auto& color = res.color0();
        Gfx::Color particleColor{0.0f, 0.0f, 0.0f, 1.0f};
        if (res.colorCalcType() == ColorCalcType::Pass1) {
            particleColor = color[0];
        } else if (res.colorCalcType() == ColorCalcType::Interpolate) {
            const s32 numRepeat = res.colorNumRepeat();
            s32 freq = numRepeat != 0 ? ptcl->life / numRepeat : 0;
            if (freq == 0) {
                freq = ptcl->life;
            }
            const s32 localCount = (ptcl->tickCount - 1) % std::max<s32>(1, freq);
            const s32 sec1 = (res.colorSection1() * freq) / 100;
            const s32 sec2 = (res.colorSection2() * freq) / 100;
            const s32 sec3 = (res.colorSection3() * freq) / 100;
            if (localCount < sec1) {
                particleColor = color[0];
            } else if (localCount < sec2) {
                const f32 t = static_cast<f32>(localCount - sec1)
                    / static_cast<f32>(std::max<s32>(1, sec2 - sec1));
                particleColor = {
                    std::fmaf(color[1].r() - color[0].r(), t, color[0].r()),
                    std::fmaf(color[1].g() - color[0].g(), t, color[0].g()),
                    std::fmaf(color[1].b() - color[0].b(), t, color[0].b()),
                    1.0f,
                };
            } else if (localCount < sec3) {
                const f32 t = static_cast<f32>(localCount - sec2)
                    / static_cast<f32>(std::max<s32>(1, sec3 - sec2));
                particleColor = {
                    std::fmaf(color[2].r() - color[1].r(), t, color[1].r()),
                    std::fmaf(color[2].g() - color[1].g(), t, color[1].g()),
                    std::fmaf(color[2].b() - color[1].b(), t, color[1].b()),
                    1.0f,
                };
            } else {
                particleColor = color[2];
            }
        }
        draw.matrix(0, 0) = particleColor.r() * emitter.mSet.color.r();
        draw.matrix(1, 0) = particleColor.g() * emitter.mSet.color.g();
        draw.matrix(2, 0) = particleColor.b() * emitter.mSet.color.b();
        calcRotZ(draw.matrix, *ptcl);
        populateParticleState(emitter.mSet, emitter, draw, *ptcl);

        batch.particles.push_back(draw);
        ++aliveCount;
    }

    if (!batch.particles.empty()) {
        renderer.emitCompact(batch);
    }

    return aliveCount;
}


// ========================================================================== //


} // namespace Ptcl::Engine
