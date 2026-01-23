#include "ptcl/ptclEmitter.h"


namespace Ptcl {


// ========================================================================== //


Emitter::Emitter() {
    m_2C = 0;
    m_2D = 0;
    m_2E = 0;
    m_31 = 0;
    m_8C = 0.0f;
    mBillboardType = BillboardType::Billboard;
}

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

u8 Emitter::_2C() const {
    return m_2C;
}

void Emitter::set_2C(const u8 _2C) {
    m_2C = _2C;
}

u8 Emitter::_2D() const {
    return m_2D;
}

void Emitter::set_2D(const u8 _2D) {
    m_2D = _2D;
}

u8 Emitter::_2E() const {
    return m_2E;
}

void Emitter::set_2E(const u8 _2E) {
    m_2E = _2E;
}

u8 Emitter::_31() const {
    return m_31;
}

void Emitter::set_31(const u8 _31) {
    m_31 = _31;
}

f32 Emitter::_8C() const {
    return m_8C;
}

void Emitter::set_8C(const f32 _8C) {
    m_8C = _8C;
}

BillboardType Emitter::billboardType() const {
    return mBillboardType;
}

void Emitter::setBillboardType(const BillboardType billboardType) {
    mBillboardType = billboardType;
}

const BasicProperties& Emitter::basicProperties() const {
    return mBasicProperties;
}

void Emitter::setBasicProperties(const BasicProperties& basicProperties) {
    mBasicProperties = basicProperties;
}

const GravityProperties& Emitter::gravityProperties() const {
    return mGravityProperties;
}

void Emitter::setGravityProperties(const GravityProperties& gravityProperties) {
    mGravityProperties = gravityProperties;
}

const TransformProperties& Emitter::transformProperties() const {
    return mTransformProperties;
}

void Emitter::setTransformProperties(const TransformProperties& transformProperties) {
    mTransformProperties = transformProperties;
}

const LifespanProperties& Emitter::lifespanProperties() const {
    return mLifespanProperties;
}

void Emitter::setLifespanProperties(const LifespanProperties& lifespanProperties) {
    mLifespanProperties = lifespanProperties;
}

const TerminationProperties& Emitter::terminationProperties() const {
    return mTerminationProperties;
}

void Emitter::setTerminationProperties(const TerminationProperties& terminationProperties) {
    mTerminationProperties = terminationProperties;
}

const EmissionProperties& Emitter::emissionProperties() const {
    return mEmissionProperties;
}

void Emitter::setEmissionProperties(const EmissionProperties& emissionProperties) {
    mEmissionProperties = emissionProperties;
}

const VelocityProperties& Emitter::velocityProperties() const {
    return mVelocityProperties;
}

void Emitter::setVelocityProperties(const VelocityProperties& velocityProperties) {
    mVelocityProperties = velocityProperties;
}

const VolumeProperties& Emitter::volumeProperties() const {
    return mVolumeProperties;
}

void Emitter::setVolumeProperties(const VolumeProperties& volumeProperties) {
    mVolumeProperties = volumeProperties;
}

const ColorProperties& Emitter::colorProperties() const {
    return mColorProperties;
}

void Emitter::setColorProperties(const ColorProperties& colorProperties) {
    mColorProperties = colorProperties;
    // Sync color flags with mFlag
    mColorProperties.colorRandom ? mFlag.set(EmitterFlag::ColorRandom) : mFlag.clear(EmitterFlag::ColorRandom);
    mColorProperties.colorAnimation ? mFlag.set(EmitterFlag::ColorAnimation) : mFlag.clear(EmitterFlag::ColorAnimation);
}

const AlphaProperties& Emitter::alphaProperties() const {
    return mAlphaProperties;
}

void Emitter::setAlphaProperties(const AlphaProperties& alphaProperties) {
    mAlphaProperties = alphaProperties;
}

const ScaleProperties& Emitter::scaleProperties() const {
    return mScaleProperties;
}

void Emitter::setScaleProperties(const ScaleProperties& scaleProperties) {
    mScaleProperties = scaleProperties;
}

const TextureProperties& Emitter::textureProperties() const {
    return mTextureProperties;
}

void Emitter::setTextureProperties(const TextureProperties& textureProperties) {
    mTextureProperties = textureProperties;
}

const RotationProperties& Emitter::rotationProperties() const {
    return mRotationProperties;
}

void Emitter::setRotationProperties(const RotationProperties& rotationProperties) {
    mRotationProperties = rotationProperties;
}

const CombinerProperties& Emitter::combinerProperties() const {
    return mCombinerProperties;
}

void Emitter::setCombinerProperties(const CombinerProperties& combinerProperties) {
    mCombinerProperties = combinerProperties;
}

BitFlag<ChildFlag>& Emitter::childFlags() {
    return mChildFlags;
}

BitFlag<FieldFlag>& Emitter::fieldFlags() {
    return mFieldFlags;
}

BitFlag<FluctuationFlag>& Emitter::fluctuationFlags() {
    return mFluctuationFlags;
}

BitFlag<StripeFlag>& Emitter::stripeFlags() {
    return mStripeFlags;
}

const BitFlag<ChildFlag>& Emitter::childFlags() const {
    return mChildFlags;
}

const BitFlag<FieldFlag>& Emitter::fieldFlags() const {
    return mFieldFlags;
}

const BitFlag<FluctuationFlag>& Emitter::fluctuationFlags() const {
    return mFluctuationFlags;
}

const BitFlag<StripeFlag>& Emitter::stripeFlags() const {
    return mStripeFlags;
}

bool Emitter::hasStripeData() const {
    return mHasStripeData;
}

void Emitter::setHasStripeData(const bool hasStripeData) {
    mHasStripeData = hasStripeData;
}

ChildData& Emitter::childData() {
    return mChildData;
}

FieldRandomData& Emitter::fieldRandomData() {
    return mFieldRandomData;
}

FieldMagnetData& Emitter::fieldMagnetData() {
    return mFieldMagnetData;
}

FieldSpinData& Emitter::fieldSpinData() {
    return mFieldSpinData;
}

FieldCollisionData& Emitter::fieldCollisionData() {
    return mFieldCollisionData;
}

FieldConvergenceData& Emitter::fieldConvergenceData() {
    return mFieldConvergenceData;
}

FieldPosAddData& Emitter::fieldPosAddData() {
    return mFieldPosAddData;
}

FluctuationData& Emitter::fluctuationData() {
    return mFluctuationData;
}

StripeData& Emitter::stripeData() {
    return mStripeData;
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

    m_2C = emitterData._2C;
    m_2D = emitterData._2D;
    m_2E = emitterData._2E;
    m_31 = emitterData._31;

    m_8C = emitterData._8C;
    mBillboardType = emitterData.billboardType;

    mBasicProperties = {
        .type = emitterData.type,
        .followType = emitterData.followType,
        // name - must be set after initialization
        .randomSeed = PtclSeed{emitterData.randomSeed}
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
        .spreadVec = Math::Vector3f(emitterData.spreadVec.x, emitterData.spreadVec.y, emitterData.spreadVec.z)
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
        .combinerFunc = emitterData.colorCombinerFunc
    };
}


// ========================================================================== //


} // namespace Ptcl
