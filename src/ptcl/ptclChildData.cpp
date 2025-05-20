#include "ptcl/ptclChildData.h"

namespace Ptcl {

ChildData::ChildData() {

}

const std::array<u8, 44>& ChildData::_0() const {

    return m_0;
}

void ChildData::set_0(const std::array<u8, 44>& _0) {

    m_0 = _0;
}

u32 ChildData::_2C() const {

    return m_2C;
}

void ChildData::set_2C(const u32 _2C) {

    m_2C = _2C;
}

const std::array<u8, 4>& ChildData::_30() const {

    return m_30;
}

void ChildData::set_30(const std::array<u8, 4>& _30) {

    m_30 = _30;
}

const TextureHandle& ChildData::textureHandle() const {

    return mTextureHande;
}

void ChildData::setTexture(std::shared_ptr<Texture> texture) {

    mTextureHande.set(texture);
}

TextureWrap ChildData::textureWrapT() const {

    return mTextureWrapT;
}

void ChildData::setTextureWrapT(TextureWrap wrap) {

    mTextureWrapT = wrap;
}

TextureWrap ChildData::textureWrapS() const {

    return mTextureWrapS;
}

void ChildData::setTextureWrapS(TextureWrap wrap) {

    mTextureWrapS = wrap;
}

TextureMagFilter ChildData::textureMagFilter() const {

    return mTextureMagFilter;
}

void ChildData::setTextureMagFilter(TextureMagFilter filter) {

    mTextureMagFilter = filter;
}

TextureMinFilter ChildData::textureMinFilter() const {

    return mTextureMinFilter;
}


void ChildData::setTextureMinFilter(TextureMinFilter filter) {

    mTextureMinFilter = filter;
}

const std::array<u8, 16>& ChildData::_4C() const {

    return m_4C;
}

void ChildData::set_4C(const std::array<u8, 16>& _4C) {

    m_4C = _4C;
}

f32 ChildData::_5C() const {

    return m_5C;
}

void ChildData::set_5C(const f32 _5C) {

    m_5C = _5C;
}

f32 ChildData::_60() const {

    return m_60;
}

void ChildData::set_60(const f32 _60) {

    m_60 = _60;
}

f32 ChildData::_64() const {

    return m_64;
}

void ChildData::set_64(const f32 _64) {

    m_64 = _64;
}

const std::array<u8, 84>& ChildData::_68() const {

    return m_68;
}

void ChildData::set_68(const std::array<u8, 84>& _68) {

    m_68 = _68;
}

const QVector3D& ChildData::_BC() const {

    return m_BC;
}

void ChildData::set_BC(const QVector3D& _BC) {

    m_BC = _BC;
}

const std::array<u8, 32>& ChildData::_C8() const {

    return m_C8;
}

void ChildData::set_C8(const std::array<u8, 32>& _C8) {

    m_C8 = _C8;
}

f32 ChildData::_E8() const {

    return m_E8;
}

void ChildData::set_E8(const f32 _E8) {

    m_E8 = _E8;
}

void ChildData::initFromBinary(const BinChildData& childData) {
    m_0 = childData._0;
    m_2C = childData._2C;
    m_30 = childData._30;
    // mTexture
    // mTextureFormat
    mTextureWrapT = childData.textureRes.wrapT;
    mTextureWrapS = childData.textureRes.wrapS;
    mTextureMagFilter = childData.textureRes.magFilter;
    mTextureMinFilter = childData.textureRes.minFilter;

    m_4C = childData._4C;
    m_5C = childData._5C;
    m_60 = childData._60;
    m_64 = childData._64;
    m_68 = childData._68;
    m_BC = QVector3D(childData._BC.x, childData._BC.y, childData._BC.z);
    m_C8 = childData._C8;
    m_E8 = childData._E8;
}

} // namespace ptcl
