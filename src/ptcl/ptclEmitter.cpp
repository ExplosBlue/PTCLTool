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
    mVolumeTblIndex = 0;
    m_33 = 0;
    mVolumeType = VolumeType::Point;
    mVolumeRadius = {1.0f, 1.0f, 1.0f};
    mVolumeSweepStart = 0;
    mVolumeSweepParam = 0;
    mFigureVel = 0.0f;
    mEmitterVelDir = {0.0f, 0.0f, 0.0f};
    m_5C = 0.0f;
    mInitVelRnd = 0.0f;
    mSpreadVec = {0.0f, 0.0f, 0.0f};
    m_70 = {0, 0, 0, 0};
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
    mColors = {
        binColor4f{0.0f, 0.0f, 0.0f, 1.0f},
        binColor4f{0.0f, 0.0f, 0.0f, 1.0f},
        binColor4f{0.0f, 0.0f, 0.0f, 1.0f},
    };
    m_D8 = {0, 0, 0};
    mColorSection1 = 20;
    mColorSection2 = 60;
    mColorSection3 = 80;
    mColorNumRepeat = 1;
    mInitAlpha = 0.0f;
    mDiffAlpha21 = 0.0f;
    mDiffAlpha32 = 0.0f;
    mAlphaSection1 = 0;
    mAlphaSection2 = 0;
    mInitScale = {1.0f, 1.0f};
    mDiffScale21 = {0.0f, 0.0f};
    mDiffScale32 = {0.0f, 0.0f};
    mScaleSection1 = 0;
    mScaleSection2 = 0;
    mScaleRand = 0.0f;
    mRotType = RotType::None;
    mCombinerType = CombinerType::None;
    mFollowType = FollowType::All;
    m_134 = 0;
    mInitRot = {};
    mInitRotRand = {};
    mRotVel = {};
    mRotVelRand = {};
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

u8 Emitter::volumeTblIndex() const {
    return mVolumeTblIndex;
}

void Emitter::setVolumeTblIndex(const u8 volumeTblIndex) {
    mVolumeTblIndex = volumeTblIndex;
}

u8 Emitter::_33() const {
    return m_33;
}

void Emitter::set_33(const u8 _33) {
    m_33 = _33;
}

VolumeType Emitter::volumeType() const {
    return mVolumeType;
}

void Emitter::setVolumeType(const VolumeType volumeType) {
    mVolumeType = volumeType;
}

const QVector3D& Emitter::volumeRadius() const {
    return mVolumeRadius;
}

void Emitter::setVolumeRadius(const QVector3D& volumeRadius) {
    mVolumeRadius = volumeRadius;
}

s32 Emitter::volumeSweepStart() const {
    return mVolumeSweepStart;
}

void Emitter::setVolumeSweepStart(const s32 volumeSweepStart) {
    mVolumeSweepStart = volumeSweepStart;
}

u32 Emitter::volumeSweepParam() const {
    return mVolumeSweepParam;
}

void Emitter::setVolumeSweepParam(const u32 volumeSweepParam) {
    mVolumeSweepParam = volumeSweepParam;
}

f32 Emitter::figureVel() const {
    return mFigureVel;
}

void Emitter::setFigureVel(const f32 figureVel) {
    mFigureVel = figureVel;
}

const QVector3D& Emitter::emitterVelDir() const {
    return mEmitterVelDir;
}

void Emitter::setEmitterVelDir(const QVector3D& emitterVelDir) {
    mEmitterVelDir = emitterVelDir;
}

f32 Emitter::_5C() const {
    return m_5C;
}

void Emitter::set_5C(const f32 _5C) {
    m_5C = _5C;
}

f32 Emitter::initVelRnd() const {
    return mInitVelRnd;
}

void Emitter::setInitVelRnd(const f32 initVelRnd) {
    mInitVelRnd = initVelRnd;
}

const QVector3D& Emitter::spreadVec() const {
    return mSpreadVec;
}

void Emitter::setSpreadVec(const QVector3D& spreadVec) {
    mSpreadVec = spreadVec;
}

const std::array<u32, 4>& Emitter::_70() const {
    return m_70;
}

void Emitter::set_70(const std::array<u32, 4>& _70) {
    m_70 = _70;
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

const std::array<binColor4f, 3>& Emitter::colors() const {
    return mColors;
}

void Emitter::setColors(const std::array<binColor4f, 3>& colors) {
    mColors = colors;
}

void Emitter::setColor(int index, const binColor4f& color) {
    if (index >= 3) {
        qWarning() << "invalid color index";
        return;
    }

    mColors[index] = color;
}

const std::array<u32, 3>& Emitter::_D8() const {
    return m_D8;
}

void Emitter::set_D8(const std::array<u32, 3>& _D8) {
    m_D8 = _D8;
}

s32 Emitter::colorSection1() const {
    return mColorSection1;
}

void Emitter::setColorSection1(const s32 colorSection1) {
    mColorSection1 = colorSection1;
}

s32 Emitter::colorSection2() const {
    return mColorSection2;
}

void Emitter::setColorSection2(const s32 colorSection2) {
    mColorSection2 = colorSection2;
}

s32 Emitter::colorSection3() const {
    return mColorSection3;
}

void Emitter::setColorSection3(const s32 colorSection3) {
    mColorSection3 = colorSection3;
}

s32 Emitter::colorNumRepeat() const {
    return mColorNumRepeat;
}

void Emitter::setColorNumRepeat(const s32 colorNumRepeat) {
    mColorNumRepeat = colorNumRepeat;
}

f32 Emitter::initAlpha() const {
    return mInitAlpha;
}

void Emitter::setInitAlpha(const f32 initAlpha) {
    mInitAlpha = initAlpha;
}


f32 Emitter::diffAlpha21() const {
    return mDiffAlpha21;
}

void Emitter::setDiffAlpha21(const f32 diffAlpha21) {
    mDiffAlpha21 = diffAlpha21;
}

f32 Emitter::diffAlpha32() const {
    return mDiffAlpha32;
}

void Emitter::setDiffAlpha32(const f32 diffAlpha32) {
    mDiffAlpha32 = diffAlpha32;
}

s32 Emitter::alphaSection1() const {
    return mAlphaSection1;
}

void Emitter::setAlphaSection1(const s32 alphaSection1) {
    mAlphaSection1 = alphaSection1;
}

s32 Emitter::alphaSection2() const {
    return mAlphaSection2;
}

void Emitter::setAlphaSection2(const s32 alphaSection2) {
    mAlphaSection2 = alphaSection2;
}

const QVector2D& Emitter::initScale() const {
    return mInitScale;
}

void Emitter::setInitScale(const QVector2D& initScale) {
    mInitScale = initScale;
}

const QVector2D& Emitter::diffScale21() const {
    return mDiffScale21;
}

void Emitter::setDiffScale21(const QVector2D& diffScale21) {
    mDiffScale21 = diffScale21;
}

const QVector2D& Emitter::diffScale32() const {
    return mDiffScale32;
}

void Emitter::setDiffScale32(const QVector2D& diffScale32) {
    mDiffScale32 = diffScale32;
}

s32 Emitter::scaleSection1() const {
    return mScaleSection1;
}

void Emitter::setScaleSection1(const s32 scaleSection1) {
    mScaleSection1 = scaleSection1;
}

s32 Emitter::scaleSection2() const {
    return mScaleSection2;
}

void Emitter::setScaleSection2(const s32 scaleSection2) {
    mScaleSection2 = scaleSection2;
}

f32 Emitter::scaleRand() const {
    return mScaleRand;
}

void Emitter::setScaleRand(const f32 scaleRand) {
    mScaleRand = scaleRand;
}

RotType Emitter::rotType() const {
    return mRotType;
}

void Emitter::setRotType(const RotType rotType) {
    mRotType = rotType;
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

const binVec3i& Emitter::initRot() const {
    return mInitRot;
}

void Emitter::setInitRot(const binVec3i& initRot) {
    mInitRot = initRot;
}

const binVec3i& Emitter::initRotRand() const {
    return mInitRotRand;
}

void Emitter::setInitRotRand(const binVec3i& initRotRand) {
    mInitRotRand = initRotRand;
}

const binVec3i& Emitter::rotVel() const {
    return mRotVel;
}

void Emitter::setRotVel(const binVec3i& rotVel) {
    mRotVel = rotVel;
}

const binVec3i& Emitter::rotVelRand() const {
    return mRotVelRand;
}

void Emitter::setRotVelRand(const binVec3i& rotVelRand) {
    mRotVelRand = rotVelRand;
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
    mVolumeTblIndex = emitterData.volumeTblIndex;
    m_33 = emitterData._33;
    mVolumeType = emitterData.volumeType;
    mVolumeRadius = QVector3D(emitterData.volumeRadius.x, emitterData.volumeRadius.y, emitterData.volumeRadius.z);
    mVolumeSweepStart = emitterData.volumeSweepStart;
    mVolumeSweepParam = emitterData.volumeSweepParam;
    mFigureVel = emitterData.figureVel;
    mEmitterVelDir = QVector3D(emitterData.emitterVelDir.x, emitterData.emitterVelDir.y, emitterData.emitterVelDir.z);
    m_5C = emitterData._5C;
    mInitVelRnd = emitterData.initVelRnd;
    mSpreadVec = QVector3D(emitterData.spreadVec.x, emitterData.spreadVec.y, emitterData.spreadVec.z);
    std::ranges::copy(emitterData._70, m_70.begin());
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

    mColors = emitterData.color;

    std::ranges::copy(emitterData._D8, m_D8.begin());

    mColorSection1 = emitterData.colorSection1;
    mColorSection2 = emitterData.colorSection2;
    mColorSection3 = emitterData.colorSection3;
    mColorNumRepeat = emitterData.colorNumRepeat;
    mInitAlpha = emitterData.initAlpha;
    mDiffAlpha21 = emitterData.diffAlpha21;
    mDiffAlpha32 = emitterData.diffAlpha32;
    mAlphaSection1 = emitterData.alphaSection1;
    mAlphaSection2 = emitterData.alphaSection2;
    mInitScale = QVector2D(emitterData.initScale.x, emitterData.initScale.y);
    mDiffScale21 = QVector2D(emitterData.diffScale21.x, emitterData.diffScale21.y);
    mDiffScale32 = QVector2D(emitterData.diffScale32.x, emitterData.diffScale32.y);
    mScaleSection1 = emitterData.scaleSection1;
    mScaleSection2 = emitterData.scaleSection2;
    mScaleRand = emitterData.scaleRand;
    mRotType = static_cast<RotType>(emitterData.rotCombinerType % 5);
    mCombinerType = static_cast<CombinerType>(emitterData.rotCombinerType / 5);
    mFollowType = emitterData.followType;
    m_134 = emitterData._134;
    mInitRot = emitterData.initRot;
    mInitRotRand = emitterData.initRotRand;
    mRotVel = emitterData.rotVel;
    mRotVelRand = emitterData.rotVelRand;
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
