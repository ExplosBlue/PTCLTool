#pragma once

#include "ptcl/ptclBinary.h"
#include "ptcl/ptclChildData.h"
#include "ptcl/ptclEnum.h"
#include "ptcl/ptclFieldData.h"
#include "ptcl/ptclFluctuationData.h"
#include "ptcl/ptclStripeData.h"
#include "ptcl/ptclSeed.h"
#include "ptcl/ptclTexture.h"
#include "typedefs.h"
#include "util/bitflagUtil.h"

#include <QMatrix3x4>
#include <QString>
#include <QVector2D>
#include <QVector3D>

#include <array>


namespace Ptcl {


// ========================================================================== //


struct ScaleProperties {
    QVector2D initScale = {1.0f, 1.0f};
    QVector2D diffScale21 = {0.0f, 0.0f};
    QVector2D diffScale32 = {0.0f, 0.0f};
    s32 scaleSection1 = 0;
    s32 scaleSection2 = 0;
    f32 scaleRand = 0.0f;
};


// ========================================================================== //


struct RotationProperties {
    RotType rotType = RotType::None;
    binVec3i initRot = {};
    binVec3i initRotRand = {};
    binVec3i rotVel = {};
    binVec3i rotVelRand = {};
};


// ========================================================================== //


struct ColorProperties {
    std::array<binColor4f, 3> colors {
        binColor4f{0.0f, 0.0f, 0.0f, 1.0f},
        binColor4f{0.0f, 0.0f, 0.0f, 1.0f},
        binColor4f{0.0f, 0.0f, 0.0f, 1.0f},
    };
    s32 colorSection1 = 20;
    s32 colorSection2 = 60;
    s32 colorSection3 = 80;
    s32 colorNumRepeat = 1;

    bool colorRandom = false;
    bool colorAnimation = false;
};


// ========================================================================== //


struct AlphaProperties {
    f32 initAlpha = 0.0f;
    f32 diffAlpha21 = 0.0f;
    f32 diffAlpha32 = 0.0f;
    s32 alphaSection1 = 0;
    s32 alphaSection2 = 0;
};


// ========================================================================== //


struct VolumeProperties {
    u8 volumeTblIndex = 0;
    VolumeType volumeType = VolumeType::Point;
    QVector3D volumeRadius = {1.0f, 1.0f, 1.0f};
    s32 volumeSweepStart = 0;
    s32 volumeSweepParam = 0;
};


// ========================================================================== //


struct VelocityProperties {
    f32 figureVel = 0.0f;
    QVector3D emitterVelDir = {0.0f, 0.0f, 0.0f};
    f32 initVel = 0.0f;
    f32 initVelRnd = 0.0f;
    QVector3D spreadVec = {0.0f, 0.0f, 0.0f};
};


// ========================================================================== //


struct EmissionProperties {
    s32 startFrame = 0;
    s32 endFrame = 1;
    s32 lifeStep = 10;
    s32 lifeStepRnd = 1;
};


// ========================================================================== //


struct TerminationProperties {
    bool isStopEmitInFade = false;
    f32 alphaAddInFade = 0.0f;
};


// ========================================================================== //


struct LifespanProperties {
    s32 ptclLife = 100;
    s32 ptclLifeRnd = 0;
};


// ========================================================================== //


class Emitter
{
public:
    void extracted();
    Emitter();

    Emitter(const Emitter&) = delete;
    Emitter &operator=(const Emitter&) = delete;

    EmitterType type() const;
    void setType(EmitterType type);

    BitFlag<EmitterFlag>& flags();
    const BitFlag<EmitterFlag> &flags() const;

    PtclSeed &randomSeed();
    const PtclSeed &randomSeed() const;

    const QString& name() const;
    void setName(const QString &name);

    const TextureHandle& textureHandle() const;
    TextureHandle& textureHandle();
    void setTexture(std::shared_ptr<Texture> texture);

    TextureWrap textureWrapT() const;
    void setTextureWrapT(TextureWrap wrap);

    TextureWrap textureWrapS() const;
    void setTextureWrapS(TextureWrap wrap);

    TextureFilter textureMagFilter() const;
    void setTextureMagFilter(TextureFilter filter);

    TextureFilter textureMinFilter() const;
    void setTextureMinFilter(TextureFilter filter);

    TextureMipFilter textureMipFilter() const;
    void setTextureMipFilter(TextureMipFilter filter);

    u8 _2C() const;
    void set_2C(u8 _2C);

    u8 _2D() const;
    void set_2D(u8 _2D);

    u8 _2E() const;
    void set_2E(u8 _2E);

    u8 _2F() const;
    void set_2F(u8 _2F);

    u8 _30() const;
    void set_30(u8 _30);

    u8 _31() const;
    void set_31(u8 _31);

    u32 _80() const;
    void set_80(u32 _80);

    f32 _8C() const;
    void set_8C(f32 _8C);

    u32 _90() const;
    void set_90(u32 _90);

    BillboardType billboardType() const;
    void setBillboardType(BillboardType billboardType);

    u32 _98() const;
    void set_98(u32 _98);

    f32 _9C() const;
    void set_9C(f32 _9C);

    f32 _A0() const;
    void set_A0(f32 _A0);

    f32 _A4() const;
    void set_A4(f32 _A4);

    const std::array<u32, 3> &_D8() const;
    void set_D8(const std::array<u32, 3> &_D8);

    const LifespanProperties &lifespanProperties() const;
    void setLifespanProperties(const LifespanProperties &lifespanProperties);

    const TerminationProperties &terminationProperties() const;
    void setTerminationProperties(const TerminationProperties &terminationProperties);

    const EmissionProperties &emissionProperties() const;
    void setEmissionProperties(const EmissionProperties &emissionProperties);

    const VelocityProperties &velocityProperties() const;
    void setVelocityProperties(const VelocityProperties &velocityProperties);

    const VolumeProperties &volumeProperties() const;
    void setVolumeProperties(const VolumeProperties &volumeProperties);

    const ColorProperties &colorProperties() const;
    void setColorProperties(const ColorProperties &colorProperties);

    const AlphaProperties &alphaProperties() const;
    void setAlphaProperties(const AlphaProperties &alphaProperties);

    const ScaleProperties &scaleProperties() const;
    void setScaleProperties(const ScaleProperties &scaleProperties);

    CombinerType combinerType() const;
    void setCombinerType(CombinerType combinerType);

    FollowType followType() const;
    void setFollowType(FollowType followType);

    u32 _134() const;
    void set_134(u32 _134);

    const RotationProperties &rotationProperties() const;
    void setRotationProperties(const RotationProperties &rotationProperties);

    const std::array<u32, 2> &_168() const;
    void set_168(const std::array<u32, 2> &_168);

    const QMatrix3x4 &transformSRT() const;
    void setTransformSRT(const QMatrix3x4 &transformSRT);

    const QMatrix3x4 &transformRT() const;
    void setTransformRT(const QMatrix3x4 &transformRT);

    u16 numTexPat() const;
    void setNumTexPat(u16 numTexPat);

    u8 numTexDivX() const;
    void setNumTexDivX(u8 numTexDivX);

    u8 numTexDivY() const;
    void setNumTexDivY(u8 numTexDivY);

    const QVector2D &texUVScale() const;
    void setTexUVScale(const QVector2D &texUVScale);

    const std::array<u8, 16> &texPatTbl() const;
    void setTexPatTbl(const std::array<u8, 16> &texPatTbl);
    void setTexPatTblData(int index, u8 value);

    u16 texPatFreq() const;
    void setTexPatFreq(u16 texPatFreq);

    u16 texPatTblUse() const;
    void setTexPatTblUse(u16 texPatTblUse);

    BitFlag<ChildFlag> &childFlags();
    BitFlag<FieldFlag> &fieldFlags();
    BitFlag<FluctuationFlag> &fluctuationFlags();
    BitFlag<StripeFlag> &stripeFlags();

    const BitFlag<ChildFlag> &childFlags() const;
    const BitFlag<FieldFlag> &fieldFlags() const;
    const BitFlag<FluctuationFlag> &fluctuationFlags() const;
    const BitFlag<StripeFlag> &stripeFlags() const;

    bool hasStripeData() const;
    void setHasStripeData(bool hasStripeData);

    ChildData &childData();

    FieldRandomData &fieldRandomData();
    FieldMagnetData &fieldMagnetData();
    FieldSpinData &fieldSpinData();
    FieldCollisionData &fieldCollisionData();
    FieldConvergenceData &fieldConvergenceData();
    FieldPosAddData &fieldPosAddData();

    FluctuationData &fluctuationData();

    StripeData &stripeData();

    void initFromBinary(const BinCommonEmitterData &emitterData);

private:
    EmitterType mType;
    BitFlag<EmitterFlag> mFlag;

    PtclSeed mRandomSeed;

    QString mName; // namePos, name

    TextureHandle mTextureHande;
    TextureWrap mTextureWrapT;
    TextureWrap mTextureWrapS;
    TextureFilter mTextureMagFilter;
    TextureFilter mTextureMinFilter;
    TextureMipFilter mTextureMipFilter;

    u8 m_2C;
    u8 m_2D;
    u8 m_2E;
    u8 m_2F;
    u8 m_30;
    u8 m_31;


    u32 m_80;
    f32 m_8C;
    u32 m_90;
    BillboardType mBillboardType;
    u32 m_98;
    f32 m_9C;
    f32 m_A0;
    f32 m_A4;

    std::array<u32, 3> m_D8;

    LifespanProperties mLifespanProperties;
    TerminationProperties mTerminationProperties;
    EmissionProperties mEmissionProperties;
    VelocityProperties mVelocityProperties;
    VolumeProperties mVolumeProperties;
    ColorProperties mColorProperties;
    AlphaProperties mAlphaProperties;
    ScaleProperties mScaleProperties;
    RotationProperties mRotationProperties;

    CombinerType mCombinerType;

    FollowType mFollowType;
    u32 m_134;

    std::array<u32, 2> m_168;
    QMatrix3x4 mTransformSRT;
    QMatrix3x4 mTransformRT;

    u16 mNumTexPat;
    u8 mNumTexDivX;
    u8 mNumTexDivY;
    QVector2D mTexUVScale;
    std::array<u8, 16> mTexPatTbl;
    u16 mTexPatFreq;
    u16 mTexPatTblUse;

    // Complex / UnkType fields
    BitFlag<ChildFlag> mChildFlags;
    BitFlag<FieldFlag> mFieldFlags;
    BitFlag<FluctuationFlag> mFluctuationFlags;
    BitFlag<StripeFlag> mStripeFlags;

    bool mHasStripeData;

    ChildData mChildData;

    FieldRandomData mFieldRandomData;
    FieldMagnetData mFieldMagnetData;
    FieldSpinData mFieldSpinData;
    FieldCollisionData mFieldCollisionData;
    FieldConvergenceData mFieldConvergenceData;
    FieldPosAddData mFieldPosAddData;

    FluctuationData mFluctuationData;

    StripeData mStripeData;
};


// ========================================================================== //


} // namespace Ptcl
