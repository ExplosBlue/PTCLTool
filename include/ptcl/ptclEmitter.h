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

    struct RotationProperties {
        RotType rotType{RotType::None};
        Math::Vector3i initRot{0, 0, 0};
        Math::Vector3i initRotRand{0, 0, 0};
        Math::Vector3i rotVel{0, 0, 0};
        Math::Vector3i rotVelRand{0, 0, 0};
        Math::Vector2f rotBasis{0.0f, 0.0f};
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

    struct AlphaProperties {
        f32 initAlpha{1.0f};
        f32 diffAlpha21{0.0f};
        f32 diffAlpha32{0.0f};
        s32 alphaSection1{0};
        s32 alphaSection2{100};
    };

    struct VolumeProperties {
        u8 volumeTblIndex{0};
        VolumeType volumeType{VolumeType::Point};
        Math::Vector3f volumeRadius{1.0f, 1.0f, 1.0f};
        s32 volumeSweepStart{0};
        s32 volumeSweepParam{0};
    };

    struct VelocityProperties {
        f32 figureVel{0.1f};
        Math::Vector3f emitterVelDir{0.0f, 1.0f, 0.0f};
        f32 initVel{0.0f};
        f32 initVelRnd{0.0f};
        Math::Vector3f spreadVec{0.0f, 0.0f, 0.0f};
        f32 airResistance{1.0f};
    };

    struct EmissionProperties {
        s32 startFrame{0};
        s32 endFrame{1};
        s32 lifeStep{10};
        s32 lifeStepRnd{1};
        s32 emitRate{1};
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

    struct CombinerProperties {
        BlendFuncType blendFunc{BlendFuncType::Translucent};
        DepthFuncType depthFunc{DepthFuncType::Unk0};
        ColorCombinerFuncType combinerFunc{ColorCombinerFuncType::CombinerConfig0};
        bool isFogEnabled{false};
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

    BitFlag<EmitterFlag>& flags();
    const BitFlag<EmitterFlag>& flags() const;

    TextureHandle& textureHandle();
    const TextureHandle& textureHandle() const;
    void setTexture(const std::shared_ptr<Texture>& texture);

    const EmissionProperties& emissionProperties() const;
    void setEmissionProperties(const EmissionProperties& emissionProperties);

    const VelocityProperties& velocityProperties() const;
    void setVelocityProperties(const VelocityProperties& velocityProperties);

    const VolumeProperties& volumeProperties() const;
    void setVolumeProperties(const VolumeProperties& volumeProperties);

    const ColorProperties& colorProperties() const;
    void setColorProperties(const ColorProperties& colorProperties);

    const AlphaProperties& alphaProperties() const;
    void setAlphaProperties(const AlphaProperties& alphaProperties);

    const TextureProperties& textureProperties() const;
    void setTextureProperties(const TextureProperties& textureProperties);

    const RotationProperties& rotationProperties() const;
    void setRotationProperties(const RotationProperties& rotationProperties);

    const CombinerProperties& combinerProperties() const;
    void setCombinerProperties(const CombinerProperties& combinerProperties);

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



    EmissionProperties mEmissionProperties{};
    VelocityProperties mVelocityProperties{};
    VolumeProperties mVolumeProperties{};
    ColorProperties mColorProperties{};
    AlphaProperties mAlphaProperties{};
    RotationProperties mRotationProperties{};
    TextureProperties mTextureProperties{};
    CombinerProperties mCombinerProperties{};

    TextureHandle mTextureHandle{};

    ComplexProperties mComplexProperties{};

    ChildData mChildData{};
    FluctuationData mFluctuationData{};
    FieldData mFieldData{};

    StripeData mStripeData{};
};


// ========================================================================== //


} // namespace Ptcl
