#include "ptcl/engine/emitterCalc.h"

#include "math/sinCosIdx.h"
#include "ptcl/engine/emitterInstance.h"


namespace Ptcl::Engine {


// ========================================================================== //


void EmitterCalc::calcFieldRandom(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;

    if (ptcl.tickCount % res.fieldRandomBlank() == 0) {
        const Math::Vector3f& randVec3 = emitter.mRandom.getVec3();

        ptcl.velocity.x += randVec3.x * res.fieldRandomVelAdd().x;
        ptcl.velocity.y += randVec3.y * res.fieldRandomVelAdd().y;
        ptcl.velocity.z += randVec3.z * res.fieldRandomVelAdd().z;
    }
}

void EmitterCalc::calcFieldMagnet(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;

    const Math::Vector3f& magnetPos = res.fieldMagnetPos();
    const f32 magnetPower = res.fieldMagnetPower();

    if (res.isFieldMagnetAxisTargetX()) {
        ptcl.velocity.x += ((magnetPos.x - ptcl.position.x) - ptcl.velocity.x) * magnetPower;
    }
    if (res.isFieldMagnetAxisTargetY()) {
        ptcl.velocity.y += ((magnetPos.y - ptcl.position.y) - ptcl.velocity.y) * magnetPower;
    }
    if (res.isFieldMagnetAxisTargetZ()) {
        ptcl.velocity.z += ((magnetPos.z - ptcl.position.z) - ptcl.velocity.z) * magnetPower;
    }
}

void EmitterCalc::calcFieldSpin(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;

    f32 sin, cos;
    Math::sinCosIdx(&sin, &cos, static_cast<u32>(res.fieldSpinRotate()));

    switch (res.fieldSpinAxis()) {
    case Ptcl::FieldSpinAxis::AxisX: {
            const f32 y = ptcl.position.y;
            const f32 z = ptcl.position.z;
            ptcl.position.y = y * cos + z * sin;
            ptcl.position.z = z * cos - y * sin;
        }
        break;
    case Ptcl::FieldSpinAxis::AxisY: {
            const f32 x = ptcl.position.x;
            const f32 z = ptcl.position.z;
            ptcl.position.z = z * cos + x * sin;
            ptcl.position.x = x * cos - z * sin;
        }
        break;
    case Ptcl::FieldSpinAxis::AxisZ: {
            const f32 x = ptcl.position.x;
            const f32 y = ptcl.position.y;
            ptcl.position.x = x * cos + y * sin;
            ptcl.position.y = y * cos - x * sin;
        }
        break;
    default:
        break;
    }
}

void EmitterCalc::calcFieldCollision(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;

    const f32 coord = res.fieldCollisionCoord();
    const f32 coef = res.fieldCollisionCoef();

    if (res.fieldCollisionIsWorld()) {
        const auto& matrixSRT = res.isFollow() ? emitter.mEmitterSRT : ptcl.emitterSRT;

        Math::Vector3f worldPos = Math::MatrixCalc::multiply(matrixSRT, ptcl.position);

        switch(res.fieldCollisionType()) {
        case Ptcl::FieldCollisionType::Die:
            if (worldPos.y < coord) {
                worldPos.y = coord;
                ptcl.tickCount = ptcl.life - 1;
            }
            break;
        case Ptcl::FieldCollisionType::Bounce:
            if (worldPos.y < coord) {
                worldPos.y = coord + 0.0001f;

                Math::Vector3f velocity = Math::MatrixCalc::rotate(matrixSRT, ptcl.velocity);
                velocity.y = -(velocity.y * coef);

                const Math::Matrix34f inverseSRT = Math::MatrixCalc::invert(matrixSRT);

                ptcl.position = Math::MatrixCalc::multiply(inverseSRT, worldPos);
                ptcl.velocity = Math::MatrixCalc::rotate(inverseSRT, velocity);
            }
            break;
        }
    } else {
        switch(res.fieldCollisionType()) {
        case Ptcl::FieldCollisionType::Die:
            if (ptcl.position.y < coord) {
                ptcl.position.y = coord;
                ptcl.tickCount = ptcl.life - 1;
            }
            break;
        case Ptcl::FieldCollisionType::Bounce:
            if (ptcl.position.y < coord) {
                ptcl.position.y = coord;
                ptcl.velocity.y = -(ptcl.velocity.y * coef);
            }
            break;
        }
    }
}

void EmitterCalc::calcFieldConvergence(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;
    const Math::Vector3f& target = res.fieldConvergencePos();
    const f32 rate = res.fieldConvergenceRate();

    ptcl.position.x += (target.x - ptcl.position.x) * rate;
    ptcl.position.y += (target.y - ptcl.position.y) * rate;
    ptcl.position.z += (target.z - ptcl.position.z) * rate;
}

void EmitterCalc::calcFieldPosAdd(EmitterInstance& emitter, PtclInstance& ptcl) {
    const auto& res = emitter.mResource;
    const Math::Vector3f& addPos = res.fieldPosAddPosition();

    ptcl.position.x += addPos.x;
    ptcl.position.y += addPos.y;
    ptcl.position.z += addPos.z;
}


// ========================================================================== //


} // namespace Ptcl::Engine
