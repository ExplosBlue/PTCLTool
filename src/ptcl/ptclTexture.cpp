#include "ptcl/ptclTexture.h"

#include <utility>

#include "util/imageUtil.h"

namespace Ptcl {


// ========================================================================== //


Texture::Texture(std::vector<u8>* encodedData, s32 width, s32 height, TextureFormat format) :
    mEncodedData{std::move(*encodedData)}, mTextureFormat{format}, mId{sNextId++} {

    // Debugging the size and contents of mEncodedData
    // qDebug() << "mEncodedData size:" << mEncodedData.size();
    // if (!mEncodedData.empty()) {
    //     qDebug() << "First few bytes:" << QByteArray(reinterpret_cast<const char*>(mEncodedData.data()),
    //                                                  std::min(static_cast<size_t>(10), mEncodedData.size())).toHex();
    // }

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

const TextureFormat Texture::textureFormat() const {

    return mTextureFormat;
}

u32 Texture::userCount() const {

    return mUserCount;
}


u32 Texture::Id() const {

    return mId;
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


void TextureHandle::set(std::shared_ptr<Texture> texture) {

    if (mTexturePtr != texture) {
        decrementCount();
        mTexturePtr = texture;
        incrementCount();
    }
}

TextureHandle& TextureHandle::operator=(std::shared_ptr<Texture> texture) {

    set(texture);
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


} // namespace Ptcl
