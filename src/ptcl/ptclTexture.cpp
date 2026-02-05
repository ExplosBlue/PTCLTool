#include "ptcl/ptclTexture.h"
#include "util/imageUtil.h"

#include <memory>
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

TextureFormat Texture::textureFormat() const {
    return mTextureFormat;
}

u32 Texture::userCount() const {
    return mUserCount;
}

u32 Texture::Id() const {
    return mId;
}

bool Texture::isPlaceholder() const {
    return mIsPlaceholder;
}

void Texture::replaceTexture(std::vector<u8>* encodedData, s32 width, s32 height, TextureFormat format) {
    mEncodedData = std::move(*encodedData);
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

const std::shared_ptr<Texture>& Texture::placeholder() {
    static std::shared_ptr<Texture> sPlaceholder = [] {
        constexpr s32 width = 8;
        constexpr s32 height = 8;
        constexpr s32 tile = width / 2;

        QImage img(width, height, QImage::Format_RGBA8888);
        img.fill(Qt::magenta);

        const QColor c0(Qt::magenta);
        const QColor c1(Qt::black);

        for (s32 y = 0; y < height; ++y) {
            for (s32 x = 0; x < width; ++x) {
                bool odd = ((x / tile) + (y / tile)) & 1;
                img.setPixelColor(x, y, odd ? c0 : c1);
            }
        }

        auto encoded = ImageUtil::QImageToPicaTexture(img, TextureFormat::ETC1);
        auto tex = std::make_shared<Texture>(&encoded, width, height, TextureFormat::ETC1);

        tex->mIsPlaceholder = true;
        return tex;
    }();
    return sPlaceholder;
}

// ========================================================================== //


TextureHandle::TextureHandle(std::shared_ptr<Texture> texture) :
    mTexturePtr(std::move(texture)) {
    incrementCount();
}

TextureHandle::~TextureHandle() {
    decrementCount();
}

TextureHandle TextureHandle::clone() const {
    return {mTexturePtr};
}

void TextureHandle::invalidate() {
    mTexturePtr = nullptr;
}

bool TextureHandle::isValid() const {
    return mTexturePtr != nullptr;
}

std::shared_ptr<Texture> TextureHandle::get() const {
    return mTexturePtr ? mTexturePtr : Texture::placeholder();
}

void TextureHandle::set(const std::shared_ptr<Texture>& texture) {
    if (mTexturePtr != texture) {
        decrementCount();
        mTexturePtr = texture;
        incrementCount();
    }
}

TextureHandle& TextureHandle::operator=(const std::shared_ptr<Texture>& texture) {
    set(std::move(texture));
    return *this;
}

std::shared_ptr<Texture> TextureHandle::operator->() const {
    return get();
}

void TextureHandle::incrementCount() {
    if (mTexturePtr && !mTexturePtr->isPlaceholder()) {
        mTexturePtr->mUserCount++;
    }
}

void TextureHandle::decrementCount() {
    if (mTexturePtr && !mTexturePtr->isPlaceholder()) {
        mTexturePtr->mUserCount--;
    }
}


// ========================================================================== //


} // namespace Ptcl
