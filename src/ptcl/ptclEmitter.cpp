#include "ptcl/ptclEmitter.h"


namespace Ptcl {


// ========================================================================== //


Emitter::Emitter() {

    mType = EmitterType::Simple;
    mFlag = BitFlag<EmitterFlag>{};
    mRandomSeed = PtclSeed{};
    mName = "";

    // TODO: Texture Handle
    mTextureWrapT = TextureWrap::ClampToEdge;
    mTextureWrapS = TextureWrap::ClampToEdge;
    mTextureMagFilter = TextureFilter::Nearest;
    mTextureMinFilter = TextureFilter::Nearest;
    mTextureMipFilter = TextureMipFilter::None;

    m_2C = 0;
    m_2D = 0;
    m_2E = 0;
    m_30 = 0;
    m_31 = 0;

    m_8C = 0.0f;
    m_90 = 0;
    mBillboardType = BillboardType::Billboard;
    m_98 = 0;

    m_D8 = {0, 0, 0};

    mGravityProperties = {
        .isDirectional = false,
        .gravity = {0.0f, -1.0f, 0.0f}
    };

    mTransformProperties = {
        .transformSRT = {},
        .transformRT = {}
    };

    mLifespanProperties = {
        .ptclLife = 100,
        .ptclLifeRnd = 0
    };

    mTerminationProperties = {
        .isStopEmitInFade = false,
        .alphaAddInFade = 0.0f
    };

    mEmissionProperties = {
        .startFrame = 0,
        .endFrame = 1,
        .lifeStep = 10,
        .lifeStepRnd = 1,
        .emitRate = 1
    };

    mVelocityProperties = {
        .figureVel = 0.0f,
        .emitterVelDir = {0.0f, 0.0f, 0.0f},
        .initVel = 0.0f,
        .initVelRnd = 0.0f
    };

    mVolumeProperties = {
        .volumeTblIndex = 0,
        .volumeType = VolumeType::Point,
        .volumeRadius = {1.0f, 1.0f, 1.0f},
        .volumeSweepStart = 0,
        .volumeSweepParam = 0,
    };

    mColorProperties = {
        .colors {
            binColor4f{0.0f, 0.0f, 0.0f, 1.0f},
            binColor4f{0.0f, 0.0f, 0.0f, 1.0f},
            binColor4f{0.0f, 0.0f, 0.0f, 1.0f},
        },
        .colorSection1 = 20,
        .colorSection2 = 60,
        .colorSection3 = 80,
        .colorNumRepeat = 1,
        .colorRandom = mFlag.isSet(EmitterFlag::ColorRandom),
        .colorAnimation = mFlag.isSet(EmitterFlag::ColorAnimation),
        .colorCalcType = ColorCalcType::Pass1
    };

    mAlphaProperties = {
        .initAlpha = 0.0f,
        .diffAlpha21 = 0.0f,
        .diffAlpha32 = 0.0f,
        .alphaSection1 = 0,
        .alphaSection2 = 0
    };

    mScaleProperties = {
        .initScale = {1.0f, 1.0f},
        .diffScale21 = {0.0f, 0.0f},
        .diffScale32 = {0.0f, 0.0f},
        .scaleSection1 = 0,
        .scaleSection2 = 0,
        .scaleRand = 0.0f
    };

    mRotationProperties = {
        .rotType = RotType::None,
        .initRot = {},
        .initRotRand = {},
        .rotVel = {},
        .rotVelRand = {}
    };

    mFollowType = FollowType::All;
    m_134 = 0;
    m_168 = {0, 0};

    mNumTexPat = 0;
    mNumTexDivX = 1;
    mNumTexDivY = 1;
    mTexUVScale = {1.0f, 1.0f};
    mTexPatTbl = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    mTexPatFreq = 0;
    mTexPatTblUse = 0;
}

EmitterType Emitter::type() const {
    return mType;
}

void Emitter::setType(const EmitterType type) {
    mType = type;
}

BitFlag<EmitterFlag>& Emitter::flags() {
    return mFlag;
}

const BitFlag<EmitterFlag>& Emitter::flags() const {
    return mFlag;
}

PtclSeed& Emitter::randomSeed() {
    return mRandomSeed;
}

const PtclSeed& Emitter::randomSeed() const {
    return mRandomSeed;
}

const QString& Emitter::name() const {
    return mName;
}

void Emitter::setName(const QString& name) {
    mName = name;
}

const TextureHandle& Emitter::textureHandle() const {
    return mTextureHande;
}


TextureHandle& Emitter::textureHandle() {
    return mTextureHande;
}

void Emitter::setTexture(std::shared_ptr<Texture> texture) {
    mTextureHande.set(texture);
}

TextureWrap Emitter::textureWrapT() const {
    return mTextureWrapT;
}

void Emitter::setTextureWrapT(TextureWrap wrap) {
    mTextureWrapT = wrap;
}

TextureWrap Emitter::textureWrapS() const {
    return mTextureWrapS;
}

void Emitter::setTextureWrapS(TextureWrap wrap) {
    mTextureWrapS = wrap;
}

TextureFilter Emitter::textureMagFilter() const {
    return mTextureMagFilter;
}

void Emitter::setTextureMagFilter(TextureFilter filter) {
    mTextureMagFilter = filter;
}

TextureFilter Emitter::textureMinFilter() const {
    return mTextureMinFilter;
}

void Emitter::setTextureMinFilter(TextureFilter filter) {
    mTextureMinFilter = filter;
}

TextureMipFilter Emitter::textureMipFilter() const {
    return mTextureMipFilter;
}

void Emitter::setTextureMipFilter(TextureMipFilter filter) {
    mTextureMipFilter = filter;
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

u8 Emitter::_30() const {
    return m_30;
}

void Emitter::set_30(const u8 _30) {
    m_30 = _30;
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

u32 Emitter::_90() const {
    return m_90;
}

void Emitter::set_90(const u32 _90) {
    m_90 = _90;
}

BillboardType Emitter::billboardType() const {
    return mBillboardType;
}

void Emitter::setBillboardType(const BillboardType billboardType) {
    mBillboardType = billboardType;
}

u32 Emitter::_98() const {
    return m_98;
}

void Emitter::set_98(const u32 _98) {
    m_98 = _98;
}

const std::array<u32, 3>& Emitter::_D8() const {
    return m_D8;
}

void Emitter::set_D8(const std::array<u32, 3>& _D8) {
    m_D8 = _D8;
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

FollowType Emitter::followType() const {
    return mFollowType;
}

void Emitter::setFollowType(const FollowType followType) {
    mFollowType = followType;
}

u32 Emitter::_134() const {
    return m_134;
}

void Emitter::set_134(const u32 _134) {
    m_134 = _134;
}

const RotationProperties& Emitter::rotationProperties() const {
    return mRotationProperties;
}

void Emitter::setRotationProperties(const RotationProperties& rotationProperties) {
    mRotationProperties = rotationProperties;
}

const std::array<u32, 2>& Emitter::_168() const {
    return m_168;
}

void Emitter::set_168(const std::array<u32, 2>& _168) {
    m_168 = _168;
}

u16 Emitter::numTexPat() const {
    return mNumTexPat;
}

void Emitter::setNumTexPat(const u16 numTexPat) {
    mNumTexPat = numTexPat;
}

u8 Emitter::numTexDivX() const {
    return mNumTexDivX;
}

void Emitter::setNumTexDivX(const u8 numTexDivX) {
    mNumTexDivX = numTexDivX;
}

u8 Emitter::numTexDivY() const {
    return mNumTexDivY;
}

void Emitter::setNumTexDivY(const u8 numTexDivY) {
    mNumTexDivY = numTexDivY;
}

const Math::Vector2f& Emitter::texUVScale() const {
    return mTexUVScale;
}

void Emitter::setTexUVScale(const Math::Vector2f& texUVScale) {
    mTexUVScale = texUVScale;
}

const std::array<u8, 16>& Emitter::texPatTbl() const {
    return mTexPatTbl;
}

void Emitter::setTexPatTbl(const std::array<u8, 16>& texPatTbl) {
    mTexPatTbl = texPatTbl;
}

void Emitter::setTexPatTblData(int index, u8 value) {
    if (index > mTexPatTbl.size()) {
        return;
    }

    mTexPatTbl.at(index) = value;
}

u16 Emitter::texPatFreq() const {
    return mTexPatFreq;
}

void Emitter::setTexPatFreq(const u16 texPatFreq) {
    mTexPatFreq = texPatFreq;
}

u16 Emitter::texPatTblUse() const {
    return mTexPatTblUse;
}

void Emitter::setTexPatTblUse(const u16 texPatTblUse) {
    mTexPatTblUse = texPatTblUse;
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
    mType = emitterData.type;
    mFlag = emitterData.flag;
    mRandomSeed = PtclSeed{emitterData.randomSeed};
    // mName
    // mTexture
    // mTextureFormat
    mTextureWrapT = emitterData.textureRes.wrapT;
    mTextureWrapS = emitterData.textureRes.wrapS;
    mTextureMagFilter = emitterData.textureRes.magFilter;
    mTextureMinFilter = static_cast<TextureFilter>(emitterData.textureRes.minMipFilter & 0x1);
    mTextureMipFilter = static_cast<TextureMipFilter>((emitterData.textureRes.minMipFilter >> 1) & 0x3);

    m_2C = emitterData._2C;
    m_2D = emitterData._2D;
    m_2E = emitterData._2E;
    m_30 = emitterData._30;
    m_31 = emitterData._31;

    m_8C = emitterData._8C;
    m_90 = emitterData._90;
    mBillboardType = emitterData.billboardType;
    m_98 = emitterData._98;

    std::ranges::copy(emitterData._D8, m_D8.begin());

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
        .colorCalcType = static_cast<ColorCalcType>(emitterData.rotCalcType / 5)
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
    };

    mFollowType = emitterData.followType;
    m_134 = emitterData._134;

    std::ranges::copy(emitterData._168, m_168.begin());

    mNumTexPat = emitterData.numTexPat;
    mNumTexDivX = emitterData.numTexDivX;
    mNumTexDivY = emitterData.numTexDivY;
    mTexUVScale = Math::Vector2f(emitterData.texUVScale.x, emitterData.texUVScale.y);
    std::ranges::copy(emitterData.texPatTbl, mTexPatTbl.begin());
    mTexPatFreq = emitterData.texPatFreq;
    mTexPatTblUse = emitterData.texPatTblUse;
}


// ========================================================================== //


} // namespace Ptcl
