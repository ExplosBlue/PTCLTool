#include "ptcl/ptclTexture.h"
#include "util/imageUtil.h"

#include <utility>


namespace Ptcl {


// ========================================================================== //


Texture::Texture(std::vector<u8>* encodedData, s32 width, s32 height, TextureFormat format) :
    mEncodedData{std::move(*encodedData)}, mTextureFormat{format}, mId{sNextId++} {
    mDecodedTexture = ImageUtil::picaTextureToQImage(mEncodedData, width, height, format);

    if (mDecodedTexture.isNull()) {
        throw std::runtime_error("Failed to decode texture from encoded data.");
    }
}

const QImage& Texture::textureData() const {
    return mDecodedTexture;
}

const std::vector<u8>& Texture::textureDataRaw() const {
    return mEncodedData;
}

TextureFormat Texture::textureFormat() const
{
    return mTextureFormat;
}

u32 Texture::userCount() const {
    return mUserCount;
}

u32 Texture::Id() const {
    return mId;
}

void Texture::replaceTexture(std::vector<u8>* encodedData, s32 width, s32 height, TextureFormat format) {
    mTextureFormat = format;
    mDecodedTexture = ImageUtil::picaTextureToQImage(mEncodedData, width, height, format);

    if (mDecodedTexture.isNull()) {
        throw std::runtime_error("Failed to decode texture from encoded data.");
    }
}

void Texture::replaceTexture(const Texture& other) {
    mTextureFormat = other.mTextureFormat;
    mEncodedData = other.mEncodedData;
    mDecodedTexture = other.mDecodedTexture;
}

// ========================================================================== //


TextureHandle::TextureHandle(std::shared_ptr<Texture> texture) :
    mTexturePtr(std::move(texture)) {
    incrementCount();
}

TextureHandle::~TextureHandle() {
    decrementCount();
}

void TextureHandle::invalidate() {
    mTexturePtr = nullptr;
}


bool TextureHandle::isValid() const {
    return mTexturePtr != nullptr;
}

std::shared_ptr<Texture> TextureHandle::get() const {
    return mTexturePtr;
}

void TextureHandle::set(const std::shared_ptr<Texture>& texture)
{
    if (mTexturePtr != texture) {
        decrementCount();
        mTexturePtr = texture;
        incrementCount();
    }
}

TextureHandle& TextureHandle::operator=(std::shared_ptr<Texture> texture) {
    set(std::move(texture));
    return *this;
}


std::shared_ptr<Texture> TextureHandle::operator->() const {
    return mTexturePtr;
}

void TextureHandle::incrementCount() {
    if (mTexturePtr) {
        mTexturePtr->mUserCount++;
    }
}

void TextureHandle::decrementCount() {
    if (mTexturePtr) {
        mTexturePtr->mUserCount--;
    }
}


// ========================================================================== //


} // namespace Ptcl
