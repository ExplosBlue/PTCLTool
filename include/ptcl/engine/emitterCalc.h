#pragma once

#include <array>
#include <cstddef>

#include "math/matrix.h"
#include "ptcl/engine/ptclRenderer.h"


namespace Ptcl {
class Emitter;
} // namespace Ptcl


namespace Ptcl::Engine {


// ========================================================================== //


class EmitterInstance;
class PtclInstance;
class PtclSystem;
class EmitterSetInstance;

std::size_t ptclRotCombinerIndex(const Ptcl::Emitter& res);
void unlinkPtcl(PtclInstance*& head, PtclInstance* ptcl);

class EmitterCalc {
public:
    static void emitCommon(EmitterInstance& emitter, PtclInstance& ptcl);
    static void addParticle(EmitterInstance& emitter, PtclInstance& ptcl);

    static PtclInstance* emitParticle(EmitterInstance& emitter);

    static s32 calcSimple(EmitterInstance& emitter, PtclRenderer& renderer);
    static s32 calcComplex(EmitterInstance& emitter, PtclRenderer& renderer);

    static s32 calc(EmitterInstance& emitter, PtclRenderer& renderer);

private:
    friend class PtclSystem;

    using CalcFunction = s32 (*)(EmitterInstance& emitter, PtclRenderer& renderer);

    static inline std::array<CalcFunction, 3> sCalcFunctions = {
        &EmitterCalc::calcSimple,
        &EmitterCalc::calcComplex,
        nullptr,
    };

    using EmitFunction = PtclInstance* (*)(EmitterInstance&);

    static PtclInstance* emitPoint(EmitterInstance& emitter);
    static PtclInstance* emitCircle(EmitterInstance& emitter);
    static PtclInstance* emitCircleSameDivide(EmitterInstance& emitter);
    static PtclInstance* emitFillCircle(EmitterInstance& emitter);
    static PtclInstance* emitSphere(EmitterInstance& emitter);
    static PtclInstance* emitSphereSameDivide(EmitterInstance& emitter);
    static PtclInstance* emitFillSphere(EmitterInstance& emitter);
    static PtclInstance* emitCylinder(EmitterInstance& emitter);
    static PtclInstance* emitFillCylinder(EmitterInstance& emitter);
    static PtclInstance* emitBox(EmitterInstance& emitter);
    static PtclInstance* emitFillBox(EmitterInstance& emitter);
    static PtclInstance* emitLine(EmitterInstance& emitter);
    static PtclInstance* emitLineSameDivide(EmitterInstance& emitter);
    static PtclInstance* emitRectangle(EmitterInstance& emitter);

    static inline std::array<EmitFunction, 14> sEmitFunctions = {
        &EmitterCalc::emitPoint,
        &EmitterCalc::emitCircle,
        &EmitterCalc::emitCircleSameDivide,
        &EmitterCalc::emitFillCircle,
        &EmitterCalc::emitSphere,
        &EmitterCalc::emitSphereSameDivide,
        &EmitterCalc::emitFillSphere,
        &EmitterCalc::emitCylinder,
        &EmitterCalc::emitFillCylinder,
        &EmitterCalc::emitBox,
        &EmitterCalc::emitFillBox,
        &EmitterCalc::emitLine,
        &EmitterCalc::emitLineSameDivide,
        &EmitterCalc::emitRectangle,
    };

    using FieldFunction = void (*)(EmitterInstance& emitter, PtclInstance& ptcl);

    static void calcFieldRandom(EmitterInstance& emitter, PtclInstance& ptcl);
    static void calcFieldMagnet(EmitterInstance& emitter, PtclInstance& ptcl);
    static void calcFieldSpin(EmitterInstance& emitter, PtclInstance& ptcl);
    static void calcFieldCollision(EmitterInstance& emitter, PtclInstance& ptcl);
    static void calcFieldConvergence(EmitterInstance& emitter, PtclInstance& ptcl);
    static void calcFieldPosAdd(EmitterInstance& emitter, PtclInstance& ptcl);

    static void calcFields(EmitterInstance& emitter, PtclInstance& ptcl);

    static void calcFluctuation(EmitterInstance& emitter, PtclInstance& ptcl);
    static void calcTexPatAnim(EmitterInstance& emitter, PtclInstance& ptcl);

    static inline std::array<FieldFunction, 6> sFieldFunctions = {
        &EmitterCalc::calcFieldRandom,
        &EmitterCalc::calcFieldMagnet,
        &EmitterCalc::calcFieldSpin,
        &EmitterCalc::calcFieldCollision,
        &EmitterCalc::calcFieldConvergence,
        &EmitterCalc::calcFieldPosAdd,
    };

    using PtclCalcFunction = Math::Matrix34f* (*)(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);

    static void calcRotNone(Math::Matrix34f& matrix, PtclInstance& ptcl);
    static void calcRotX(Math::Matrix34f& matrix, PtclInstance& ptcl);
    static void calcRotY(Math::Matrix34f& matrix, PtclInstance& ptcl);
    static void calcRotZ(Math::Matrix34f& matrix, PtclInstance& ptcl);
    static void calcRotXYZ(Math::Matrix34f& matrix, PtclInstance& ptcl);

    static Math::Matrix34f* ptclCalcRotNoneColorNoWork(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* ptclCalcRotXColorNoWork(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* ptclCalcRotYColorNoWork(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* ptclCalcRotZColorNoWork(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* ptclCalcRotXYZColorNoWork(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);

    static Math::Matrix34f* ptclCalcRotNonePassColor1(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* ptclCalcRotXPassColor1(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* ptclCalcRotYPassColor1(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* ptclCalcRotZPassColor1(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* ptclCalcRotXYZPassColor1(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);

    static Math::Matrix34f* ptclCalcRotNoneInterpolate3Colors(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* ptclCalcRotXInterpolate3Colors(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* ptclCalcRotYInterpolate3Colors(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* ptclCalcRotZInterpolate3Colors(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* ptclCalcRotXYZInterpolate3Colors(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);

    static inline std::array<PtclCalcFunction, 15> sPtclCalcFunctions = {
        &EmitterCalc::ptclCalcRotNoneColorNoWork,
        &EmitterCalc::ptclCalcRotXColorNoWork,
        &EmitterCalc::ptclCalcRotYColorNoWork,
        &EmitterCalc::ptclCalcRotZColorNoWork,
        &EmitterCalc::ptclCalcRotXYZColorNoWork,
        &EmitterCalc::ptclCalcRotNonePassColor1,
        &EmitterCalc::ptclCalcRotXPassColor1,
        &EmitterCalc::ptclCalcRotYPassColor1,
        &EmitterCalc::ptclCalcRotZPassColor1,
        &EmitterCalc::ptclCalcRotXYZPassColor1,
        &EmitterCalc::ptclCalcRotNoneInterpolate3Colors,
        &EmitterCalc::ptclCalcRotXInterpolate3Colors,
        &EmitterCalc::ptclCalcRotYInterpolate3Colors,
        &EmitterCalc::ptclCalcRotZInterpolate3Colors,
        &EmitterCalc::ptclCalcRotXYZInterpolate3Colors,
    };

    static Math::Matrix34f* childPtclCalcRotNone(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* childPtclCalcRotX(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* childPtclCalcRotY(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* childPtclCalcRotZ(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);
    static Math::Matrix34f* childPtclCalcRotXYZ(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);

    static inline std::array<PtclCalcFunction, 5> sChildPtclCalcFunctions = {
        &EmitterCalc::childPtclCalcRotNone,
        &EmitterCalc::childPtclCalcRotX,
        &EmitterCalc::childPtclCalcRotY,
        &EmitterCalc::childPtclCalcRotZ,
        &EmitterCalc::childPtclCalcRotXYZ,
    };

    using PreCalcFunction = ParticleState (*)(EmitterInstance& emitter, PtclInstance& ptcl);

    static ParticleState ptclPreCalcFollowAllSimple(EmitterInstance& emitter, PtclInstance& ptcl);
    static ParticleState ptclPreCalcFollowNoneSimple(EmitterInstance& emitter, PtclInstance& ptcl);
    static ParticleState ptclPreCalcFollowPosOnlySimple(EmitterInstance& emitter, PtclInstance& ptcl);
    static ParticleState ptclPreCalcFollowAllComplex(EmitterInstance& emitter, PtclInstance& ptcl);
    static ParticleState ptclPreCalcFollowNoneComplex(EmitterInstance& emitter, PtclInstance& ptcl);
    static ParticleState ptclPreCalcFollowPosOnlyComplex(EmitterInstance& emitter, PtclInstance& ptcl);

    static inline std::array<PreCalcFunction, 3> sSimplePreCalcFunctions = {
        &EmitterCalc::ptclPreCalcFollowAllSimple,
        &EmitterCalc::ptclPreCalcFollowNoneSimple,
        &EmitterCalc::ptclPreCalcFollowPosOnlySimple,
    };

    static inline std::array<PreCalcFunction, 3> sComplexPreCalcFunctions = {
        &EmitterCalc::ptclPreCalcFollowAllComplex,
        &EmitterCalc::ptclPreCalcFollowNoneComplex,
        &EmitterCalc::ptclPreCalcFollowPosOnlyComplex,
    };

    static void calcComplexParticle(EmitterInstance& emitter, PtclInstance& ptcl);
    static s32 calcComplexParent(EmitterInstance& emitter, PtclRenderer& renderer);
    static s32 calcComplexChild(EmitterInstance& emitter, PtclRenderer& renderer);

    static bool stepFadeAlpha(EmitterInstance& emitter, bool& canEmit);
    static void scheduleEmission(EmitterInstance& emitter, s32 frame, bool canEmit);
    static void beginEmitterMatrices(EmitterInstance& emitter);
    static EmitterState emitParentState(PtclRenderer& renderer, const EmitterInstance& emitter);
    static void emitChildState(PtclRenderer& renderer, const EmitterInstance& emitter);
    static void emitParticleState(PtclRenderer& renderer, const EmitterSetInstance& set,
        const EmitterInstance& emitter, ParticleState& draw, const PtclInstance& ptcl,
        f32 fluctuationAlpha = 1.0f, f32 fluctuationScale = 1.0f);

private:
    static PtclSystem* mSys;
};


// ========================================================================== //


} // namespace Ptcl::Engine
