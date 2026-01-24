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
#include "math/matrix.h"

#include <QMatrix3x4>
#include <QString>
#include <QVector2D>
#include <QVector3D>

#include <array>


namespace Ptcl {


// ========================================================================== //


struct BasicProperties {
    EmitterType type = EmitterType::Simple;
    FollowType followType = FollowType::All;
    QString name = "Emitter";
    PtclSeed randomSeed{};
};


// ========================================================================== //


struct ScaleProperties {
    Math::Vector2f initScale = {1.0f, 1.0f};
    Math::Vector2f diffScale21 = {0.0f, 0.0f};
    Math::Vector2f diffScale32 = {0.0f, 0.0f};
    s32 scaleSection1 = 0;
    s32 scaleSection2 = 0;
    f32 scaleRand = 0.0f;
};


// ========================================================================== //


struct RotationProperties {
    RotType rotType = RotType::None;
    Math::Vector3i initRot = {};
    Math::Vector3i initRotRand = {};
    Math::Vector3i rotVel = {};
    Math::Vector3i rotVelRand = {};
    Math::Vector2f rotBasis = {0.0f, 0.0f};
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
    ColorCalcType colorCalcType;

    binColor3f color0{255.0f, 255.0f, 255.0f};
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
    Math::Vector3f volumeRadius = {1.0f, 1.0f, 1.0f};
    s32 volumeSweepStart = 0;
    s32 volumeSweepParam = 0;
};


// ========================================================================== //


struct VelocityProperties {
    f32 figureVel = 0.0f;
    Math::Vector3f emitterVelDir = {0.0f, 0.0f, 0.0f};
    f32 initVel = 0.0f;
    f32 initVelRnd = 0.0f;
    Math::Vector3f spreadVec = {0.0f, 0.0f, 0.0f};
    f32 airResistance = 1.0f;
};


// ========================================================================== //


struct EmissionProperties {
    s32 startFrame = 0;
    s32 endFrame = 1;
    s32 lifeStep = 10;
    s32 lifeStepRnd = 1;
    s32 emitRate = 1;
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


struct TransformProperties {
    Math::Matrix34f transformSRT{};
    Math::Matrix34f transformRT{};
};


// ========================================================================== //


struct GravityProperties {
    bool isDirectional = false;
    Math::Vector3f gravity{0.0f, -1.0f, 0.0f};
};


// ========================================================================== //


struct TextureProperties {
    TextureWrap textureWrapT = TextureWrap::ClampToEdge;
    TextureWrap textureWrapS = TextureWrap::ClampToEdge;
    TextureFilter textureMagFilter = TextureFilter::Nearest;
    TextureFilter textureMinFilter = TextureFilter::Nearest;
    TextureMipFilter textureMipFilter = TextureMipFilter::None;

    u16 numTexPat = 0;
    u8 numTexDivX = 1;
    u8 numTexDivY = 1;
    Math::Vector2f texUVScale{1.0f, 1.0f};
    std::array<u8, 16> texPatTbl{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    u16 texPatFreq = 0;
    u16 texPatTblUse = 0;
    bool isTexPatAnim = false;
};


// ========================================================================== //


struct CombinerProperties {
    BlendFuncType blendFunc = BlendFuncType::Translucent;
    DepthFuncType depthFunc = DepthFuncType::Unk0;
    ColorCombinerFuncType combinerFunc = ColorCombinerFuncType::CombinerConfig0;
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

    BitFlag<EmitterFlag>& flags();
    const BitFlag<EmitterFlag> &flags() const;

    const QString& name() const;
    void setName(const QString &name);

    const TextureHandle& textureHandle() const;
    TextureHandle& textureHandle();
    void setTexture(const std::shared_ptr<Texture>& texture);

    u8 _2C() const;
    void set_2C(u8 _2C);

    u8 _2D() const;
    void set_2D(u8 _2D);

    u8 _2E() const;
    void set_2E(u8 _2E);

    u8 _31() const;
    void set_31(u8 _31);

    u32 _80() const;
    void set_80(u32 _80);

    BillboardType billboardType() const;
    void setBillboardType(BillboardType billboardType);

    const BasicProperties &basicProperties() const;
    void setBasicProperties(const BasicProperties &basicProperties);

    const GravityProperties &gravityProperties() const;
    void setGravityProperties(const GravityProperties &gravityProperties);

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

    const TextureProperties &textureProperties() const;
    void setTextureProperties(const TextureProperties &textureProperties);

    const RotationProperties &rotationProperties() const;
    void setRotationProperties(const RotationProperties &rotationProperties);

    const CombinerProperties &combinerProperties() const;
    void setCombinerProperties(const CombinerProperties &combinerProperties);

    const TransformProperties &transformProperties() const;
    void setTransformProperties(const TransformProperties &transformProperties);

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
    BitFlag<EmitterFlag> mFlag;

    u8 m_2C;
    u8 m_2D;
    u8 m_2E;
    u8 m_31;

    u32 m_80;
    BillboardType mBillboardType;

    BasicProperties mBasicProperties;
    GravityProperties mGravityProperties;
    TransformProperties mTransformProperties;
    LifespanProperties mLifespanProperties;
    TerminationProperties mTerminationProperties;
    EmissionProperties mEmissionProperties;
    VelocityProperties mVelocityProperties;
    VolumeProperties mVolumeProperties;
    ColorProperties mColorProperties;
    AlphaProperties mAlphaProperties;
    ScaleProperties mScaleProperties;
    RotationProperties mRotationProperties;
    TextureProperties mTextureProperties;
    CombinerProperties mCombinerProperties;

    TextureHandle mTextureHandle;

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
