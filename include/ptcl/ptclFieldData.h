#pragma once

#include "typedefs.h"
#include "ptcl/ptclBinary.h"
#include "ptcl/ptclEnum.h"

#include <QVector3D>


namespace Ptcl {


// ========================================================================== //


class FieldRandomData
{
public:
    FieldRandomData() = default;

    FieldRandomData(const FieldRandomData&) = delete;
    FieldRandomData& operator=(const FieldRandomData&) = delete;

    s32 randomBlank() const;
    void setRandomBlank(s32 randomBlank);

    const QVector3D& randomVelAdd() const;
    void setRandomVelAdd(const QVector3D& randomVelAdd);

    void initFromBinary(const BinFieldRandomData& fieldRandomData);

private:
    s32 mRandomBlank;
    QVector3D mRandomVelAdd;
};


// ========================================================================== //


class FieldMagnetData
{
public:
    FieldMagnetData() = default;

    FieldMagnetData(const FieldMagnetData&) = delete;
    FieldMagnetData& operator=(const FieldMagnetData&) = delete;

    f32 magnetPower() const;
    void setMagnetPower(f32 magnetPower);

    const QVector3D& magnetPos() const;
    void setMagnetPos(const QVector3D& magnetPos);

    u32 magnetFlag() const;
    void setMagnetFlag(u32 magnetFlag);

    void initFromBinary(const BinFieldMagnetData& fieldMagnetData);

private:
    f32 mMagnetPower;
    QVector3D mMagnetPos;
    u32 mMagnetFlag;
};


// ========================================================================== //


class FieldSpinData
{
public:
    FieldSpinData() = default;

    FieldSpinData(const FieldSpinData&) = delete;
    FieldSpinData& operator=(const FieldSpinData&) = delete;

    s32 spinRotate() const;
    void setSpinRotate(s32 spinRotate);

    s32 spinAxis() const;
    void setSpinAxis(s32 spinAxis);

    void initFromBinary(const BinFieldSpinData& fieldSpinData);

private:
    s32 mSpinRotate;
    s32 mSpinAxis;
};


// ========================================================================== //


class FieldCollisionData
{
public:
    FieldCollisionData() = default;

    FieldCollisionData(const FieldCollisionData&) = delete;
    FieldCollisionData& operator=(const FieldCollisionData&) = delete;

    u16 collisionType() const;
    void setCollisionType(u16 collisionType);

    u16 collisionIsWorld() const;
    void setCollisionIsWorld(u16 collisionIsWorld);

    f32 collisionCoord() const;
    void setCollisionCoord(f32 collisionCoord);

    f32 collisionCoef() const;
    void setCollisionCoef(f32 collisionCoef);

    void initFromBinary(const BinFieldCollisionData& fieldCollisionData);

private:
    u16 mCollisionType;
    u16 mCollisionIsWorld;
    f32 mCollisionCoord;
    f32 mCollisionCoef;
};


// ========================================================================== //


class FieldConvergenceData
{
public:
    FieldConvergenceData() = default;

    FieldConvergenceData(const FieldConvergenceData&) = delete;
    FieldConvergenceData& operator=(const FieldConvergenceData&) = delete;

    FieldConvergenceType convergenceType() const;
    void                 setConvergenceType(FieldConvergenceType convergenceType);

    const QVector3D& convergencePos() const;
    void setConvergencePos(const QVector3D& convergencePos);

    void initFromBinary(const BinFieldConvergenceData& fieldConvergenceData);

private:
    FieldConvergenceType mConvergenceType;
    QVector3D mConvergencePos;
};


// ========================================================================== //

class FieldPosAddData
{
public:
    FieldPosAddData() = default;

    FieldPosAddData(const FieldPosAddData&) = delete;
    FieldPosAddData& operator=(const FieldPosAddData&) = delete;

    const QVector3D& posAdd() const;
    void setPosAdd(const QVector3D& posAdd);

    void initFromBinary(const BinFieldPosAddData& fieldPosAddData);

private:
    QVector3D mPosAdd;
};


// ========================================================================== //


} // namespace Ptcl
