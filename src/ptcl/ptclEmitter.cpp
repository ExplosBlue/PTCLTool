#include "ptcl/ptclEmitter.h"


namespace Ptcl {


// ========================================================================== //


EmitterType Emitter::type() const {
    return mBasicProperties.type;
}

BitFlag<EmitterFlag>& Emitter::flags() {
    return mFlag;
}

const BitFlag<EmitterFlag>& Emitter::flags() const {
    return mFlag;
}

const QString& Emitter::name() const {
    return mBasicProperties.name;
}

void Emitter::setName(const QString& name) {
    mBasicProperties.name = name;
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

const Emitter::BasicProperties& Emitter::basicProperties() const {
    return mBasicProperties;
}

void Emitter::setBasicProperties(const BasicProperties& basicProperties) {
    mBasicProperties = basicProperties;
}

const Emitter::GravityProperties& Emitter::gravityProperties() const {
    return mGravityProperties;
}

void Emitter::setGravityProperties(const GravityProperties& gravityProperties) {
    mGravityProperties = gravityProperties;
}

const Emitter::TransformProperties& Emitter::transformProperties() const {
    return mTransformProperties;
}

void Emitter::setTransformProperties(const TransformProperties& transformProperties) {
    mTransformProperties = transformProperties;
}

const Emitter::LifespanProperties& Emitter::lifespanProperties() const {
    return mLifespanProperties;
}

void Emitter::setLifespanProperties(const LifespanProperties& lifespanProperties) {
    mLifespanProperties = lifespanProperties;
}

const Emitter::TerminationProperties& Emitter::terminationProperties() const {
    return mTerminationProperties;
}

void Emitter::setTerminationProperties(const TerminationProperties& terminationProperties) {
    mTerminationProperties = terminationProperties;
}

const Emitter::EmissionProperties& Emitter::emissionProperties() const {
    return mEmissionProperties;
}

void Emitter::setEmissionProperties(const EmissionProperties& emissionProperties) {
    mEmissionProperties = emissionProperties;
}

const Emitter::VelocityProperties& Emitter::velocityProperties() const {
    return mVelocityProperties;
}

void Emitter::setVelocityProperties(const VelocityProperties& velocityProperties) {
    mVelocityProperties = velocityProperties;
}

const Emitter::VolumeProperties& Emitter::volumeProperties() const {
    return mVolumeProperties;
}

void Emitter::setVolumeProperties(const VolumeProperties& volumeProperties) {
    mVolumeProperties = volumeProperties;
}

const Emitter::ColorProperties& Emitter::colorProperties() const {
    return mColorProperties;
}

void Emitter::setColorProperties(const ColorProperties& colorProperties) {
    mColorProperties = colorProperties;
    // Sync color flags with mFlag
    mFlag.set(EmitterFlag::ColorRandom, mColorProperties.colorRandom);
    mFlag.set(EmitterFlag::ColorAnimation, mColorProperties.colorAnimation);
}

const Emitter::AlphaProperties& Emitter::alphaProperties() const {
    return mAlphaProperties;
}

void Emitter::setAlphaProperties(const AlphaProperties& alphaProperties) {
    mAlphaProperties = alphaProperties;
}

const Emitter::ScaleProperties& Emitter::scaleProperties() const {
    return mScaleProperties;
}

void Emitter::setScaleProperties(const ScaleProperties& scaleProperties) {
    mScaleProperties = scaleProperties;
}

const Emitter::TextureProperties& Emitter::textureProperties() const {
    return mTextureProperties;
}

void Emitter::setTextureProperties(const TextureProperties& textureProperties) {
    mTextureProperties = textureProperties;
}

const Emitter::RotationProperties& Emitter::rotationProperties() const {
    return mRotationProperties;
}

void Emitter::setRotationProperties(const RotationProperties& rotationProperties) {
    mRotationProperties = rotationProperties;
}

const Emitter::CombinerProperties& Emitter::combinerProperties() const {
    return mCombinerProperties;
}

void Emitter::setCombinerProperties(const CombinerProperties& combinerProperties) {
    mCombinerProperties = combinerProperties;
    mFlag.set(EmitterFlag::EnableFog, mCombinerProperties.isFogEnabled);
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
        .fluctuationPhaseRnd = fluctuationData.fluctuationPhaseRnd
    };
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
    const auto bType = mBasicProperties.billboardType;
    const bool isBillboardStripe = bType == BillboardType::Stripe || bType == BillboardType::ComplexStripe;
    return mBasicProperties.type != EmitterType::Simple && isBillboardStripe;
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

    mBasicProperties = {
        .type = emitterData.type,
        .followType = emitterData.followType,
        // name - must be set after initialization
        .randomSeed = PtclSeed{emitterData.randomSeed},
        .billboardType = emitterData.billboardType,
        .isPolygon = emitterData.isPolygon,
        .isVelLook = emitterData.isVelLook,
        .isEmitterBillboardMtx = emitterData.isEmitterBillboardMtx,
        .isFollow = emitterData.isFollow
    };

    mGravityProperties = {
        .isDirectional = emitterData.isDirectional,
        .gravity = Math::Vector3f(emitterData.gravity.x, emitterData.gravity.y, emitterData.gravity.z)
    };

    mTransformProperties = {
        .transformSRT = emitterData.transformSRT.toMatrix34f(),
        .transformRT = emitterData.transformRT.toMatrix34f()
    };

    mLifespanProperties = {
        .ptclLife = emitterData.ptclLife,
        .ptclLifeRnd = emitterData.ptclLifeRnd
    };

    mTerminationProperties = {
        .isStopEmitInFade = emitterData.isStopEmitInFade,
        .alphaAddInFade = emitterData.alphaAddInFade
    };

    mEmissionProperties = {
        .startFrame = emitterData.startFrame,
        .endFrame = emitterData.endFrame,
        .lifeStep = emitterData.lifeStep,
        .lifeStepRnd = emitterData.lifeStepRnd,
        .emitRate = emitterData.emitRate
    };

    mVelocityProperties = {
        .figureVel = emitterData.figureVel,
        .emitterVelDir = Math::Vector3f(emitterData.emitterVelDir.x, emitterData.emitterVelDir.y, emitterData.emitterVelDir.z),
        .initVel = emitterData.initVel,
        .initVelRnd = emitterData.initVelRnd,
        .spreadVec = Math::Vector3f(emitterData.spreadVec.x, emitterData.spreadVec.y, emitterData.spreadVec.z),
        .airResistance = emitterData.airResistance
    };

    mVolumeProperties = {
        .volumeTblIndex = emitterData.volumeTblIndex,
        .volumeType = emitterData.volumeType,
        .volumeRadius = Math::Vector3f(emitterData.volumeRadius.x, emitterData.volumeRadius.y, emitterData.volumeRadius.z),
        .volumeSweepStart = emitterData.volumeSweepStart,
        .volumeSweepParam = emitterData.volumeSweepParam,
    };

    mColorProperties = {
        .colors = emitterData.color,
        .colorSection1 = emitterData.colorSection1,
        .colorSection2 = emitterData.colorSection2,
        .colorSection3 = emitterData.colorSection3,
        .colorNumRepeat = emitterData.colorNumRepeat,
        .colorRandom = emitterData.flag.isSet(EmitterFlag::ColorRandom),
        .colorAnimation = emitterData.flag.isSet(EmitterFlag::ColorAnimation),
        .colorCalcType = static_cast<ColorCalcType>(emitterData.rotCalcType / 5),
        .color0 = emitterData.color0
    };

    mAlphaProperties = {
        .initAlpha = emitterData.initAlpha,
        .diffAlpha21 = emitterData.diffAlpha21,
        .diffAlpha32 = emitterData.diffAlpha32,
        .alphaSection1 = emitterData.alphaSection1,
        .alphaSection2 = emitterData.alphaSection2,
    };

    mScaleProperties = {
        .initScale = Math::Vector2f(emitterData.initScale.x, emitterData.initScale.y),
        .diffScale21 = Math::Vector2f(emitterData.diffScale21.x, emitterData.diffScale21.y),
        .diffScale32 = Math::Vector2f(emitterData.diffScale32.x, emitterData.diffScale32.y),
        .scaleSection1 = emitterData.scaleSection1,
        .scaleSection2 = emitterData.scaleSection2,
        .scaleRand = emitterData.scaleRand
    };

    mRotationProperties = {
        .rotType = static_cast<RotType>(emitterData.rotCalcType % 5),
        .initRot = Math::Vector3i(emitterData.initRot.x, emitterData.initRot.y, emitterData.initRot.z),
        .initRotRand = Math::Vector3i(emitterData.initRotRand.x, emitterData.initRotRand.y, emitterData.initRotRand.z),
        .rotVel = Math::Vector3i(emitterData.rotVel.x, emitterData.rotVel.y, emitterData.rotVel.z),
        .rotVelRand = Math::Vector3i(emitterData.rotVelRand.x, emitterData.rotVelRand.y, emitterData.rotVelRand.z),
        .rotBasis = Math::Vector2f(emitterData.rotBasis.x, emitterData.rotBasis.y)
    };

    mCombinerProperties = {
        .blendFunc = emitterData.blendFunc,
        .depthFunc = emitterData.depthFunc,
        .combinerFunc = emitterData.colorCombinerFunc,
        .isFogEnabled = emitterData.flag.isSet(EmitterFlag::EnableFog)
    };
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
