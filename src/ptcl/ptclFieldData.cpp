#include "ptcl/ptclFieldData.h"
#include "ptcl/ptclBinary.h"

namespace Ptcl {

const FieldData::FieldRandomData& FieldData::randomData() const {
    return mRandomData;
}

void FieldData::setRandomData(const FieldRandomData& randomData) {
    mRandomData = randomData;
}

void FieldData::initRandomData(const BinFieldRandomData& randomData) {
    mRandomData = {
        .randomBlank = randomData.fieldRandomBlank,
        .randomVelAdd = Math::Vector3f(randomData.fieldRandomVelAdd.x, randomData.fieldRandomVelAdd.y, randomData.fieldRandomVelAdd.z)
    };
}

const FieldData::FieldMagnetData& FieldData::magnetData() const {
    return mMagnetData;
}

void FieldData::setMagnetData(const FieldMagnetData& magnetData) {
    mMagnetData = magnetData;
}

void FieldData::initMagnetData(const BinFieldMagnetData& magnetData) {
    mMagnetData = {
        .magnetPower = magnetData.fieldMagnetPower,
        .magnetPos = Math::Vector3f(magnetData.fieldMagnetPos.x, magnetData.fieldMagnetPos.y, magnetData.fieldMagnetPos.z),
        .magnetFlag = magnetData.fieldMagnetFlag,
    };
}

const FieldData::FieldSpinData& FieldData::spinData() const {
    return mSpinData;
}

void FieldData::setSpinData(const FieldSpinData& spinData) {
    mSpinData = spinData;
}

void FieldData::initSpinData(const BinFieldSpinData& spinData) {
    mSpinData = {
        .spinRotate = spinData.fieldSpinRotate,
        .spinAxis = spinData.fieldSpinAxis
    };
}

const FieldData::FieldCollisionData& FieldData::collisionData() const {
    return mCollisionData;
}

void FieldData::setCollisionData(const FieldCollisionData& collisionData) {
    mCollisionData = collisionData;
}

void FieldData::initCollisionData(const BinFieldCollisionData& collisionData) {
    mCollisionData = {
        .collisionType = collisionData.fieldCollisionType,
        .collisionIsWorld = collisionData.fieldCollisionIsWorld > 0,
        .collisionCoord = collisionData.fieldCollisionCoord,
        .collisionCoef = collisionData.fieldCollisionCoef
    };
}

const FieldData::FieldConvergenceData& FieldData::convergenceData() const {
    return mConvergenceData;
}

void FieldData::setConvergenceData(const FieldConvergenceData& convergenceData) {
    mConvergenceData = convergenceData;
}

void FieldData::initConvergenceData(const BinFieldConvergenceData& convergenceData) {
    mConvergenceData = {
        .convergenceType = convergenceData.fieldConvergenceType,
        .convergencePos = Math::Vector3f(convergenceData.fieldConvergencePos.x, convergenceData.fieldConvergencePos.y, convergenceData.fieldConvergencePos.z)
    };
}

const FieldData::FieldPosAddData& FieldData::posAddData() const {
    return mPosAddData;
}

void FieldData::setPosAddData(const FieldPosAddData& posAddData) {
    mPosAddData = posAddData;
}

void FieldData::initPosAddData(const BinFieldPosAddData& posAddData) {
    mPosAddData = {
        .posAdd = Math::Vector3f(posAddData.fieldPosAdd.x, posAddData.fieldPosAdd.y, posAddData.fieldPosAdd.z)
    };
}


// ========================================================================== //


} // namespace Ptcl
