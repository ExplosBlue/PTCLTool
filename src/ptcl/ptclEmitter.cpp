#include "ptcl/ptclEmitter.h"


namespace Ptcl {


// ========================================================================== //

std::unique_ptr<Emitter> Emitter::clone() const {
    auto newEmitter = std::make_unique<Emitter>();

    newEmitter->mFlag = mFlag;

    // Basic Properties
    newEmitter->mType = mType;
    newEmitter->mFollowType = mFollowType;
    newEmitter->mName = mName;
    newEmitter->mRandomSeed = mRandomSeed;
    newEmitter->mBillboardType = mBillboardType;
    newEmitter->mIsPolygon = mIsPolygon;
    newEmitter->mIsVelLook = mIsVelLook;
    newEmitter->mIsEmitterBillboardMtx = mIsEmitterBillboardMtx;
    newEmitter->mIsFollow = mIsFollow;

    // Gravity Properties
    newEmitter->mIsDirectional = mIsDirectional;
    newEmitter->mGravity = mGravity;

    // Lifespan Properties
    newEmitter->mPtclLife = mPtclLife;
    newEmitter->mPtclLifeRnd = mPtclLifeRnd;

    // Transform Properties
    newEmitter->mTransformSRT = mTransformSRT;
    newEmitter->mTransformRT = mTransformRT;

    // Termination Properties
    newEmitter->mIsStopEmitInFade = mIsStopEmitInFade;
    newEmitter->mAlphaAddInFade = mAlphaAddInFade;

    // Emission Properties
    newEmitter->mEmitStartFrame = mEmitStartFrame;
    newEmitter->mEmitEndFrame = mEmitEndFrame;
    newEmitter->mLifeStep = mLifeStep;
    newEmitter->mLifeStepRnd = mLifeStepRnd;
    newEmitter->mEmitRate = mEmitRate;

    // Velocity Properties
    newEmitter->mFigureVelocity = mFigureVelocity;
    newEmitter->mVelocityDir = mVelocityDir;
    newEmitter->mInitVelocity = mInitVelocity;
    newEmitter->mInitVelocityRnd = mInitVelocityRnd;
    newEmitter->mSpreadVec = mSpreadVec;
    newEmitter->mAirResistance = mAirResistance;

    // Volume Properties
    newEmitter->mVolumeTblIndex = mVolumeTblIndex;
    newEmitter->mVolumeType = mVolumeType;
    newEmitter->mVolumeRadius = mVolumeRadius;
    newEmitter->mVolumeSweepStart = mVolumeSweepStart;
    newEmitter->mVolumeSweepParam = mVolumeSweepParam;

    // Color Properties
    newEmitter->mColor0 = mColor0;
    newEmitter->mColorSection1 = mColorSection1;
    newEmitter->mColorSection2 = mColorSection2;
    newEmitter->mColorSection3 = mColorSection3;
    newEmitter->mColorNumRepeat = mColorNumRepeat;
    newEmitter->mColorCalcType = mColorCalcType;
    newEmitter->mColor1 = mColor1;

    // Alpha Properties
    newEmitter->mAlphaAnim = mAlphaAnim;

    // Scale Properties
    newEmitter->mScaleAnim = mScaleAnim;
    newEmitter->mScaleRand = mScaleRand;

    // Rotation Properties
    newEmitter->mRotType = mRotType;
    newEmitter->mInitRot = mInitRot;
    newEmitter->mInitRotRand = mInitRotRand;
    newEmitter->mRotVel = mRotVel;
    newEmitter->mRotVelRand = mRotVelRand;
    newEmitter->mRotBasis = mRotBasis;

    // Combiner Properties
    newEmitter->mBlendFunc = mBlendFunc;
    newEmitter->mDepthFunc = mDepthFunc;
    newEmitter->mCombinerFunc = mCombinerFunc;

    newEmitter->mTextureProperties = mTextureProperties;
    newEmitter->mTextureHandle = mTextureHandle.clone();
    newEmitter->mComplexProperties = mComplexProperties;
    newEmitter->mChildData = std::move(*mChildData.clone());
    newEmitter->mFluctuationData = mFluctuationData;
    newEmitter->mFieldData = mFieldData;
    newEmitter->mStripeData = mStripeData;
    return newEmitter;
}

BitFlag<EmitterFlag>& Emitter::flags() {
    return mFlag;
}

const BitFlag<EmitterFlag>& Emitter::flags() const {
    return mFlag;
}

const TextureHandle& Emitter::textureHandle() const {
    return mTextureHandle;
}

TextureHandle& Emitter::textureHandle() {
    return mTextureHandle;
}

void Emitter::setTexture(const std::shared_ptr<Texture>& texture) {
    mTextureHandle.set(texture);
}

const Emitter::TextureProperties& Emitter::textureProperties() const {
    return mTextureProperties;
}

void Emitter::setTextureProperties(const TextureProperties& textureProperties) {
    mTextureProperties = textureProperties;
}

const Emitter::ComplexProperties& Emitter::complexProperties() const {
    return mComplexProperties;
}

void Emitter::setComplexProperties(const ComplexProperties& complexProperties) {
    mComplexProperties = complexProperties;
}

void Emitter::setChildFlags(const BitFlag<ChildFlag>& childFlags) {
    mComplexProperties.childFlags = childFlags;
}

void Emitter::setFluctuationFlags(const BitFlag<FluctuationFlag>& fluxFlags) {
    mComplexProperties.fluctuationFlags = fluxFlags;
}

void Emitter::setFieldFlags(const BitFlag<FieldFlag>& fieldFlags) {
    mComplexProperties.fieldFlags = fieldFlags;
}

void Emitter::setStripeFlags(const BitFlag<StripeFlag>& stripeFlags) {
    mComplexProperties.stripeFlags = stripeFlags;
}

const ChildData& Emitter::childData() const {
    return mChildData;
}

ChildData& Emitter::childData() {
    return mChildData;
}

const FluctuationData& Emitter::fluctuationData() const {
    return mFluctuationData;
}

void Emitter::setFluctuationData(const FluctuationData& fluctuationData) {
    mFluctuationData = fluctuationData;
}

void Emitter::initFluctuationData(const BinFluctuationData& fluctuationData) {
    mFluctuationData = {
        .fluctuationScale = fluctuationData.fluctuationScale,
        .fluctuationFreq = fluctuationData.fluctuationFreq,
        .fluctuationPhaseRnd = static_cast<bool>(fluctuationData.fluctuationPhaseRnd)
    };
}

const FieldData& Emitter::fieldData() const {
    return mFieldData;
}

FieldData& Emitter::fieldData() {
    return mFieldData;
}

void Emitter::setFieldData(const FieldData& fieldData) {
    mFieldData = fieldData;
}

const StripeData& Emitter::stripeData() const {
    return mStripeData;
}

void Emitter::setStripeData(const StripeData& stripeData) {
    mStripeData = stripeData;
}

void Emitter::initStripeData(const BinStripeData& stripeData) {
    mStripeData = {
        .type = stripeData.stripeType,
        .numHistory = stripeData.stripeNumHistory,
        .startAlpha = stripeData.stripeStartAlpha,
        .endAlpha = stripeData.stripeEndAlpha,
        .uvScrollSpeed = {stripeData.uvScrollSpeed.x, stripeData.uvScrollSpeed.y},
        .historyStep = stripeData.stripeHistoryStep,
        .dirInterpolate = stripeData.stripeDirInterpolate
    };
}

bool Emitter::hasStripeData() const {
    const auto bType = mBillboardType;
    const bool isBillboardStripe = bType == BillboardType::Stripe || bType == BillboardType::ComplexStripe;
    return mType != EmitterType::Simple && isBillboardStripe;
}

void Emitter::initFromBinary(const BinCommonEmitterData& emitterData) {
    mFlag = emitterData.flag;

    mTextureProperties = {
        .textureWrapT = emitterData.textureRes.wrapT,
        .textureWrapS = emitterData.textureRes.wrapS,
        .textureMagFilter = emitterData.textureRes.magFilter,
        .textureMinFilter = static_cast<TextureFilter>(emitterData.textureRes.minMipFilter & 0x1),
        .textureMipFilter = static_cast<TextureMipFilter>((emitterData.textureRes.minMipFilter >> 1) & 0x3),
        .numTexPat = emitterData.numTexPat,
        .numTexDivX = emitterData.numTexDivX,
        .numTexDivY = emitterData.numTexDivY,
        .texUVScale = { emitterData.texUVScale.x, emitterData.texUVScale.y },
        .texPatTbl = emitterData.texPatTbl,
        .texPatFreq = emitterData.texPatFreq,
        .texPatTblUse = emitterData.texPatTblUse,
        .isTexPatAnim = emitterData.isTexPatAnim
    };

    // Basic Properties
    mType = emitterData.type;
    mFollowType = emitterData.followType;
    // name - must be set after initialization
    mRandomSeed = PtclSeed{emitterData.randomSeed};
    mBillboardType = emitterData.billboardType;
    mIsPolygon = emitterData.isPolygon;
    mIsVelLook = emitterData.isVelLook;
    mIsEmitterBillboardMtx = emitterData.isEmitterBillboardMtx;
    mIsFollow = emitterData.isFollow;

    // Gravity Properties
    mIsDirectional = emitterData.isDirectional;
    mGravity = Math::Vector3f(emitterData.gravity.x, emitterData.gravity.y, emitterData.gravity.z);

    // Lifespan Properties
    mPtclLife = emitterData.ptclLife;
    mPtclLifeRnd = emitterData.ptclLifeRnd;

    // Transform Properties
    mTransformSRT = emitterData.transformSRT.toMatrix34f();
    mTransformRT = emitterData.transformRT.toMatrix34f();

    // Termination Properties
    mIsStopEmitInFade = emitterData.isStopEmitInFade;
    mAlphaAddInFade = emitterData.alphaAddInFade;

    // Emission Properties
    mEmitStartFrame = emitterData.startFrame;
    mEmitEndFrame = emitterData.endFrame;
    mLifeStep = emitterData.lifeStep;
    mLifeStepRnd = emitterData.lifeStepRnd;
    mEmitRate = emitterData.emitRate;

    // Velocity Properties
    mFigureVelocity = emitterData.figureVel;
    mVelocityDir = Math::Vector3f(emitterData.emitterVelDir.x, emitterData.emitterVelDir.y, emitterData.emitterVelDir.z);
    mInitVelocity = emitterData.initVel;
    mInitVelocityRnd = emitterData.initVelRnd;
    mSpreadVec = Math::Vector3f(emitterData.spreadVec.x, emitterData.spreadVec.y, emitterData.spreadVec.z);
    mAirResistance = emitterData.airResistance;

    // Volume Properties
    mVolumeTblIndex = emitterData.volumeTblIndex;
    mVolumeType = emitterData.volumeType;
    mVolumeRadius = Math::Vector3f(emitterData.volumeRadius.x, emitterData.volumeRadius.y, emitterData.volumeRadius.z);
    mVolumeSweepStart = emitterData.volumeSweepStart;
    mVolumeSweepParam = emitterData.volumeSweepParam;

    // Color Properties
    mColor0 = emitterData.color0;
    mColorSection1 = emitterData.colorSection1;
    mColorSection2 = emitterData.colorSection2;
    mColorSection3 = emitterData.colorSection3;
    mColorNumRepeat = emitterData.colorNumRepeat;
    mColorCalcType = static_cast<ColorCalcType>(emitterData.rotCalcType / 5);
    mColor1 = emitterData.color1;

    // Alpha Properties
    mAlphaAnim = {
        .initAlpha = emitterData.initAlpha,
        .diffAlpha21 = emitterData.diffAlpha21,
        .diffAlpha32 = emitterData.diffAlpha32,
        .alphaSection1 = emitterData.alphaSection1,
        .alphaSection2 = emitterData.alphaSection2,
    };

    // Scale Properties
    mScaleAnim = {
        .initScale = Math::Vector2f(emitterData.initScale.x, emitterData.initScale.y),
        .diffScale21 = Math::Vector2f(emitterData.diffScale21.x, emitterData.diffScale21.y),
        .diffScale32 = Math::Vector2f(emitterData.diffScale32.x, emitterData.diffScale32.y),
        .scaleSection1 = emitterData.scaleSection1,
        .scaleSection2 = emitterData.scaleSection2,
    };

    mScaleRand = emitterData.scaleRand;

    // Rotation Properties
    mRotType = static_cast<RotType>(emitterData.rotCalcType % 5);
    mInitRot = Math::Vector3i(emitterData.initRot.x, emitterData.initRot.y, emitterData.initRot.z);
    mInitRotRand = Math::Vector3i(emitterData.initRotRand.x, emitterData.initRotRand.y, emitterData.initRotRand.z);
    mRotVel = Math::Vector3i(emitterData.rotVel.x, emitterData.rotVel.y, emitterData.rotVel.z);
    mRotVelRand = Math::Vector3i(emitterData.rotVelRand.x, emitterData.rotVelRand.y, emitterData.rotVelRand.z);
    mRotBasis = Math::Vector2f(emitterData.rotBasis.x, emitterData.rotBasis.y);

    // Combiner Properties
    mBlendFunc = emitterData.blendFunc;
    mDepthFunc = emitterData.depthFunc;
    mCombinerFunc = emitterData.colorCombinerFunc;
}

void Emitter::initComplexFromBinary(const BinComplexEmitterData& emitterData) {
    mComplexProperties = {
        .childFlags = emitterData.childFlag,
        .fieldFlags = emitterData.fieldFlag,
        .fluctuationFlags = emitterData.fluctuationFlag,
        .stripeFlags = emitterData.stripeFlag
    };
}


// ========================================================================== //


} // namespace Ptcl
