#ifndef PTCLTEXTURE_H
#define PTCLTEXTURE_H

#include <QImage>

#include "ptcl/ptclEnum.h"

namespace Ptcl {


// ========================================================================== //


class Texture
{
public:
    Texture() = delete;
    Texture(std::vector<u8>* encodedData, s32 width, s32 height, TextureFormat format);

    // Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // TODO: getters/setters/whatever
    const QImage& textureData() const;
    const std::vector<u8>& textureDataRaw() const;

    const TextureFormat textureFormat() const;

    u32 userCount() const;

    u32 Id() const;

private:
    std::vector<u8> mEncodedData;
    TextureFormat mTextureFormat;
    QImage mDecodedTexture;

    // TODO: maybe other stuff here?

    inline static u32 sNextId = 0;
    u32 mId;

protected:
    u32 mUserCount = 0;

    friend class TextureHandle;
};


// ========================================================================== //


class TextureHandle
{
public:
    TextureHandle(std::shared_ptr<Texture> texture = nullptr);

    TextureHandle(const TextureHandle&) = delete;
    TextureHandle& operator=(const TextureHandle&) = delete;

    ~TextureHandle();

    void invalidate();
    bool isValid() const;

    std::shared_ptr<Texture> get() const;
    void set(std::shared_ptr<Texture> texture);

    TextureHandle& operator=(std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> operator->() const;

private:
    void incrementCount();
    void decrementCount();

private:
    std::shared_ptr<Texture> mTexturePtr;
};

} // namespace Ptcl

#endif // PTCLTEXTURE_H
