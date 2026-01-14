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
    m_2F = 0;
    m_30 = 0;
    m_31 = 0;
    m_33 = 0;

    m_80 = 0;
    mPtclLife = 0;
    mPtclLifeRnd = 0;
    m_8C = 0.0f;
    m_90 = 0;
    mBillboardType = BillboardType::Billboard;
    m_98 = 0;
    m_9C = 0.0f;
    m_A0 = 0.0f;
    m_A4 = 0.0f;

    m_D8 = {0, 0, 0};


    mEmissionProperties = {
        .startFrame = 0,
        .endFrame = 1,
        .lifeStep = 10,
        .lifeStepRnd = 1
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

    mCombinerType = CombinerType::None;
    mFollowType = FollowType::All;
    m_134 = 0;
    m_168 = {0, 0};
    mTransformSRT = {};
    mTransformRT = {};
    mAlphaAddInFade = 0.0f;

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

u8 Emitter::_2F() const {
    return m_2F;
}

void Emitter::set_2F(const u8 _2F) {
    m_2F = _2F;
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

u8 Emitter::_33() const {
    return m_33;
}

void Emitter::set_33(const u8 _33) {
    m_33 = _33;
}

u32 Emitter::_80() const {
    return m_80;
}

void Emitter::set_80(const u32 _80) {
    m_80 = _80;
}

s32 Emitter::ptclLife() const {
    return mPtclLife;
}

void Emitter::setPtclLife(const s32 ptclLife) {
    mPtclLife = ptclLife;
}

s32 Emitter::ptclLifeRnd() const {
    return mPtclLifeRnd;
}

void Emitter::setPtclLifeRnd(const s32 ptclLifeRnd) {
    mPtclLifeRnd = ptclLifeRnd;
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

f32 Emitter::_9C() const {
    return m_9C;
}

void Emitter::set_9C(const f32 _9C) {
    m_9C = _9C;
}

f32 Emitter::_A0() const {
    return m_A0;
}

void Emitter::set_A0(const f32 _A0) {
    m_A0 = _A0;
}

f32 Emitter::_A4() const {
    return m_A4;
}

void Emitter::set_A4(const f32 _A4) {
    m_A4 = _A4;
}

const std::array<u32, 3>& Emitter::_D8() const {
    return m_D8;
}

void Emitter::set_D8(const std::array<u32, 3>& _D8) {
    m_D8 = _D8;
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

CombinerType Emitter::combinerType() const {
    return mCombinerType;
}

void Emitter::setCombinerType(const CombinerType combinerType) {
    mCombinerType = combinerType;
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

const QMatrix3x4& Emitter::transformSRT() const {
    return mTransformSRT;
}

void Emitter::setTransformSRT(const QMatrix3x4& transformSRT) {
    mTransformSRT = transformSRT;
}

const QMatrix3x4& Emitter::transformRT() const {
    return mTransformRT;
}

void Emitter::setTransformRT(const QMatrix3x4& transformRT) {
    mTransformRT = transformRT;
}

f32 Emitter::alphaAddInFade() const {
    return mAlphaAddInFade;
}

void Emitter::setAlphaAddInFade(const f32 alphaAddInFade) {
    mAlphaAddInFade = alphaAddInFade;
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

const QVector2D& Emitter::texUVScale() const {
    return mTexUVScale;
}

void Emitter::setTexUVScale(const QVector2D& texUVScale) {
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
    m_2F = emitterData._2F;
    m_30 = emitterData._30;
    m_31 = emitterData._31;
    m_33 = emitterData._33;

    m_80 = emitterData._80;
    mPtclLife = emitterData.ptclLife;
    mPtclLifeRnd = emitterData.ptclLifeRnd;
    m_8C = emitterData._8C;
    m_90 = emitterData._90;
    mBillboardType = emitterData.billboardType;
    m_98 = emitterData._98;
    m_9C = emitterData._9C;
    m_A0 = emitterData._A0;
    m_A4 = emitterData._A4;


    std::ranges::copy(emitterData._D8, m_D8.begin());

    mEmissionProperties = {
        .startFrame = emitterData.startFrame,
        .endFrame = emitterData.endFrame,
        .lifeStep = emitterData.lifeStep,
        .lifeStepRnd = emitterData.lifeStepRnd
    };

    mVelocityProperties = {
        .figureVel = emitterData.figureVel,
        .emitterVelDir = QVector3D(emitterData.emitterVelDir.x, emitterData.emitterVelDir.y, emitterData.emitterVelDir.z),
        .initVel = emitterData.initVel,
        .initVelRnd = emitterData.initVelRnd,
        .spreadVec = QVector3D(emitterData.spreadVec.x, emitterData.spreadVec.y, emitterData.spreadVec.z)
    };

    mVolumeProperties = {
        .volumeTblIndex = emitterData.volumeTblIndex,
        .volumeType = emitterData.volumeType,
        .volumeRadius = QVector3D(emitterData.volumeRadius.x, emitterData.volumeRadius.y, emitterData.volumeRadius.z),
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
    };

    mAlphaProperties = {
        .initAlpha = emitterData.initAlpha,
        .diffAlpha21 = emitterData.diffAlpha21,
        .diffAlpha32 = emitterData.diffAlpha32,
        .alphaSection1 = emitterData.alphaSection1,
        .alphaSection2 = emitterData.alphaSection2,
    };

    mScaleProperties = {
        .initScale = QVector2D(emitterData.initScale.x, emitterData.initScale.y),
        .diffScale21 = QVector2D(emitterData.diffScale21.x, emitterData.diffScale21.y),
        .diffScale32 = QVector2D(emitterData.diffScale32.x, emitterData.diffScale32.y),
        .scaleSection1 = emitterData.scaleSection1,
        .scaleSection2 = emitterData.scaleSection2,
        .scaleRand = emitterData.scaleRand
    };

    mRotationProperties = {
        .rotType = static_cast<RotType>(emitterData.rotCombinerType % 5),
        .initRot = emitterData.initRot,
        .initRotRand = emitterData.initRotRand,
        .rotVel = emitterData.rotVel,
        .rotVelRand = emitterData.rotVelRand,
    };

    mCombinerType = static_cast<CombinerType>(emitterData.rotCombinerType / 5);
    mFollowType = emitterData.followType;
    m_134 = emitterData._134;

    std::ranges::copy(emitterData._168, m_168.begin());

    mTransformSRT = emitterData.transformSRT.toQMatrix3x4();
    mTransformRT = emitterData.transformRT.toQMatrix3x4();
    mAlphaAddInFade = emitterData.alphaAddInFade;
    mNumTexPat = emitterData.numTexPat;
    mNumTexDivX = emitterData.numTexDivX;
    mNumTexDivY = emitterData.numTexDivY;
    mTexUVScale = QVector2D(emitterData.texUVScale.x, emitterData.texUVScale.y);
    std::ranges::copy(emitterData.texPatTbl, mTexPatTbl.begin());
    mTexPatFreq = emitterData.texPatFreq;
    mTexPatTblUse = emitterData.texPatTblUse;
}


// ========================================================================== //


} // namespace Ptcl
