#include "ptcl/ptclFieldData.h"


namespace Ptcl {


// ========================================================================== //


s32 FieldRandomData::randomBlank() const {
    return mRandomBlank;
}

void FieldRandomData::setRandomBlank(const s32 randomBlank) {
    mRandomBlank = randomBlank;
}

const QVector3D& FieldRandomData::randomVelAdd() const {
    return mRandomVelAdd;
}

void FieldRandomData::setRandomVelAdd(const QVector3D& randomVelAdd) {
    mRandomVelAdd = randomVelAdd;
}

void FieldRandomData::initFromBinary(const BinFieldRandomData& fieldRandomData) {
    mRandomBlank = fieldRandomData.fieldRandomBlank;
    mRandomVelAdd = QVector3D(fieldRandomData.fieldRandomVelAdd.x, fieldRandomData.fieldRandomVelAdd.y, fieldRandomData.fieldRandomVelAdd.z);
}


// ========================================================================== //


f32 FieldMagnetData::magnetPower() const {
    return mMagnetPower;
}

void FieldMagnetData::setMagnetPower(const f32 magnetPower) {
    mMagnetPower = magnetPower;
}

const QVector3D& FieldMagnetData::magnetPos() const {
    return mMagnetPos;
}

void FieldMagnetData::setMagnetPos(const QVector3D& magnetPos) {
    mMagnetPos = magnetPos;
}

u32 FieldMagnetData::magnetFlag() const {
    return mMagnetFlag;
}

void FieldMagnetData::setMagnetFlag(const u32 magnetFlag) {
    mMagnetFlag = magnetFlag;
}

void FieldMagnetData::initFromBinary(const BinFieldMagnetData& fieldMagnetData) {
    mMagnetPower = fieldMagnetData.fieldMagnetPower;
    mMagnetPos = QVector3D(fieldMagnetData.fieldMagnetPos.x, fieldMagnetData.fieldMagnetPos.y, fieldMagnetData.fieldMagnetPos.z);
    mMagnetFlag = fieldMagnetData.fieldMagnetFlag;
}


// ========================================================================== //


s32 FieldSpinData::spinRotate() const {
    return mSpinRotate;
}

void FieldSpinData::setSpinRotate(const s32 spinRotate) {
    mSpinRotate = spinRotate;
}

s32 FieldSpinData::spinAxis() const {
    return mSpinAxis;
}

void FieldSpinData::setSpinAxis(const s32 spinAxis) {
    mSpinAxis = spinAxis;
}

void FieldSpinData::initFromBinary(const BinFieldSpinData& fieldSpinData) {
    mSpinRotate = fieldSpinData.fieldSpinRotate;
    mSpinAxis = fieldSpinData.fieldSpinAxis;
}


// ========================================================================== //


u16 FieldCollisionData::collisionType() const {
    return mCollisionType;
}

void FieldCollisionData::setCollisionType(const u16 collisionType) {
    mCollisionType = collisionType;
}

u16 FieldCollisionData::collisionIsWorld() const {
    return mCollisionIsWorld;
}

void FieldCollisionData::setCollisionIsWorld(const u16 collisionIsWorld) {
    mCollisionIsWorld = collisionIsWorld;
}

f32 FieldCollisionData::collisionCoord() const {
    return mCollisionCoord;
}

void FieldCollisionData::setCollisionCoord(const f32 collisionCoord) {
    mCollisionCoord = collisionCoord;
}

f32 FieldCollisionData::collisionCoef() const {
    return mCollisionCoef;
}

void FieldCollisionData::setCollisionCoef(const f32 collisionCoef) {
    mCollisionCoef = collisionCoef;
}

void FieldCollisionData::initFromBinary(const BinFieldCollisionData& fieldCollisionData) {
    mCollisionType = fieldCollisionData.fieldCollisionType;
    mCollisionIsWorld = fieldCollisionData.fieldCollisionIsWorld;
    mCollisionCoord = fieldCollisionData.fieldCollisionCoord;
    mCollisionCoef = fieldCollisionData.fieldCollisionCoef;
}


// ========================================================================== //


FieldConvergenceType FieldConvergenceData::convergenceType() const {
    return mConvergenceType;
}

void FieldConvergenceData::setConvergenceType(const FieldConvergenceType convergenceType) {
    mConvergenceType = convergenceType;
}

const QVector3D& FieldConvergenceData::convergencePos() const {
    return mConvergencePos;
}

void FieldConvergenceData::setConvergencePos(const QVector3D& convergencePos) {
    mConvergencePos = convergencePos;
}

void FieldConvergenceData::initFromBinary(const BinFieldConvergenceData& fieldConvergenceData) {
    mConvergenceType = fieldConvergenceData.fieldConvergenceType;
    mConvergencePos = QVector3D(fieldConvergenceData.fieldConvergencePos.x, fieldConvergenceData.fieldConvergencePos.y, fieldConvergenceData.fieldConvergencePos.z);
}


// ========================================================================== //


const QVector3D& FieldPosAddData::posAdd() const {
    return mPosAdd;
}

void FieldPosAddData::setPosAdd(const QVector3D& posAdd) {
    mPosAdd = posAdd;
}

void FieldPosAddData::initFromBinary(const BinFieldPosAddData& fieldPosAddData) {
    mPosAdd = QVector3D(fieldPosAddData.fieldPosAdd.x, fieldPosAddData.fieldPosAdd.y, fieldPosAddData.fieldPosAdd.z);
}


// ========================================================================== //


} // namespace Ptcl
