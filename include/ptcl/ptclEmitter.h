#pragma once

#include "math/matrix.h"
#include "math/util.h"

#include "ptcl/ptclBinary.h"
#include "ptcl/ptclChildData.h"
#include "ptcl/ptclEnum.h"
#include "ptcl/ptclFieldData.h"
#include "ptclFluctuationData.h"
#include "ptcl/ptclStripeData.h"
#include "ptcl/ptclSeed.h"
#include "ptcl/ptclTexture.h"

#include "typedefs.h"

#include "util/bitflagUtil.h"

#include <QString>

#include <array>


namespace Ptcl {


// ========================================================================== //


class Emitter {

public:
    struct ScaleAnim {
        Math::Vector2f initScale{1.0f, 1.0f};
        Math::Vector2f diffScale21{0.0f, 0.0f};
        Math::Vector2f diffScale32{0.0f, 0.0f};
        s32 scaleSection1{0};
        s32 scaleSection2{0};

        bool operator==(const ScaleAnim&) const = default;
    };

    struct ColorProperties {
        std::array<binColor4f, 3> color0{
            binColor4f{1.0f, 1.0f, 1.0f, 1.0f},
            binColor4f{1.0f, 1.0f, 1.0f, 1.0f},
            binColor4f{1.0f, 1.0f, 1.0f, 1.0f},
        };
        s32 colorSection1{20};
        s32 colorSection2{60};
        s32 colorSection3{80};
        s32 colorNumRepeat{1};

        bool colorRandom{false};
        bool colorAnimation{false};
        ColorCalcType colorCalcType{ColorCalcType::None};

        binColor3f color1{255.0f, 255.0f, 255.0f};
    };

    struct AlphaAnim {
        f32 initAlpha{1.0f};
        f32 diffAlpha21{0.0f};
        f32 diffAlpha32{0.0f};
        s32 alphaSection1{0};
        s32 alphaSection2{100};

        bool operator==(const AlphaAnim&) const = default;
    };

    struct TextureProperties {
        TextureWrap textureWrapT{TextureWrap::ClampToEdge};
        TextureWrap textureWrapS{TextureWrap::ClampToEdge};
        TextureFilter textureMagFilter{TextureFilter::Nearest};
        TextureFilter textureMinFilter{TextureFilter::Nearest};
        TextureMipFilter textureMipFilter{TextureMipFilter::None};

        u16 numTexPat{1};
        u8 numTexDivX{1};
        u8 numTexDivY{1};
        Math::Vector2f texUVScale{1.0f, 1.0f};
        std::array<u8, 16> texPatTbl{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        u16 texPatFreq{0};
        u16 texPatTblUse{2};
        bool isTexPatAnim{false};
    };

    struct ComplexProperties {
        BitFlag<ChildFlag> childFlags{
            ChildFlag::AlphaInherit,
            ChildFlag::ScaleInherit,
            ChildFlag::RotateInherit,
            ChildFlag::IsFollow,
            ChildFlag::Unk80
        };
        BitFlag<FieldFlag> fieldFlags{};
        BitFlag<FluctuationFlag> fluctuationFlags{
            FluctuationFlag::ApplyAlpha,
            FluctuationFlag::ApplyScale
        };
        BitFlag<StripeFlag> stripeFlags{};
    };

public:
    Emitter() = default;

    Emitter(const Emitter&) = delete;
    Emitter& operator=(const Emitter&) = delete;

    std::unique_ptr<Emitter> clone() const;

    // ----- Basic Properties ----- \\

    EmitterType type() const { return mType; }
    void setType(EmitterType type) { mType = type; }

    FollowType followType() const { return mFollowType; }
    void setFollowType(FollowType type) {
        mFollowType = type;

        // TODO: Check if this should also be set for Ptcl::FollowType::PosOnly
        mIsFollow = (type == Ptcl::FollowType::All);
    }

    const QString& name() const { return mName; }
    void setName(const QString& name) { mName = name; }

    const PtclSeed& randomSeed() const { return mRandomSeed; }
    void setRandomSeed(PtclSeed seed) { mRandomSeed = seed; }

    BillboardType billboardType() const { return mBillboardType; }
    void setBillboardType(BillboardType type) {
        mBillboardType = type;
        mIsPolygon = (type == Ptcl::BillboardType::PolygonXY || type == Ptcl::BillboardType::PolygonXZ);
        mIsVelLook = (type == Ptcl::BillboardType::VelLook || type == Ptcl::BillboardType::VelLookPolygon);
    }

    bool isEmitterBillboardMtx() const { return mIsEmitterBillboardMtx; }
    void setIsEmitterBillboardMtx(bool isEmitterBillboardMtx) { mIsEmitterBillboardMtx = isEmitterBillboardMtx; }

    bool isPolygon() const { return mIsPolygon; }
    bool isFollow() const { return mIsFollow; }
    bool isVelLook() const { return mIsVelLook; }

    // ----- Gravity Properties ----- \\

    bool isDirectional() const { return mIsDirectional; }
    void setDirectional(bool isDirectional) { mIsDirectional = isDirectional; }

    const Math::Vector3f& gravity() const { return mGravity; }
    void setGravity(const Math::Vector3f& gravity) { mGravity = gravity; }

    // ----- Lifespan Properties ----- \\

    s32 ptclLife() const { return mPtclLife; }
    void setPtclLife(const s32 ptclLife) { mPtclLife = ptclLife; }

    s32 ptclLifeRandom() const { return mPtclLifeRnd; }
    void setPtclLifeRandom(const s32 lifeRandom) { mPtclLifeRnd = lifeRandom; }

    // ----- Termination Properties ----- \\

    bool isStopEmitInFade() const { return mIsStopEmitInFade; }
    void setIsStopEmitInFade(bool isStop) { mIsStopEmitInFade = isStop; }

    f32 alphaAddInFade() const { return mAlphaAddInFade; }
    void setAlphaAddInFade(f32 alpha) { mAlphaAddInFade = alpha; }

    // ----- Transform Properties ----- \\

    const Math::Matrix34f& transformRT() const { return mTransformRT; }
    const Math::Matrix34f& transformSRT() const { return mTransformSRT; }

    void setTransform(const Math::Vector3f& rotation, const Math::Vector3f& translation, const Math::Vector3f& scale) {
        const auto mtxR = Math::Util::eulerToRotationMatrix(rotation);

        Math::Matrix34f mtxRT;
        for (s32 r = 0; r < 3; ++r) {
            for (s32 c = 0; c < 3; ++c) {
                mtxRT(r, c) = mtxR(r, c);
            }
            mtxRT(r, 3) = translation[r];
        }

        mTransformRT = mtxRT;

        Math::Matrix34f mtxSRT;
        for (s32 r = 0; r < 3; ++r) {
            mtxSRT(r, 0) = mtxRT(r, 0) * scale.getX();
            mtxSRT(r, 1) = mtxRT(r, 1) * scale.getY();
            mtxSRT(r, 2) = mtxRT(r, 2) * scale.getZ();
            mtxSRT(r, 3) = mtxRT(r, 3);
        }

        mTransformSRT = mtxSRT;
    }

    Math::Vector3f translation() const { return Math::Util::getTranslation(mTransformRT); }
    void setTranslation(const Math::Vector3f& translation) { setTransform(rotation(), translation, scale()); }

    Math::Vector3f rotation() const {
        auto rot = Math::Util::getRotationEuler(mTransformRT);
        rot.setX(Math::Util::to180(rot.getX()));
        rot.setY(Math::Util::to180(rot.getY()));
        rot.setZ(Math::Util::to180(rot.getZ()));
        return rot;
    }
    void setRotation(const Math::Vector3f& rotation) { setTransform(rotation, translation(), scale()); }

    Math::Vector3f scale() const { return Math::Util::getScale(mTransformSRT); }
    void setScale(const Math::Vector3f& scale) { setTransform(rotation(), translation(), scale); }

    // ----- Scale Properties ----- \\

    const ScaleAnim& scaleAnim() const { return mScaleAnim; }
    void setScaleAnim(const ScaleAnim& scaleAnim) { mScaleAnim = scaleAnim; }

    f32 scaleRand() const { return mScaleRand; }
    void setScaleRand(f32 scaleRand) { mScaleRand = scaleRand; }

    // ----- Emission Properties ----- \\

    s32 emitStartFrame() const { return mEmitStartFrame; }
    void setEmitStartFrame(s32 startFrame) { mEmitStartFrame = startFrame; }

    s32 emitEndFrame() const { return mEmitEndFrame; }
    void setEmitEndFrame(s32 endFrame) { mEmitEndFrame = endFrame; }

    s32 lifeStep() const { return mLifeStep; }
    void setLifeStep(s32 lifeStep) { mLifeStep = lifeStep; }

    s32 lifeStepRandom() const { return mLifeStepRnd; }
    void setLifeStepRandom(s32 random) { mLifeStepRnd = random; }

    s32 emitRate() const { return mEmitRate; }
    void setEmitRate(s32 emitRate) { mEmitRate = emitRate; }

    // ----- Velocity Properties ----- \\

    f32 figureVelocity() const { return mFigureVelocity; }
    void setFigureVelocity(f32 velocity) { mFigureVelocity = velocity; }

    const Math::Vector3f& velocityDirection() const { return mVelocityDir; }
    void setVelocityDirection(const Math::Vector3f& direction) { mVelocityDir = direction; }

    f32 initialVelocity() const { return mInitVelocity; }
    void setInitialVelocity(f32 velocity) { mInitVelocity = velocity; }

    f32 initialVelocityRandom() const { return mInitVelocityRnd; }
    void setInitialVelocityRandom(f32 random) { mInitVelocityRnd = random; }

    const Math::Vector3f& spreadVector() const { return mSpreadVec; }
    void setSpreadVector(const Math::Vector3f& vector) { mSpreadVec = vector; }

    f32 airResistance() const { return mAirResistance; }
    void setAirResistance(f32 resistance) { mAirResistance = resistance; }

    // ----- Volume Properties ----- \\

    u8 volumeTblIndex() const { return mVolumeTblIndex; }
    void setVolumeTblIndex(u8 index) { mVolumeTblIndex = index; }

    VolumeType volumeType() const { return mVolumeType; }
    void setVolumeType(VolumeType type) { mVolumeType = type; }

    const Math::Vector3f& volumeRadius() const { return mVolumeRadius; }
    void setVolumeRadius(const Math::Vector3f& radius) { mVolumeRadius = radius; }

    s32 volumeSweepStart() const { return mVolumeSweepStart; }
    void setVolumeSweepStart(s32 sweepStart) { mVolumeSweepStart = sweepStart; }

    s32 volumeSweepParam() const { return mVolumeSweepParam; }
    void setVolumeSweepParam(s32 sweepParam) { mVolumeSweepParam = sweepParam; }

    // ----- Rotation Properties ----- \\

    RotType rotationType() const { return mRotType; }
    void setRotationType(RotType type) { mRotType = type; }

    const Math::Vector3i& initialRotation() const { return mInitRot; }
    void setInitialRotation(const Math::Vector3i& rotation) { mInitRot = rotation; }

    const Math::Vector3i& initialRotationRandom() const { return mInitRotRand; }
    void setInitialRotationRandom(const Math::Vector3i& rotation) { mInitRotRand = rotation; }

    const Math::Vector3i& rotationVelocity() const { return mRotVel; }
    void setRotationVelocity(const Math::Vector3i& velocity) { mRotVel = velocity; }

    const Math::Vector3i& rotationVelocityRandom() const { return mRotVelRand; }
    void setRotationVelocityRandom(const Math::Vector3i& random) { mRotVelRand = random; }

    const Math::Vector2f& rotationBasis() const { return mRotBasis; }
    void setRotationBasis(const Math::Vector2f& basis) { mRotBasis = basis; }

    // ----- Alpha Properties ----- \\

    const AlphaAnim& alphaAnim() const { return mAlphaAnim; }
    void setAlphaAnim(const AlphaAnim& alphaAnim) { mAlphaAnim = alphaAnim; }

    // ----- Combiner Properties ----- \\

    BlendFuncType blendFunction() const { return mBlendFunc; }
    void setBlendFunction(BlendFuncType blendFunction) { mBlendFunc = blendFunction; }

    DepthFuncType depthFunction() const { return mDepthFunc; }
    void setDepthFunction(DepthFuncType depthFunction) { mDepthFunc = depthFunction; }

    ColorCombinerFuncType combinerFunction() const { return mCombinerFunc; }
    void setCombinerFunction(ColorCombinerFuncType combineFunc) { mCombinerFunc = combineFunc; }

    bool isFogEnabled() const { return mFlag.isSet(EmitterFlag::EnableFog); }
    void setIsFogEnabled(bool enabled) { mFlag.set(EmitterFlag::EnableFog, enabled); }

    BitFlag<EmitterFlag>& flags();
    const BitFlag<EmitterFlag>& flags() const;

    TextureHandle& textureHandle();
    const TextureHandle& textureHandle() const;
    void setTexture(const std::shared_ptr<Texture>& texture);

    const ColorProperties& colorProperties() const;
    void setColorProperties(const ColorProperties& colorProperties);

    const TextureProperties& textureProperties() const;
    void setTextureProperties(const TextureProperties& textureProperties);

    const ComplexProperties& complexProperties() const;
    void setComplexProperties(const ComplexProperties& complexProperties);

    void setChildFlags(const BitFlag<ChildFlag>& childFlags);
    void setFluctuationFlags(const BitFlag<FluctuationFlag>& fluxFlags);
    void setFieldFlags(const BitFlag<FieldFlag>& fieldFlags);
    void setStripeFlags(const BitFlag<StripeFlag>& stripeFlags);

    const ChildData& childData() const;
    ChildData& childData();

    const FluctuationData& fluctuationData() const;
    void setFluctuationData(const FluctuationData& fluctuationData);
    void initFluctuationData(const BinFluctuationData& fluctuationData);

    const FieldData& fieldData() const;
    FieldData& fieldData();
    void setFieldData(const FieldData& fieldData);

    const StripeData& stripeData() const;
    void setStripeData(const StripeData& stripeData);
    void initStripeData(const BinStripeData& stripeData);

    bool hasStripeData() const;

    void initFromBinary(const BinCommonEmitterData& emitterData);
    void initComplexFromBinary(const BinComplexEmitterData& emitterData);

private:
    BitFlag<EmitterFlag> mFlag{};

    // Basic Properties
    EmitterType mType{EmitterType::Simple};
    FollowType mFollowType{FollowType::All};
    QString mName{"Emitter"};
    PtclSeed mRandomSeed{};
    BillboardType mBillboardType{BillboardType::Billboard};
    bool mIsPolygon{false};
    bool mIsVelLook{false};
    bool mIsEmitterBillboardMtx{false};
    bool mIsFollow{false};

    // Gravity Properties
    bool mIsDirectional{false};
    Math::Vector3f mGravity{0.0f, -1.0f, 0.0f};

    // Lifespan Properties
    s32 mPtclLife{100};
    s32 mPtclLifeRnd{0};

    // Termination Properties
    bool mIsStopEmitInFade{true};
    f32 mAlphaAddInFade{0.0f};

    // Transform Properties
    Math::Matrix34f mTransformSRT{
         {1.0f, 0.0f, 0.0f, 0.0f},
         {0.0f, 1.0f, 0.0f, 0.0f},
         {0.0f, 0.0f, 1.0f, 0.0f},
    };
    Math::Matrix34f mTransformRT{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
    };

    // Scale Properties
    ScaleAnim mScaleAnim{};
    f32 mScaleRand{0.0f};

    // Emission Properties
    s32 mEmitStartFrame{0};
    s32 mEmitEndFrame{1};
    s32 mLifeStep{10};
    s32 mLifeStepRnd{1};
    s32 mEmitRate{1};

    // Velocity Properties
    f32 mFigureVelocity{0.1f};
    Math::Vector3f mVelocityDir{0.0f, 1.0f, 0.0f};
    f32 mInitVelocity{0.0f};
    f32 mInitVelocityRnd{0.0f};
    Math::Vector3f mSpreadVec{0.0f, 0.0f, 0.0f};
    f32 mAirResistance{1.0f};

    // Volume Properties
    u8 mVolumeTblIndex{0};
    VolumeType mVolumeType{VolumeType::Point};
    Math::Vector3f mVolumeRadius{1.0f, 1.0f, 1.0f};
    s32 mVolumeSweepStart{0};
    s32 mVolumeSweepParam{0};

    // Rotation Properties
    RotType mRotType{RotType::None};
    Math::Vector3i mInitRot{0, 0, 0};
    Math::Vector3i mInitRotRand{0, 0, 0};
    Math::Vector3i mRotVel{0, 0, 0};
    Math::Vector3i mRotVelRand{0, 0, 0};
    Math::Vector2f mRotBasis{0.0f, 0.0f};

    // Alpha properties
    AlphaAnim mAlphaAnim{};

    // Combiner Properties
    BlendFuncType mBlendFunc{BlendFuncType::Translucent};
    DepthFuncType mDepthFunc{DepthFuncType::Unk0};
    ColorCombinerFuncType mCombinerFunc{ColorCombinerFuncType::CombinerConfig0};

    ColorProperties mColorProperties{};
    TextureProperties mTextureProperties{};

    TextureHandle mTextureHandle{};

    ComplexProperties mComplexProperties{};

    ChildData mChildData{};
    FluctuationData mFluctuationData{};
    FieldData mFieldData{};

    StripeData mStripeData{};
};


// ========================================================================== //


} // namespace Ptcl
