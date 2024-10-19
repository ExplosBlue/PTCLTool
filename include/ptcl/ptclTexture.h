#ifndef PTCLTEXTURE_H
#define PTCLTEXTURE_H

#include <QImage>
#include <utility>

#include "gr.h"

namespace Ptcl {

class Texture
{
public:
    Texture() = delete;

    // TODO: Rethink this maybe...
    Texture(const QImage& image, gr::PicaDataTextureFormat format) :
        mTextureFormat{format} {
        mTextureData = image;
    }

    // TODO: getters/setters/whatever
    const QImage& textureData() const { return mTextureData; }

    const gr::PicaDataTextureFormat textureFormat() const { return mTextureFormat; }
    // void setTextureFormat(gr::PicaDataTextureFormat format) { mTextureFormat = format; }

    const u32 userCount() const { return mUserCount; }

private:
    QImage mTextureData;
    gr::PicaDataTextureFormat mTextureFormat;

    // TODO: maybe other stuff here?

protected:
    u32 mUserCount = 0;

    friend class TextureHandle;
};


class TextureHandle
{
public:
    TextureHandle(std::shared_ptr<Texture> texture = nullptr) :
        mTexturePtr(std::move(texture)) { incrementCount(); }

    ~TextureHandle() { decrementCount(); }

    void invalidate() { mTexturePtr = nullptr; }
    bool isValid() const { return mTexturePtr != nullptr; }

    std::shared_ptr<Texture> get() const { return mTexturePtr; }
    void set(std::shared_ptr<Texture> texture) {

        qDebug() << "setting texture handle";

        if (mTexturePtr != texture) {
            decrementCount();
            mTexturePtr = texture;
            incrementCount();
        }
    }

    TextureHandle& operator=(std::shared_ptr<Texture> texture) { set(texture); return *this; }
    std::shared_ptr<Texture> operator->() const { return mTexturePtr; }

private:
    void incrementCount() { qDebug() << "Incrementing count"; if (mTexturePtr) mTexturePtr->mUserCount++; }
    void decrementCount() { qDebug() << "Decrementing count"; if (mTexturePtr) mTexturePtr->mUserCount--; }

private:
    std::shared_ptr<Texture> mTexturePtr;
};

} // namespace Ptcl

#endif // PTCLTEXTURE_H
