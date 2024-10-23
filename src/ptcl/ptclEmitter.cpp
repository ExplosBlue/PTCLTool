#include "ptcl/ptclEmitter.h"

namespace Ptcl {


Emitter::Emitter() {

}

EmitterType Emitter::type() const {

    return mType;
}

void Emitter::setType(const EmitterType type) {

    mType = type;
}

u32 Emitter::flag() const {

    return mFlag;
}

void Emitter::setFlag(const u32 flag) {

    mFlag = flag;
}

u32 Emitter::randomSeed() const {

    return mRandomSeed;
}

void Emitter::setRandomSeed(const u32 randomSeed) {

    mRandomSeed = randomSeed;
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

TextureMagFilter Emitter::textureMagFilter() const {

    return mTextureMagFilter;
}

void Emitter::setTextureMagFilter(TextureMagFilter filter) {

    mTextureMagFilter = filter;
}

TextureMinFilter Emitter::textureMinFilter() const {

    return mTextureMinFilter;
}


void Emitter::setTextureMinFilter(TextureMinFilter filter) {

    mTextureMinFilter = filter;
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

u16 Emitter::_32() const {

    return m_32;
}

void Emitter::set_32(const u16 _32) {

    m_32 = _32;
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

f32 Emitter::_44() const {

    return m_44;
}

void Emitter::set_44(const f32 _44) {

    m_44 = _44;
}

f32 Emitter::_48() const {

    return m_48;
}

void Emitter::set_48(const f32 _48) {

    m_48 = _48;
}

f32 Emitter::_4C() const {

    return m_4C;
}

void Emitter::set_4C(const f32 _4C) {

    m_4C = _4C;
}

const std::array<u32, 13>& Emitter::_50() const {

    return m_50;
}

void Emitter::set_50(const std::array<u32, 13>& _50) {

    m_50 = _50;
}

s32 Emitter::ptclLife() const {

    return mPtclLife;
}

void Emitter::setPtclLife(const s32 ptclLife) {

    mPtclLife = ptclLife;
}

u32 Emitter::_88() const {

    return m_88;
}

void Emitter::set_88(const u32 _88) {

    m_88 = _88;
}

f32 Emitter::_8C() const {

    return m_8C;
}

void Emitter::set_8C(const f32 _8C) {

    m_8C = _8C;
}

const std::array<u32, 3>& Emitter::_90() const {

    return m_90;
}

void Emitter::set_90(const std::array<u32, 3>& _90) {

    m_90 = _90;
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

const std::array<QColor, 3>& Emitter::colors() const {

    return mColors;
}

void Emitter::setColors(const std::array<QColor, 3>& colors) {

    mColors = colors;
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

const std::array<u32, 4>& Emitter::_F8() const {

    return m_F8;
}

void Emitter::set_F8(const std::array<u32, 4>& _F8) {

    m_F8 = _F8;
}

const QVector2D& Emitter::initScale() const {

    return mInitScale;
}

void Emitter::setInitScale(const QVector2D& initScale) {

    mInitScale = initScale;
}

const std::array<u32, 24>& Emitter::_110() const {

    return m_110;
}

void Emitter::set_110(const std::array<u32, 24>& _110) {

    m_110 = _110;
}

const QMatrix3x4& Emitter::_170() const {

    return m_170;
}

void Emitter::set_170(const QMatrix3x4& _170) {

    m_170 = _170;
}

const QMatrix3x4& Emitter::_1A0() const {

    return m_1A0;
}
void Emitter::set_1A0(const QMatrix3x4& _1A0) {

    m_1A0 = _1A0;
}

const std::array<u32, 9>& Emitter::_1D0() const {

    return m_1D0;
}

void Emitter::set_1D0(const std::array<u32, 9>& _1D0) {

    m_1D0 = _1D0;
}

void Emitter::initFromBinary(const BinCommonEmitterData& emitterData) {

    mType = emitterData.type;
    mFlag = emitterData.flag;
    mRandomSeed = emitterData.randomSeed;
    // mName
    // mTexture
    // mTextureFormat
    m_2C = emitterData._2C;
    m_2D = emitterData._2D;
    m_2E = emitterData._2E;
    m_2F = emitterData._2F;
    m_30 = emitterData._30;
    m_31 = emitterData._31;
    m_32 = emitterData._32;
    mVolumeType = emitterData.volumeType;
    mVolumeRadius = QVector3D(emitterData.volumeRadius.x, emitterData.volumeRadius.y, emitterData.volumeRadius.z);
    m_44 = emitterData._44;
    m_48 = emitterData._48;
    m_4C = emitterData._4C;
    std::ranges::copy(emitterData._50, m_50.begin());
    mPtclLife = emitterData.ptclLife;
    m_88 = emitterData._88;
    m_8C = emitterData._8C;
    std::ranges::copy(emitterData._90, m_90.begin());
    m_9C = emitterData._9C;
    m_A0 = emitterData._A0;
    m_A4 = emitterData._A4;

    for (size_t idx = 0; idx < mColors.size(); ++idx) {
        auto& color = mColors[idx];
        auto& binColor = emitterData.color[idx];

        // qDebug() << "BinColor {" << binColor.r << "," << binColor.r << "," << binColor.b << "," << binColor.a << "}";

        color.fromRgbF(binColor.r, binColor.g, binColor.b, binColor.a);
    }

    std::ranges::copy(emitterData._D8, m_D8.begin());

    mColorSection1 = emitterData.colorSection1;
    mColorSection2 = emitterData.colorSection2;
    mColorSection3 = emitterData.colorSection3;
    mColorNumRepeat = emitterData.colorNumRepeat;
    mInitAlpha = emitterData.initAlpha;

    std::ranges::copy(emitterData._F8, m_F8.begin());

    mInitScale = QVector2D(emitterData.initScale.x, emitterData.initScale.y);

    std::ranges::copy(emitterData._110, m_110.begin());

    m_170 = QMatrix3x4(emitterData._170.cells.data());
    m_1A0 = QMatrix3x4(emitterData._1A0.cells.data());

    std::ranges::copy(emitterData._1D0, m_1D0.begin());
}

// TODO: This is temporary, replace with better solution
void Emitter::setComplexEmitterData(const BinComplexEmitterData& emitterData) {

    mComplexEmitterData = emitterData;
}

const BinComplexEmitterData& Emitter::complexEmitterData() const {

    return mComplexEmitterData;
}

const std::vector<char>& Emitter::complexData() const {

    return mComplexData;
}

void Emitter::setComplexData(const std::vector<char>& complexData) {

    mComplexData = complexData;
}

void Emitter::setEmitterDataType2(const BinEmitterDataType2& emitterData) {
    mEmitterDataType2 = emitterData;
}

const BinEmitterDataType2& Emitter::emitterDataType2() const {

    return mEmitterDataType2;
}

} // namespace Ptcl
