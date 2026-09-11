#include "ptcl/engine/emitterCalc.h"

#include <cstddef>

#include "math/vector.h"
#include "ptcl/engine/emitterInstance.h"
#include "ptcl/engine/emitterSetInstance.h"
#include "ptcl/engine/ptclInstance.h"
#include "ptcl/engine/ptclRenderer.h"
#include "ptcl/engine/ptclSystem.h"


namespace Ptcl::Engine {

namespace /* Anonymous */ {

constexpr s32 cStripeNumHistoryMask = static_cast<s32>(PtclSystem::cStripeNumHistory) - 1;

} // namespace /* Anonymous */

Math::Matrix34f* EmitterCalc::childPtclCalcRotNone(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    const auto& set = emitter.mSet;

    buff(0, 0) = ptcl.color.r() * set.color.r();
    buff(1, 0) = ptcl.color.g() * set.color.g();
    buff(2, 0) = ptcl.color.b() * set.color.b();

    calcRotNone(buff, ptcl);

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::childPtclCalcRotX(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    const auto& set = emitter.mSet;

    buff(0, 0) = ptcl.color.r() * set.color.r();
    buff(1, 0) = ptcl.color.g() * set.color.g();
    buff(2, 0) = ptcl.color.b() * set.color.b();

    calcRotX(buff, ptcl);

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::childPtclCalcRotY(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    const auto& set = emitter.mSet;

    buff(0, 0) = ptcl.color.r() * set.color.r();
    buff(1, 0) = ptcl.color.g() * set.color.g();
    buff(2, 0) = ptcl.color.b() * set.color.b();

    calcRotY(buff, ptcl);

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::childPtclCalcRotZ(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    const auto& set = emitter.mSet;

    buff(0, 0) = ptcl.color.r() * set.color.r();
    buff(1, 0) = ptcl.color.g() * set.color.g();
    buff(2, 0) = ptcl.color.b() * set.color.b();

    calcRotZ(buff, ptcl);

    return ++buffAddr;
}

Math::Matrix34f* EmitterCalc::childPtclCalcRotXYZ(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl) {
    auto& buff = buffAddr[0];

    const auto& set = emitter.mSet;

    buff(0, 0) = ptcl.color.r() * set.color.r();
    buff(1, 0) = ptcl.color.g() * set.color.g();
    buff(2, 0) = ptcl.color.b() * set.color.b();

    calcRotXYZ(buff, ptcl);

    return ++buffAddr;
}


// ========================================================================== //


s32 EmitterCalc::calcComplex(EmitterInstance& emitter, PtclRenderer& renderer) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    bool canEmit;
    if (!stepFadeAlpha(emitter, canEmit)) {
        return 0;
    }

    const s32 frame = emitter.mTickCount - set.mStartFrame;
    scheduleEmission(emitter, frame, canEmit);

    ++emitter.mTickCount;

    if (!(res.emitEndFrame() > frame && canEmit) && emitter.mPtclHead == nullptr && emitter.mChildHead == nullptr) {
        mSys->killEmitter(emitter);
        return 0;
    }

    beginEmitterMatrices(emitter);

    s32 aliveCount = calcComplexParent(emitter, renderer);

    aliveCount += calcComplexChild(emitter, renderer);

    return aliveCount;
}


// ========================================================================== //


s32 EmitterCalc::calcComplexParent(EmitterInstance& emitter, PtclRenderer& renderer) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    if (emitter.mPtclHead == nullptr) {
        return 0;
    }

    emitParentState(renderer, emitter);

    s32 aliveCount = 0;

    PtclInstance* ptcl = emitter.mPtclHead;
    while (ptcl != nullptr) {
        PtclInstance* cur = ptcl;
        ptcl = cur->next;


        ++aliveCount;

        if (cur->life > cur->tickCount) {
            if (cur->tickCount == 0) {
                cur->fluctuationAlpha = 1.0f;
                cur->fluctuationScale = 1.0f;
            }

            ParticleState draw = sComplexPreCalcFunctions[static_cast<std::size_t>(res.followType())](emitter, *cur);
            sPtclCalcFunctions[ptclRotCombinerIndex(res)](&draw.matrix, emitter, *cur);

            const bool isStripeBillboard = static_cast<u32>(res.billboardType()) == static_cast<u32>(BillboardType::Stripe);
            if (res.childFlags().isSet(ChildFlag::Unk80) && !isStripeBillboard) {
                emitParticleState(renderer, set, emitter, draw, *cur, cur->fluctuationAlpha,
                    cur->fluctuationScale);
            }

            calcComplexParticle(emitter, *cur);
        } else {
            PtclStripe* stripe = cur->stripe;
            if (stripe == nullptr) {
                cur->res = nullptr;
                unlinkPtcl(emitter.mPtclHead, cur);
                continue;
            }
            if (stripe->histQStart != stripe->histQEnd) {
                stripe->histQStart = (stripe->histQStart + 1) & cStripeNumHistoryMask;
                --stripe->numHistory;
                stripe->color = {
                    set.color.r(),
                    set.color.g(),
                    set.color.b(),
                    (cur->alpha * set.color.a()) * emitter.mFadeAlpha,
                };
                stripe->emitterSRT = emitter.mEmitterSRT;
            } else {
                mSys->removeStripe(*stripe);
                cur->stripe = nullptr;
                unlinkPtcl(emitter.mPtclHead, cur);
            }
        }
    }

    return aliveCount;
}


// ========================================================================== //


void EmitterCalc::calcComplexParticle(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    if (static_cast<u32>(res.billboardType()) == static_cast<u32>(BillboardType::Stripe)) {
        if (ptcl.tickCount == 1) {
            ptcl.stripe = mSys->allocAndConnectStripe(emitter, ptcl);
        }

        PtclStripe* stripe = ptcl.stripe;
        if (stripe != nullptr) {
            PtclStripeHistory& now = stripe->hist[stripe->histQEnd];
            if (res.stripeFlags().isSet(StripeFlag::EmitterCoord)) {
                now.pos = ptcl.position;
                stripe->emitterSRT = Math::Matrix34f{
                    {1.0f, 0.0f, 0.0f, 0.0f},
                    {0.0f, 1.0f, 0.0f, 0.0f},
                    {0.0f, 0.0f, 1.0f, 0.0f},
                };
            } else {
                now.pos = ptcl.positionDiff;
                stripe->emitterSRT = emitter.mEmitterSRT;
            }
            now.scale = ptcl.scale.x * set.mParticleScaleForCalc.x;

            const s32 prevCount = stripe->numHistory;
            if (prevCount >= 2) {
                const PtclStripeHistory& prev = stripe->hist[(stripe->histQEnd - 1) & cStripeNumHistoryMask];
                Math::Vector3f dir = Math::VectorCalc::sub(now.pos, prev.pos);
                Math::VectorCalc::normalize(dir);
                dir = Math::VectorCalc::sub(dir, stripe->interpolateNextDir);
                stripe->interpolateNextDir = Math::VectorCalc::add(
                    stripe->interpolateNextDir, Math::VectorCalc::multScalar(dir, res.stripeDirInterpolate()));
            } else if (prevCount >= 1) {
                const PtclStripeHistory& prev = stripe->hist[(stripe->histQEnd - 1) & cStripeNumHistoryMask];
                stripe->interpolateNextDir = Math::VectorCalc::sub(now.pos, prev.pos);
            }
            Math::VectorCalc::normalize(stripe->interpolateNextDir);

            const f32 outScale = now.scale;
            const Math::Matrix34f& stripeSRT = stripe->emitterSRT;
            switch (res.stripeType()) {
            case StripeType::EmitterUpDown: {
                now.outer = Math::VectorCalc::multScalar(
                    Math::Vector3f{stripeSRT(0, 1), stripeSRT(1, 1), stripeSRT(2, 1)}, outScale);
                break;
            }
            case StripeType::EmitterMatrix: {
                Math::Vector3f outer = Math::VectorCalc::cross(
                    Math::Vector3f{stripeSRT(0, 1), stripeSRT(1, 1), stripeSRT(2, 1)},
                    stripe->interpolateNextDir);
                Math::VectorCalc::normalize(outer);
                now.outer = Math::VectorCalc::multScalar(outer, outScale);
                break;
            }
            case StripeType::Billboard:
            default: {
                now.outer = stripe->interpolateNextDir;
                break;
            }
            }

            if (stripe->numHistory >= static_cast<s32>(PtclSystem::cStripeNumHistory)) {
                stripe->histQStart = (stripe->histQStart + 1) & cStripeNumHistoryMask;
            } else {
                ++stripe->numHistory;
            }
            stripe->histQEnd = (stripe->histQEnd + 1) & cStripeNumHistoryMask;

            stripe->uvOffset = ptcl.uvOffset;
            stripe->uvSubOffset = ptcl.uvSubOffset;
            stripe->color = {
                set.color.r(),
                set.color.g(),
                set.color.b(),
                (ptcl.alpha * set.color.a()) * emitter.mFadeAlpha,
            };
        }
    }

    if (res.childFlags().isSet(ChildFlag::Enabled)) {
        const s32 count = ptcl.tickCount - 1;
        s32& childEmitCount = ptcl.childEmitCount;
        if (count == 0) {
            childEmitCount = 0x7FFFFFFF;
        }

        const u64 emitTime = (static_cast<u64>(0x51EB851FULL)
            * static_cast<u64>(ptcl.life - 1) * static_cast<u64>(res.childEmitTiming())) >> 32;
        const s32 threshold = static_cast<s32>(emitTime >> 5) - static_cast<s32>(emitTime >> 31);

        if (count >= threshold) {
            if (res.childEmitStep() <= childEmitCount) {
                childEmitCount = 0;

                for (s32 i = 0; i < res.childEmitRate(); ++i) {
                    PtclInstance* child = mSys->allocPtcl();
                    if (child == nullptr) {
                        break;
                    }

                    child->res = &res;
                    child->stripe = nullptr;
                    child->tickCount = 0;
                    child->life = res.childLife();
                    child->prev = nullptr;
                    child->next = nullptr;

                    Math::Vector3f vel{0.0f, 0.0f, 0.0f};
                    if (res.childFlags().isSet(ChildFlag::VelInherit)) {
                        vel = Math::VectorCalc::multScalar(ptcl.posDiff, res.childVelocityInheritRate());
                    }

                    const Math::Vector3f& norm = emitter.mRandom.getNormalizedVec3();
                    const Math::Vector3f& randV = emitter.mRandom.getVec3();
                    const Math::Vector3f& childRandVel = res.childRandVelocity();
                    const f32 figureVel = res.childFigureVelocity();

                    child->velocity.x = vel.x + norm.x * figureVel + randV.x * childRandVel.x;
                    child->velocity.y = vel.y + norm.y * figureVel + randV.y * childRandVel.y;
                    child->velocity.z = vel.z + norm.z * figureVel + randV.z * childRandVel.z;
                    child->position = Math::VectorCalc::add(ptcl.position, Math::VectorCalc::multScalar(norm, res.childInitalPositionRand()));

                    if (res.childFlags().isSet(ChildFlag::RotateInherit)) {
                        child->rotation = ptcl.rotation;
                    } else {
                        child->rotation.x = res.childInitialRotation().x + emitter.mRandom.getS32() * res.childInitialRotationRandom().x;
                        child->rotation.y = res.childInitialRotation().y + emitter.mRandom.getS32() * res.childInitialRotationRandom().y;
                        child->rotation.z = res.childInitialRotation().z + emitter.mRandom.getS32() * res.childInitialRotationRandom().z;
                    }

                    child->rotationVelocity.x = res.childRotationVelocity().x + emitter.mRandom.getS32() * res.childRotationVelocityRandom().x;
                    child->rotationVelocity.y = res.childRotationVelocity().y + emitter.mRandom.getS32() * res.childRotationVelocityRandom().y;
                    child->rotationVelocity.z = res.childRotationVelocity().z + emitter.mRandom.getS32() * res.childRotationVelocityRandom().z;

                    if (res.childFlags().isSet(ChildFlag::Color0Inherit)) {
                        child->color = ptcl.color;
                    } else {
                        child->color = res.childSecondaryColor();
                    }

                    const f32 childAlpha = res.childFlags().isSet(ChildFlag::AlphaInherit)
                        ? ptcl.alpha * ptcl.fluctuationAlpha
                        : res.childAlpha();

                    if (res.childAlphaBaseFrame() != 0) {
                        child->alpha = res.childAlphaInit();
                        child->alphaAddSection1
                            = (childAlpha - res.childAlphaInit()) / static_cast<f32>(res.childAlphaBaseFrame());
                    } else {
                        child->alpha = childAlpha;
                        child->alphaAddSection1 = 0.0f;
                    }

                    child->alphaAddSection2 = (res.childAlphaTarget() - childAlpha)
                        / static_cast<f32>(res.childLife() - res.childAlphaStartFrame());

                    if (res.childFlags().isSet(ChildFlag::ScaleInherit)) {
                        const f32 factor = res.childScaleInheritRate() * ptcl.fluctuationScale;
                        child->scale.x = ptcl.scale.x * factor;
                        child->scale.y = ptcl.scale.y * factor;
                    } else {
                        child->scale = res.childScale();
                    }

                    child->scaleAddSection1.x = (child->scale.x * (res.childScaleTarget().x - 1.0f))
                        / static_cast<f32>(res.childLife() - res.childScaleStartFrame());
                    child->scaleAddSection1.y = (child->scale.y * (res.childScaleTarget().y - 1.0f))
                        / static_cast<f32>(res.childLife() - res.childScaleStartFrame());

                    child->uvOffset = {0.0f, 0.0f};
                    child->uvSubOffset = res.childTextureUVScale();

                    if (!res.childFlags().isSet(ChildFlag::IsFollow)) {
                        if (res.isFollow()) {
                            child->emitterRT = emitter.mEmitterRT;
                            child->emitterSRT = emitter.mEmitterSRT;
                        } else {
                            child->emitterRT = ptcl.emitterRT;
                            child->emitterSRT = ptcl.emitterSRT;
                        }
                    }

                    child->posDiff = child->position;
                    child->random = emitter.mRandom.getU32();

                    child->prev = nullptr;
                    child->next = emitter.mChildHead;
                    if (emitter.mChildHead != nullptr) {
                        emitter.mChildHead->prev = child;
                    }
                    emitter.mChildHead = child;
                }
            } else {
                ++childEmitCount;
            }
        }
    }
}


// ========================================================================== //


s32 EmitterCalc::calcComplexChild(EmitterInstance& emitter, PtclRenderer& renderer) {
    const auto& res = emitter.mResource;
    const auto& set = emitter.mSet;

    if (emitter.mChildHead == nullptr) {
        return 0;
    }

    emitChildState(renderer, emitter);

    s32 aliveCount = 0;

    PtclInstance* child = emitter.mChildHead;
    while (child != nullptr) {
        PtclInstance* cur = child;
        child = cur->next;

        if (cur->life > cur->tickCount) {
            if (res.childFlags().isSet(ChildFlag::IsFollow)) {
                cur->emitterSRT = emitter.mEmitterSRT;
                cur->emitterRT = emitter.mEmitterRT;
            }

            const Math::Vector3f prevPos = cur->position;

            if (res.childFlags().isSet(ChildFlag::Unk100)) {
                cur->velocity = Math::VectorCalc::add(cur->velocity, Math::VectorCalc::rotate(emitter.mEmitterRT, res.childGravity()));
            } else {
                cur->velocity = Math::VectorCalc::add(cur->velocity, res.childGravity());
            }
            cur->velocity = Math::VectorCalc::multScalar(cur->velocity, res.childAirResistance());
            cur->position = Math::VectorCalc::add(cur->position, cur->velocity);

            if (res.childFlags().isSet(ChildFlag::ParentField)) {
                calcFields(emitter, *cur);
            }

            cur->positionDiff = Math::VectorCalc::multiply(cur->emitterSRT, cur->position);

            if (cur->tickCount < res.childAlphaBaseFrame()) {
                cur->alpha += cur->alphaAddSection1;
            } else if (cur->tickCount >= res.childAlphaStartFrame()) {
                cur->alpha += cur->alphaAddSection2;
            }
            if (cur->tickCount >= res.childScaleStartFrame()) {
                cur->scale.x += cur->scaleAddSection1.x;
                cur->scale.y += cur->scaleAddSection1.y;
            }

            ParticleState draw;
            u32 followCount = 0;
            if (res.childFlags().isSet(ChildFlag::IsPolygon)) {
                followCount = 3;
                draw.followBasis = cur->emitterRT;
            } else {
                const u32 childBillboard = static_cast<u32>(res.childBillboardType());
                if (childBillboard == 3 || childBillboard == 4) {
                    followCount = 1;
                    const Math::Vector3f dir = Math::VectorCalc::rotate(cur->emitterSRT, Math::VectorCalc::sub(cur->position, prevPos));
                    draw.followDirection = dir;
                }
            }
            draw.followCount = followCount;

            sChildPtclCalcFunctions[static_cast<std::size_t>(res.childRotationType())](&draw.matrix, emitter, *cur);

            emitParticleState(renderer, set, emitter, draw, *cur);

            ++cur->tickCount;
            ++aliveCount;
        } else {
            unlinkPtcl(emitter.mChildHead, cur);
        }
    }

    return aliveCount;
}


// ========================================================================== //


} // namespace Ptcl::Engine
