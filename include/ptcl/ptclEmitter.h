#pragma once

#include "math/matrix.h"
#include "math/util.h"

#include "ptcl/ptclBinary.h"
#include "ptcl/ptclEnum.h"
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

    struct AlphaAnim {
        f32 initAlpha{1.0f};
        f32 diffAlpha21{0.0f};
        f32 diffAlpha32{0.0f};
        s32 alphaSection1{0};
        s32 alphaSection2{100};

        bool operator==(const AlphaAnim&) const = default;
    };

public:
    Emitter() = default;
    Emitter(QString name);

    Emitter(const Emitter&) = delete;
    Emitter& operator=(const Emitter&) = delete;

    std::unique_ptr<Emitter> clone() const;

    BitFlag<EmitterFlag>& flags();
    const BitFlag<EmitterFlag>& flags() const;

    void initFromBinary(const BinCommonEmitterData& emitterData);
    void initComplexFromBinary(const BinComplexEmitterData& emitterData);

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

    // ----- Color Properties ----- \\

    const std::array<binColor4f, 3>& color0() const { return mColor0; }

    const binColor4f& primaryColor() const { return mColor0[0]; }
    void setPrimaryColor(const binColor4f& color) { mColor0[0] = color; }

    const binColor4f& randomColorA() const { return mColor0[0]; }
    void setRandomColorA(const binColor4f& color) { mColor0[0] = color; }

    const binColor4f& randomColorB() const { return mColor0[1]; }
    void setRandomColorB(const binColor4f& color) { mColor0[1] = color; }

    const binColor4f& randomColorC() const { return mColor0[2]; }
    void setRandomColorC(const binColor4f& color) { mColor0[2] = color; }

    const binColor4f& startColor() const { return mColor0[0]; }
    void setStartColor(const binColor4f& color) { mColor0[0] = color; }

    const binColor4f& midColor() const { return mColor0[1]; }
    void setMidColor(const binColor4f& color) { mColor0[1] = color; }

    const binColor4f& endColor() const { return mColor0[2]; }
    void setEndColor(const binColor4f& color) { mColor0[2] = color; }

    s32 colorSection1() const { return mColorSection1; }
    void setColorSection1(s32 section) { mColorSection1 = section; }

    s32 colorSection2() const { return mColorSection2; }
    void setColorSection2(s32 section) { mColorSection2 = section; }

    s32 colorSection3() const { return mColorSection3; }
    void setColorSection3(s32 section) { mColorSection3 = section; }

    s32 colorNumRepeat() const { return mColorNumRepeat; }
    void setColorNumRepeat(s32 num) { mColorNumRepeat = num; }

    bool isColorRandom() const { return mFlag.isSet(EmitterFlag::ColorRandom); }
    void setIsColorRandom(bool isRandom) { mFlag.set(EmitterFlag::ColorRandom, isRandom); }

    bool isColorAnimation() const { return mFlag.isSet(EmitterFlag::ColorAnimation); }
    void setIsColorAnimation(bool isAnim) { mFlag.set(EmitterFlag::ColorAnimation, isAnim); }

    ColorCalcType colorCalcType() const { return mColorCalcType; }
    void setColorCalcType(ColorCalcType type) { mColorCalcType = type; }

    const binColor3f& secondaryColor() const { return mColor1; }
    void setSecondaryColor(const binColor3f& color) { mColor1 = color; }

    // ----- Texture Properties ----- \\

    TextureWrap textureWrapT() const { return mTextureWrapT; }
    void setTextureWrapT(TextureWrap wrap) { mTextureWrapT = wrap; }

    TextureWrap textureWrapS() const { return mTextureWrapS; }
    void setTextureWrapS(TextureWrap wrap) { mTextureWrapS = wrap; }

    TextureFilter textureMagFilter() const { return mTextureMagFilter; }
    void setTextureMagFilter(TextureFilter filter) { mTextureMagFilter = filter; }

    TextureFilter textureMinFilter() const { return mTextureMinFilter; }
    void setTextureMinFilter(TextureFilter filter) { mTextureMinFilter = filter; }

    TextureMipFilter textureMipFilter() const { return mTextureMipFilter; }
    void setTextureMipFilter(TextureMipFilter filter) { mTextureMipFilter = filter; }

    u16 numTexturePattern() const { return mNumTexturePattern; }
    void setNumTexturePattern(u16 num) { mNumTexturePattern = num; }

    u8 numTextureDivisionX() const { return mNumTextureDivisionX; }
    void setNumTextureDivisionX(u8 num) {
        mTextureUVScale.setX(static_cast<f32>(numTextureRepetitionsX()) / static_cast<f32>(num));
        mNumTextureDivisionX = num;
    }

    u8 numTextureDivisionY() const { return mNumTextureDivisionY; }
    void setNumTextureDivisionY(u8 num) {
        mTextureUVScale.setY(static_cast<f32>(numTextureRepetitionsY()) / static_cast<f32>(num));
        mNumTextureDivisionY = num;
    }

    s32 numTextureRepetitionsX() const { return static_cast<s32>(std::round(mTextureUVScale.getX() * static_cast<f32>(mNumTextureDivisionX))); }
    void setNumTextureRepetitionsX(s32 num) { mTextureUVScale.setX(static_cast<f32>(num) / static_cast<f32>(mNumTextureDivisionX)); }

    s32 numTextureRepetitionsY() const { return static_cast<s32>(std::round(mTextureUVScale.getY() * static_cast<f32>(mNumTextureDivisionY))); }
    void setNumTextureRepetitionsY(s32 num) { mTextureUVScale.setY(static_cast<f32>(num) / static_cast<f32>(mNumTextureDivisionY)); }

    const Math::Vector2f& textureUVScale() const { return mTextureUVScale; }
    void setTextureUVScale(const Math::Vector2f& scale) { mTextureUVScale = scale; }

    const std::array<u8, 16>& texturePatternTable() const { return mTexturePatternTbl; }
    void setTexturePatternTable(const std::array<u8, 16>& table) { mTexturePatternTbl = table; }

    u16 texturePatternFrequency() const { return mTexturePatternFrequency; }
    void setTexturePatternFrequency(u16 frequency) { mTexturePatternFrequency = frequency; }

    u16 texturePatternTableUse() const { return mTexturePatternTblUse; }
    void setTexturePatternTableUse(u16 use) { mTexturePatternTblUse = use; }

    bool isTexturePatternAnim() const { return mIsTexturePatternAnim; }
    void setIsTexturePatternAnim(bool isAnim) { mIsTexturePatternAnim = isAnim; }

    const TextureHandle& textureHandle() const { return mTextureHandle; }

    std::shared_ptr<Texture> texture() const { return mTextureHandle.get(); }
    void setTexture(const std::shared_ptr<Texture>& texture) { mTextureHandle.set(texture); }

    // ----- Fluctuation Properties ----- \\

    void initFluctuationData(const BinFluctuationData& fluctuationData);

    const BitFlag<FluctuationFlag>& fluctuationFlags() const { return mFluctuationFlags; }
    void setFluctuationFlags(const BitFlag<FluctuationFlag>& fluxFlags) { mFluctuationFlags = fluxFlags; }

    f32 fluctuationScale() const { return mFluctuationScale; }
    void setFluctuationScale(f32 scale) { mFluctuationScale = scale; }

    f32 fluctuationFrequency() const { return mFluctuationFreq; }
    void setFluctuationFrequency(f32 frequency) { mFluctuationFreq = frequency; }

    bool isFluctuationPhaseRandom() const { return mFluctuationPhaseRnd; }
    void setFluctuationPhaseRandom(bool random) { mFluctuationPhaseRnd = random; }

    bool isFluctuationEnabled() const { return mFluctuationFlags.isSet(Ptcl::FluctuationFlag::Enabled); }
    void setFluctuationEnabled(bool enabled) { mFluctuationFlags.set(Ptcl::FluctuationFlag::Enabled, enabled); }

    bool isFluctuationApplyAlpha() const { return mFluctuationFlags.isSet(Ptcl::FluctuationFlag::ApplyAlpha); }
    void setFluctuationApplyAlpha(bool apply) { mFluctuationFlags.set(Ptcl::FluctuationFlag::ApplyAlpha, apply); }

    bool isFluctuationApplyScale() const { return mFluctuationFlags.isSet(Ptcl::FluctuationFlag::ApplyScale); }
    void setFluctuationApplyScale(bool apply) { mFluctuationFlags.set(Ptcl::FluctuationFlag::ApplyScale, apply); }

    // ----- Stripe Properties ----- \\

    void initStripeData(const BinStripeData& stripeData);

    const BitFlag<StripeFlag>& stripeFlags() const { return mStripeFlags; }
    void setStripeFlags(const BitFlag<StripeFlag>& stripeFlags) { mStripeFlags = stripeFlags; }

    bool hasStripeData() const;

    StripeType stripeType() const { return mStripeType; }
    void setStripeType(StripeType type) { mStripeType = type; }

    s32 stripeNumHistory() const { return mStripeNumHistory; }
    void setStripeNumHistory(s32 num) { mStripeNumHistory = num; }

    f32 stripeStartAlpha() const { return mStripeStartAlpha; }
    void setStripeStartAlpha(f32 alpha) { mStripeStartAlpha = alpha; }

    f32 stripeEndAlpha() const { return mStripeEndAlpha; }
    void setStripeEndAlpha(f32 alpha) { mStripeEndAlpha = alpha; }

    const Math::Vector2f& stripeUVScrollSpeed() const { return mStripeUVScrollSpeed; }
    void setStripeUVScrollSpeed(const Math::Vector2f& speed) { mStripeUVScrollSpeed = speed; }

    s32 stripeHistoryStep() const { return mStripeHistoryStep; }
    void setStripeHistoryStep(s32 step) { mStripeHistoryStep = step; }

    f32 stripeDirInterpolate() const { return mStripeDirInterpolate; }
    void setStripeDirInterpolate(f32 interp) { mStripeDirInterpolate = interp; }

    bool isStripeEmitterCoord() const { return mStripeFlags.isSet(Ptcl::StripeFlag::EmitterCoord); }
    void setStripeEmitterCoord(bool emitterCoord) { mStripeFlags.set(Ptcl::StripeFlag::EmitterCoord, emitterCoord); }

    // ----- Field Properties ----- \\

    bool isFieldEnabled() const { return mFieldFlags.isSet(FieldFlag::Enabled); }

    const BitFlag<FieldFlag>& fieldFlags() const { return mFieldFlags; }
    void setFieldFlags(const BitFlag<FieldFlag>& fieldFlags) { mFieldFlags = fieldFlags; }

    // Field Random

    void initFieldRandom(const BinFieldRandomData& randomData);

    bool isFieldRandomEnabled() const { return mFieldFlags.isSet(FieldFlag::Random); }
    void setFieldRandomEnabled(bool enabled) { mFieldFlags.set(FieldFlag::Random, enabled); }

    s32 fieldRandomBlank() const { return mFieldRandom.randomBlank; }
    void setFieldRandomBlank(s32 blank) { mFieldRandom.randomBlank = blank; }

    const Math::Vector3f& fieldRandomVelAdd() const { return mFieldRandom.randomVelAdd; }
    void setFieldRandomVelAdd(const Math::Vector3f& velAdd) { mFieldRandom.randomVelAdd = velAdd; }

    // Field Magnet

    void initFieldMagnet(const BinFieldMagnetData& magnetData);

    bool isFieldMagnetEnabled() const { return mFieldFlags.isSet(FieldFlag::Magnet); }
    void setFieldMagnetEnabled(bool enabled) { mFieldFlags.set(FieldFlag::Magnet, enabled); }

    f32 fieldMagnetPower() const { return mFieldMagnet.magnetPower; }
    void setFieldMagnetPower(f32 power) { mFieldMagnet.magnetPower = power; }

    const Math::Vector3f& fieldMagnetPos() const { return mFieldMagnet.magnetPos; }
    void setFieldMagnetPos(const Math::Vector3f& pos) { mFieldMagnet.magnetPos = pos; }

    const BitFlag<FieldMagnetFlag>& fieldMagnetFlag() const { return mFieldMagnet.magnetFlag; }

    bool isFieldMagnetAxisTargetX() const { return mFieldMagnet.magnetFlag.isSet(FieldMagnetFlag::AxisTargetX); }
    void setFieldMagnetAxisTargetX(bool enabled) { mFieldMagnet.magnetFlag.set(FieldMagnetFlag::AxisTargetX, enabled); }

    bool isFieldMagnetAxisTargetY() const { return mFieldMagnet.magnetFlag.isSet(FieldMagnetFlag::AxisTargetY); }
    void setFieldMagnetAxisTargetY(bool enabled) { mFieldMagnet.magnetFlag.set(FieldMagnetFlag::AxisTargetY, enabled); }

    bool isFieldMagnetAxisTargetZ() const { return mFieldMagnet.magnetFlag.isSet(FieldMagnetFlag::AxisTargetZ); }
    void setFieldMagnetAxisTargetZ(bool enabled) { mFieldMagnet.magnetFlag.set(FieldMagnetFlag::AxisTargetZ, enabled); }

    // Field Spin

    void initFieldSpin(const BinFieldSpinData& spinData);

    bool isFieldSpinEnabled() const { return mFieldFlags.isSet(FieldFlag::Spin); }
    void setFieldSpinEnabled(bool enabled) { mFieldFlags.set(FieldFlag::Spin, enabled); }

    s32 fieldSpinRotate() const { return mFieldSpin.spinRotate; }
    void setFieldSpinRotate(s32 rotate) { mFieldSpin.spinRotate = rotate; }

    FieldSpinAxis fieldSpinAxis() const { return mFieldSpin.spinAxis; }
    void setFieldSpinAxis(FieldSpinAxis axis) { mFieldSpin.spinAxis = axis; }

    // Field Collision

    void initFieldCollision(const BinFieldCollisionData& collisionData);

    bool isFieldCollisionEnabled() const { return mFieldFlags.isSet(FieldFlag::Collision); }
    void setFieldCollisionEnabled(bool enabled) { mFieldFlags.set(FieldFlag::Collision, enabled); }

    FieldCollisionType fieldCollisionType() const { return mFieldCollision.collisionType; }
    void setFieldCollisionType(FieldCollisionType type) { mFieldCollision.collisionType = type; }

    bool fieldCollisionIsWorld() const { return mFieldCollision.collisionIsWorld; }
    void setFieldCollisionIsWorld(bool isWorld) { mFieldCollision.collisionIsWorld = isWorld; }

    f32 fieldCollisionCoord() const { return mFieldCollision.collisionCoord; }
    void setFieldCollisionCoord(f32 coord) { mFieldCollision.collisionCoord = coord; }

    f32 fieldCollisionCoef() const { return mFieldCollision.collisionCoef; }
    void setFieldCollisionCoef(f32 coef) { mFieldCollision.collisionCoef = coef; }

    // Field Convergence

    void initFieldConvergence(const BinFieldConvergenceData& convergenceData);

    bool isFieldConvergenceEnabled() const { return mFieldFlags.isSet(FieldFlag::Convergence); }
    void setFieldConvergenceEnabled(bool enabled) { mFieldFlags.set(FieldFlag::Convergence, enabled); }

    FieldConvergenceType fieldConvergenceType() const { return mFieldConvergence.convergenceType; }
    void setFieldConvergenceType(FieldConvergenceType type) { mFieldConvergence.convergenceType = type; }

    const Math::Vector3f& fieldConvergencePos() const { return mFieldConvergence.convergencePos; }
    void setFieldConvergencePos(const Math::Vector3f& pos) { mFieldConvergence.convergencePos = pos; }

    // Field PosAdd

    void initFieldPosAdd(const BinFieldPosAddData& posAddData);

    bool isFieldPosAddEnabled() const { return mFieldFlags.isSet(FieldFlag::PosAdd); }
    void setFieldPosAddEnabled(bool enabled) { mFieldFlags.set(FieldFlag::PosAdd, enabled); }

    const Math::Vector3f& fieldPosAddPosition() const { return mFieldPosAdd.posAdd; }
    void setFieldPosAddPosition(const Math::Vector3f& pos) { mFieldPosAdd.posAdd = pos; }

    // ----- Child Properties ----- \\

    void initChild(const BinChildData& childData);

    const BitFlag<ChildFlag>& childFlags() const { return mChildFlags; }
    void setChildFlags(const BitFlag<ChildFlag>& childFlags) { mChildFlags = childFlags; }

    bool isChildEnabled() const { return mChildFlags.isSet(ChildFlag::Enabled); }
    void setChildEnabled(bool inherit) { mChildFlags.set(ChildFlag::Enabled, inherit); }

    bool isChildInheritAlpha() const { return mChildFlags.isSet(ChildFlag::AlphaInherit); }
    void setChildInheritAlpha(bool inherit) { mChildFlags.set(ChildFlag::AlphaInherit, inherit); }

    bool isChildFollow() const { return mChildFlags.isSet(ChildFlag::IsFollow); }
    void setChildFollow(bool follow) { mChildFlags.set(ChildFlag::IsFollow, follow); }

    bool isChildParentField() const { return mChildFlags.isSet(ChildFlag::ParentField); }
    void setChildParentField(bool isField) { mChildFlags.set(ChildFlag::ParentField, isField); }

    bool isChildPreDraw() const { return mChildFlags.isSet(ChildFlag::PreChildDraw); }
    void setChildPreDraw(bool preDraw) { mChildFlags.set(ChildFlag::PreChildDraw, preDraw); }

    bool isChildInheritParentColor() const { return mChildFlags.isSet(ChildFlag::Color0Inherit); }
    void setChildInheritParentColor(bool inherit) { mChildFlags.set(ChildFlag::Color0Inherit, inherit); }

    bool isChildInheritRotation() const { return mChildFlags.isSet(ChildFlag::RotateInherit); }
    void setChildInheritRotation(bool inherit) { mChildFlags.set(ChildFlag::RotateInherit, inherit); }

    bool isChildInheritScale() const { return mChildFlags.isSet(ChildFlag::ScaleInherit); }
    void setChildInheritScale(bool inherit) { mChildFlags.set(ChildFlag::ScaleInherit, inherit); }

    bool isChildInheritVelocity() const { return mChildFlags.isSet(ChildFlag::VelInherit); }
    void setChildInheritVelocity(bool inherit) { mChildFlags.set(ChildFlag::VelInherit, inherit); }

    // Child Basic

    BillboardType childBillboardType() const { return mChild.billboardType; }
    void setChildBillboardType(BillboardType type) {
        mChild.billboardType = type;

        const bool isPolygon = (type == Ptcl::BillboardType::PolygonXY || type == Ptcl::BillboardType::PolygonXZ);
        mChildFlags.set(ChildFlag::IsPolygon, isPolygon);
    }

    // Child Emission

    s32 childEmitRate() const { return mChild.emitRate; }
    void setChildEmitRate(s32 rate) { mChild.emitRate = rate; }

    s32 childEmitTiming() const { return mChild.emitTiming; }
    void setChildEmitTiming(s32 timing) { mChild.emitTiming = timing; }

    s32 childLife() const { return mChild.life; }
    void setChildLife(s32 life) { mChild.life = life; }

    s32 childEmitStep() const { return mChild.emitStep; }
    void setChildEmitStep(s32 step) { mChild.emitStep = step; }

    // Child Velocity

    const Math::Vector3f& childRandVelocity() const { return mChild.randVel; }
    void setChildRandVelocity(const Math::Vector3f& vel) { mChild.randVel = vel; }

    const Math::Vector3f& childGravity() const { return mChild.gravity; }
    void setChildGravity(const Math::Vector3f& gravity) { mChild.gravity = gravity; }

    f32 childVelocityInheritRate() const { return mChild.velInheritRate; }
    void setChildVelocityInheritRate(f32 rate) { mChild.velInheritRate = rate; }

    f32 childInitalPositionRand() const { return mChild.initPosRand; }
    void setChildInitialPositionRand(f32 rand) { mChild.initPosRand = rand; }

    f32 childFigureVelocity() const { return mChild.figurVel; }
    void setChildFigureVelocity(f32 vel) { mChild.figurVel = vel; }

    f32 childAirResistance() const { return mChild.airResist; }
    void setChildAirResistance(f32 resist) { mChild.airResist = resist; }

    // Child Rotation

    RotType childRotationType() const { return mRotType; }
    void setChildRotationType(RotType type) { mRotType = type; }

    const Math::Vector3i& childInitialRotation() const { return mChild.initRot; }
    void setChildInitialRotation(const Math::Vector3i& rotation) { mChild.initRot = rotation; }

    const Math::Vector3i& childInitialRotationRandom() const { return mChild.initRotRand; }
    void setChildInitialRotationRandom(const Math::Vector3i& rotation) { mChild.initRotRand = rotation; }

    const Math::Vector3i& childRotationVelocity() const { return mChild.rotVel; }
    void setChildRotationVelocity(const Math::Vector3i& velocity) { mChild.rotVel = velocity; }

    const Math::Vector3i& childRotationVelocityRandom() const { return mChild.rotVelRand; }
    void setChildRotationVelocityRandom(const Math::Vector3i& random) { mChild.rotVelRand = random; }

    const Math::Vector2f& childRotationBasis() const { return mChild.rotBasis; }
    void setChildRotationBasis(const Math::Vector2f& basis) { mChild.rotBasis = basis; }

    // Child Scale

    const Math::Vector2f& childScale() const { return mChild.scale; }
    void setChildScale(const Math::Vector2f& scale) { mChild.scale = scale; }

    const Math::Vector2f& childScaleTarget() const { return mChild.scaleTarget; }
    void setChildScaleTarget(const Math::Vector2f& scale) { mChild.scaleTarget = scale; }

    f32 childScaleInheritRate() const { return mChild.scaleInheritRate; }
    void setChildScaleInheritRate(f32 rate) { mChild.scaleInheritRate = rate; }

    s32 childScaleStartFrame() const { return mChild.scaleStartFrame; }
    void setChildScaleStartFrame(s32 frame) { mChild.scaleStartFrame = frame; }

    // Child Texture

    TextureWrap childTextureWrapT() const { return mTextureWrapT; }
    void setChildTextureWrapT(TextureWrap wrap) { mTextureWrapT = wrap; }

    TextureWrap childTextureWrapS() const { return mChild.textureWrapS; }
    void setChildTextureWrapS(TextureWrap wrap) { mChild.textureWrapS = wrap; }

    TextureFilter childTextureMagFilter() const { return mChild.textureMagFilter; }
    void setChildTextureMagFilter(TextureFilter filter) { mChild.textureMagFilter = filter; }

    TextureFilter childTextureMinFilter() const { return mChild.textureMinFilter; }
    void setChildTextureMinFilter(TextureFilter filter) { mChild.textureMinFilter = filter; }

    TextureMipFilter childTextureMipFilter() const { return mChild.textureMipFilter; }
    void setChildTextureMipFilter(TextureMipFilter filter) { mChild.textureMipFilter = filter; }

    const Math::Vector2f& childTextureUVScale() const { return mChild.texUVScale; }
    void setChildTextureUVScale(const Math::Vector2f& scale) { mChild.texUVScale = scale; }

    const TextureHandle& childTextureHandle() const { return mChild.textureHandle; }

    std::shared_ptr<Texture> childTexture() const { return mChild.textureHandle.get(); }
    void setChildTexture(const std::shared_ptr<Texture>& texture) { mChild.textureHandle.set(texture); }

    // Child Color

    const binColor4f& childPrimaryColor() const { return mChild.color0; }
    void setChildPrimaryColor(const binColor4f& color) { mChild.color0 = color; }

    const binColor3f& childSecondaryColor() const { return mChild.color1; }
    void setChildSecondaryColor(const binColor3f& color) { mChild.color1 = color; }

    // Child Alpha

    f32 childAlpha() const { return mChild.alpha; }
    void setChildAlpha(f32 alpha) { mChild.alpha = alpha; }

    f32 childAlphaTarget() const { return mChild.alphaTarget; }
    void setChildAlphaTarget(f32 alpha) { mChild.alphaTarget = alpha; }

    f32 childAlphaInit() const { return mChild.alphaInit; }
    void setChildAlphaInit(f32 alpha) { mChild.alphaInit = alpha; }

    s32 childAlphaStartFrame() const { return mChild.alphaStartFrame; }
    void setChildAlphaStartFrame(s32 frame) { mChild.alphaStartFrame = frame; }

    s32 childAlphaBaseFrame() const { return mChild.alphaBaseFrame; }
    void setChildAlphaBaseFrame(s32 frame) { mChild.alphaBaseFrame = frame; }

    // Child Combiner

    BlendFuncType childBlendFunc() const { return mChild.blendFunc; }
    void setChildBlendFunc(BlendFuncType type) { mChild.blendFunc = type; }

    DepthFuncType childDepthFunc() const { return mChild.depthFunc; }
    void setChildDepthFunc(DepthFuncType type) { mChild.depthFunc = type; }

    ColorCombinerFuncType childCombinerFunc() const { return mChild.combinerFunc; }
    void setChildCombinerFunc(ColorCombinerFuncType type) { mChild.combinerFunc = type; }

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

    // Color Properties
    std::array<binColor4f, 3> mColor0{
        binColor4f{1.0f, 1.0f, 1.0f, 1.0f},
        binColor4f{1.0f, 1.0f, 1.0f, 1.0f},
        binColor4f{1.0f, 1.0f, 1.0f, 1.0f},
    };
    s32 mColorSection1{20};
    s32 mColorSection2{60};
    s32 mColorSection3{80};
    s32 mColorNumRepeat{1};
    ColorCalcType mColorCalcType{ColorCalcType::None};

    binColor3f mColor1{255.0f, 255.0f, 255.0f};

    // Texture Properties
    TextureWrap mTextureWrapT{TextureWrap::ClampToEdge};
    TextureWrap mTextureWrapS{TextureWrap::ClampToEdge};
    TextureFilter mTextureMagFilter{TextureFilter::Nearest};
    TextureFilter mTextureMinFilter{TextureFilter::Nearest};
    TextureMipFilter mTextureMipFilter{TextureMipFilter::None};
    u16 mNumTexturePattern{1};
    u8 mNumTextureDivisionX{1};
    u8 mNumTextureDivisionY{1};
    Math::Vector2f mTextureUVScale{1.0f, 1.0f};
    std::array<u8, 16> mTexturePatternTbl{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    u16 mTexturePatternFrequency{0};
    u16 mTexturePatternTblUse{2};
    bool mIsTexturePatternAnim{false};
    TextureHandle mTextureHandle{};

    // ----- Complex Properties ----- \\

    // Fluctuation Properties
    f32 mFluctuationScale{1.0f};
    f32 mFluctuationFreq{20.0f};
    bool mFluctuationPhaseRnd{false};
    BitFlag<FluctuationFlag> mFluctuationFlags{
        FluctuationFlag::ApplyAlpha,
        FluctuationFlag::ApplyScale
    };

    // Stripe Properties
    StripeType mStripeType{StripeType::Billboard};
    s32 mStripeNumHistory{60};
    f32 mStripeStartAlpha{1.0f};
    f32 mStripeEndAlpha{1.0f};
    Math::Vector2f mStripeUVScrollSpeed{0.0f, 0.0f};
    s32 mStripeHistoryStep{1};
    f32 mStripeDirInterpolate{1.0f};
    BitFlag<StripeFlag> mStripeFlags{};

    // Field Properties
    struct {
        s32 randomBlank{1};
        Math::Vector3f randomVelAdd{0.0f, 0.0f, 0.0f};
    } mFieldRandom;

    struct {
        f32 magnetPower{0.0f};
        Math::Vector3f magnetPos{0.0f, 0.0f, 0.0f};
        BitFlag<FieldMagnetFlag> magnetFlag{
            FieldMagnetFlag::AxisTargetX,
            FieldMagnetFlag::AxisTargetY,
            FieldMagnetFlag::AxisTargetZ
        };
    } mFieldMagnet;

    struct {
        s32 spinRotate{0};
        FieldSpinAxis spinAxis{FieldSpinAxis::AxisX};
    } mFieldSpin;

    struct {
        FieldCollisionType collisionType{FieldCollisionType::Die};
        bool collisionIsWorld{false};
        f32 collisionCoord{0.0f};
        f32 collisionCoef{0.0f};
    } mFieldCollision;

    struct {
        FieldConvergenceType convergenceType{FieldConvergenceType::AssignedPos};
        Math::Vector3f convergencePos{0.0f, 0.0f, 0.0f};
    } mFieldConvergence;

    struct {
        Math::Vector3f posAdd{0.0f, 0.0f, 0.0f};
    } mFieldPosAdd;

    BitFlag<FieldFlag> mFieldFlags{};

    // Child Propertiies
    struct {
        // Basic Properties
        BillboardType billboardType{BillboardType::Billboard};

        // Emission Properties
        s32 emitRate{1};
        s32 emitTiming{0};
        s32 life{10};
        s32 emitStep{1};

        // Velocity Properties
        Math::Vector3f randVel{0.0f, 0.0f, 0.0f};
        Math::Vector3f gravity{0.0f, -1.0f, 0.0f};
        f32 velInheritRate{1.0f};
        f32 initPosRand{0.0f};
        f32 figurVel{0.1f};
        f32 airResist{1.0f};

        // Rotation Properties
        RotType rotType{RotType::None};
        Math::Vector3i initRot{0, 0, 0};
        Math::Vector3i initRotRand{0, 0, 0};
        Math::Vector3i rotVel{0, 0, 0};
        Math::Vector3i rotVelRand{0, 0, 0};
        Math::Vector2f rotBasis{0.0f, 0.0f};

        // Scale properties
        Math::Vector2f scale{1.0f, 1.0f};
        Math::Vector2f scaleTarget{1.0f, 1.0f};
        f32 scaleInheritRate{1.0f};
        s32 scaleStartFrame{0};

        // Texture Properties
        TextureWrap textureWrapT{TextureWrap::ClampToEdge};
        TextureWrap textureWrapS{TextureWrap::ClampToEdge};
        TextureFilter textureMagFilter{TextureFilter::Nearest};
        TextureFilter textureMinFilter{TextureFilter::Nearest};
        TextureMipFilter textureMipFilter{TextureMipFilter::None};
        Math::Vector2f texUVScale{1.0f, 1.0f};
        TextureHandle textureHandle{};

        // Color Properties
        binColor4f color0{1.0f, 1.0f, 1.0f, 1.0f};
        binColor3f color1{255.0f, 255.0f, 255.0f};

        // Alpha Properties
        f32 alpha{1.0f};
        f32 alphaTarget{1.0f};
        f32 alphaInit{1.0f};
        s32 alphaStartFrame{1};
        s32 alphaBaseFrame{1};

        // Combiner Properties
        BlendFuncType blendFunc{BlendFuncType::Translucent};
        DepthFuncType depthFunc{DepthFuncType::Unk0};
        ColorCombinerFuncType combinerFunc{ColorCombinerFuncType::CombinerConfig0};
    } mChild;

    BitFlag<ChildFlag> mChildFlags{
        ChildFlag::AlphaInherit,
        ChildFlag::ScaleInherit,
        ChildFlag::RotateInherit,
        ChildFlag::IsFollow,
        ChildFlag::Unk80
    };
};


// ========================================================================== //


} // namespace Ptcl
