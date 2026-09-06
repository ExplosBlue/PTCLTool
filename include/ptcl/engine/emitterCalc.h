#pragma once

#include <array>

#include "math/matrix.h"


namespace Ptcl::Engine {


// ========================================================================== //


class EmitterInstance;
class PtclInstance;
class PtclSystem;

class EmitterCalc {
public:
    static void emitCommon(EmitterInstance& emitter, PtclInstance& ptcl);
    static void addParticle(EmitterInstance& emitter, PtclInstance& ptcl);

    static PtclInstance* emitParticle(EmitterInstance& emitter);

private:
    friend class PtclSystem;

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

    static void calcFluctuation(EmitterInstance& emitter, PtclInstance& ptcl);

    static inline std::array<FieldFunction, 6> sFieldFunctions = {
        &EmitterCalc::calcFieldRandom,
        &EmitterCalc::calcFieldMagnet,
        &EmitterCalc::calcFieldSpin,
        &EmitterCalc::calcFieldCollision,
        &EmitterCalc::calcFieldConvergence,
        &EmitterCalc::calcFieldPosAdd,
    };

    using PtclCalcFunction = Math::Matrix34f* (*)(Math::Matrix34f* buffAddr, EmitterInstance& emitter, PtclInstance& ptcl);

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

private:
    static PtclSystem* mSys;
};


// ========================================================================== //


} // namespace Ptcl::Engine
