#pragma once

#include "typedefs.h"
#include "util/bitflagUtil.h"
#include "ptcl/ptclEnum.h"
#include "math/vector.h"


namespace Ptcl {


struct BinFieldRandomData;
struct BinFieldMagnetData;
struct BinFieldSpinData;
struct BinFieldCollisionData;
struct BinFieldConvergenceData;
struct BinFieldPosAddData;


// ========================================================================== //


class FieldData {
public:
    struct FieldRandomData {
        s32 randomBlank{1};
        Math::Vector3f randomVelAdd{0.0f, 0.0f, 0.0f};
    };

    struct FieldMagnetData {
        f32 magnetPower{0.0f};
        Math::Vector3f magnetPos{0.0f, 0.0f, 0.0f};
        BitFlag<FieldMagnetFlag> magnetFlag{
            FieldMagnetFlag::AxisTargetX,
            FieldMagnetFlag::AxisTargetY,
            FieldMagnetFlag::AxisTargetZ
        };
    };

    struct FieldSpinData {
        s32 spinRotate{0};
        FieldSpinAxis spinAxis{FieldSpinAxis::AxisX};
    };

    struct FieldCollisionData {
        u16 collisionType;
        u16 collisionIsWorld;
        f32 collisionCoord;
        f32 collisionCoef;
    };

    struct FieldConvergenceData {
        FieldConvergenceType convergenceType;
        Math::Vector3f convergencePos;
    };

    struct FieldPosAddData {
        Math::Vector3f posAdd;
    };

public:
    FieldData() = default;

    const FieldRandomData& randomData() const;
    void setRandomData(const FieldRandomData& randomData);
    void initRandomData(const BinFieldRandomData& randomData);

    const FieldMagnetData& magnetData() const;
    void setMagnetData(const FieldMagnetData& magnetData);
    void initMagnetData(const BinFieldMagnetData& magnetData);

    const FieldSpinData& spinData() const;
    void setSpinData(const FieldSpinData& spinData);
    void initSpinData(const BinFieldSpinData& spinData);

    const FieldCollisionData& collisionData() const;
    void setCollisionData(const FieldCollisionData& collisionData);
    void initCollisionData(const BinFieldCollisionData& collisionData);

    const FieldConvergenceData& convergenceData() const;
    void setConvergenceData(const FieldConvergenceData& convergenceData);
    void initConvergenceData(const BinFieldConvergenceData& convergenceData);

    const FieldPosAddData& posAddData() const;
    void setPosAddData(const FieldPosAddData& posAddData);
    void initPosAddData(const BinFieldPosAddData& posAddData);

private:
    FieldRandomData mRandomData;
    FieldMagnetData mMagnetData;
    FieldSpinData mSpinData;
    FieldCollisionData mCollisionData;
    FieldConvergenceData mConvergenceData;
    FieldPosAddData mPosAddData;
};


// ========================================================================== //


} // namespace Ptcl
