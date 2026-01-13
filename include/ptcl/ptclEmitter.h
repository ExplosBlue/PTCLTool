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


class Emitter
{
public:
    Emitter();

    Emitter(const Emitter&) = delete;
    Emitter& operator=(const Emitter&) = delete;

    EmitterType type() const;
    void        setType(EmitterType type);

    BitFlag<EmitterFlag>& flags();
    const BitFlag<EmitterFlag>& flags() const;

    PtclSeed& randomSeed();
    const PtclSeed& randomSeed() const;

    const QString& name() const;
    void setName(const QString& name);

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

    u8 volumeTblIndex() const;
    void setVolumeTblIndex(u8 volumeTblIndex);

    u8 _33() const;
    void set_33(u8 _33);

    VolumeType volumeType() const;
    void setVolumeType(VolumeType volumeType);

    const QVector3D& volumeRadius() const;
    void setVolumeRadius(const QVector3D& volumeRadius);

    s32 volumeSweepStart() const;
    void setVolumeSweepStart(s32 volumeSweepStart);

    u32 volumeSweepParam() const;
    void setVolumeSweepParam(u32 volumeSweepParam);

    f32 figureVel() const;
    void setFigureVel(f32 figureVel);

    const QVector3D& emitterVelDir() const;
    void setEmitterVelDir(const QVector3D& emitterVelDir);

    f32 _5C() const;
    void set_5C(f32 _5C);

    f32 initVelRnd() const;
    void setInitVelRnd(f32 initVelRnd);

    const QVector3D& spreadVec() const;
    void setSpreadVec(const QVector3D& spreadVec);

    const std::array<u32, 4>& _70() const;
    void set_70(const std::array<u32, 4>& _70);

    u32 _80() const;
    void set_80(u32 _80);

    s32 ptclLife() const;
    void setPtclLife(s32 ptclLife);

    s32 ptclLifeRnd() const;
    void setPtclLifeRnd(s32 ptclLifeRnd);

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

    const std::array<binColor4f, 3>& colors() const;
    void setColors(const std::array<binColor4f, 3>& colors);
    void setColor(int index, const binColor4f& color);

    const std::array<u32, 3>& _D8() const;
    void set_D8(const std::array<u32, 3>& _D8);

    s32 colorSection1() const;
    void setColorSection1(s32 colorSection1);

    s32 colorSection2() const;
    void setColorSection2(s32 colorSection2);

    s32 colorSection3() const;
    void setColorSection3(s32 colorSection3);

    s32 colorNumRepeat() const;
    void setColorNumRepeat(s32 colorNumRepeat);

    f32 initAlpha() const;
    void setInitAlpha(f32 initAlpha);

    f32 diffAlpha21() const;
    void setDiffAlpha21(f32 diffAlpha21);

    f32 diffAlpha32() const;
    void setDiffAlpha32(f32 diffAlpha32);

    s32 alphaSection1() const;
    void setAlphaSection1(s32 alphaSection1);

    s32 alphaSection2() const;
    void setAlphaSection2(s32 alphaSection2);

    const ScaleProperties& scaleProperties() const;
    void setScaleProperties(const ScaleProperties& scaleProperties);

    // const QVector2D& initScale() const;
    // void setInitScale(const QVector2D& initScale);

    // const QVector2D& diffScale21() const;
    // void setDiffScale21(const QVector2D& diffScale21);

    // const QVector2D& diffScale32() const;
    // void setDiffScale32(const QVector2D& diffScale32);

    // s32 scaleSection1() const;
    // void setScaleSection1(s32 scaleSection1);

    // s32 scaleSection2() const;
    // void setScaleSection2(s32 scaleSection2);

    // f32 scaleRand() const;
    // void setScaleRand(f32 scaleRand);

    RotType rotType() const;
    void setRotType(RotType rotType);

    CombinerType combinerType() const;
    void setCombinerType(CombinerType combinerType);

    FollowType followType() const;
    void setFollowType(FollowType followType);

    u32 _134() const;
    void set_134(u32 _134);

    const binVec3i& initRot() const;
    void setInitRot(const binVec3i& initRot);

    const binVec3i& initRotRand() const;
    void setInitRotRand(const binVec3i& initRotRand);

    const binVec3i& rotVel() const;
    void setRotVel(const binVec3i& rotVel);

    const binVec3i& rotVelRand() const;
    void setRotVelRand(const binVec3i& rotVelRand);

    const std::array<u32, 2>& _168() const;
    void set_168(const std::array<u32, 2>& _168);

    const QMatrix3x4& transformSRT() const;
    void setTransformSRT(const QMatrix3x4& transformSRT);

    const QMatrix3x4& transformRT() const;
    void setTransformRT(const QMatrix3x4& transformRT);

    f32 alphaAddInFade() const;
    void setAlphaAddInFade(f32 alphaAddInFade);

    u16 numTexPat() const;
    void setNumTexPat(u16 numTexPat);

    u8 numTexDivX() const;
    void setNumTexDivX(u8 numTexDivX);

    u8 numTexDivY() const;
    void setNumTexDivY(u8 numTexDivY);

    const QVector2D& texUVScale() const;
    void setTexUVScale(const QVector2D& texUVScale);

    const std::array<u8, 16>& texPatTbl() const;
    void setTexPatTbl(const std::array<u8, 16>& texPatTbl);
    void setTexPatTblData(int index, u8 value);

    u16 texPatFreq() const;
    void setTexPatFreq(u16 texPatFreq);

    u16 texPatTblUse() const;
    void setTexPatTblUse(u16 texPatTblUse);

    BitFlag<ChildFlag>& childFlags();
    BitFlag<FieldFlag>& fieldFlags();
    BitFlag<FluctuationFlag>& fluctuationFlags();
    BitFlag<StripeFlag>& stripeFlags();

    const BitFlag<ChildFlag>& childFlags() const;
    const BitFlag<FieldFlag>& fieldFlags() const;
    const BitFlag<FluctuationFlag>& fluctuationFlags() const;
    const BitFlag<StripeFlag>& stripeFlags() const;

    bool hasStripeData() const;
    void setHasStripeData(bool hasStripeData);

    ChildData& childData();

    FieldRandomData& fieldRandomData();
    FieldMagnetData& fieldMagnetData();
    FieldSpinData& fieldSpinData();
    FieldCollisionData& fieldCollisionData();
    FieldConvergenceData& fieldConvergenceData();
    FieldPosAddData& fieldPosAddData();

    FluctuationData& fluctuationData();

    StripeData& stripeData();

    void initFromBinary(const BinCommonEmitterData& emitterData);

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
    u8 mVolumeTblIndex;
    u8 m_33; // probably padding...
    VolumeType mVolumeType;
    QVector3D mVolumeRadius;
    s32 mVolumeSweepStart;
    u32 mVolumeSweepParam;
    f32 mFigureVel;
    QVector3D mEmitterVelDir;
    f32 m_5C;
    f32 mInitVelRnd;
    QVector3D mSpreadVec;
    std::array<u32, 4> m_70;
    u32 m_80;
    s32 mPtclLife;
    u32 mPtclLifeRnd;
    f32 m_8C;
    u32 m_90;
    BillboardType mBillboardType;
    u32 m_98;
    f32 m_9C;
    f32 m_A0;
    f32 m_A4;
    std::array<binColor4f, 3> mColors;
    std::array<u32, 3> m_D8;
    s32 mColorSection1; // Should these be QColor?
    s32 mColorSection2; // Should these be QColor?
    s32 mColorSection3; // Should these be QColor?
    s32 mColorNumRepeat;
    f32 mInitAlpha;
    f32 mDiffAlpha21;
    f32 mDiffAlpha32;
    s32 mAlphaSection1;
    s32 mAlphaSection2;

    ScaleProperties mScaleProperties;

    CombinerType mCombinerType;
    RotType mRotType;
    FollowType mFollowType;
    u32 m_134;
    binVec3i mInitRot;
    binVec3i mInitRotRand;
    binVec3i mRotVel;
    binVec3i mRotVelRand;
    std::array<u32, 2> m_168;
    QMatrix3x4 mTransformSRT;
    QMatrix3x4 mTransformRT;
    f32 mAlphaAddInFade;

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
    BitFlag<StripeFlag> mStripeFlags; // I think this is unused

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
