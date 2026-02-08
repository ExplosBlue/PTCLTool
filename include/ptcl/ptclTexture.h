#pragma once

#include "ptcl/ptclEnum.h"

#include <QImage>


namespace Ptcl {


// ========================================================================== //


class Texture {
public:
    using UserCountCallback = std::function<void()>;

public:
    Texture() = delete;
    Texture(std::vector<u8>* encodedData, s32 width, s32 height, TextureFormat format);

    Texture& operator=(const Texture&) = delete;

    const QImage& textureData() const;
    const std::vector<u8>& textureDataRaw() const;

    TextureFormat textureFormat() const;

    u32 userCount() const;
    u32 Id() const;

    bool isPlaceholder() const;

    void replaceTexture(std::vector<u8>* encodedData, s32 width, s32 height, TextureFormat format);
    void replaceTexture(const Texture& other);

    static const std::shared_ptr<Texture>& placeholder();

    void setUserCountCallback(UserCountCallback callback);

protected:
    void incrementUserCount();
    void decrementUserCount();

private:
    void doUserCountCallback() const;

private:
    std::vector<u8> mEncodedData{};
    TextureFormat mTextureFormat{};
    QImage mDecodedTexture{};

    inline static u32 sNextId{0};

    u32 mId{};
    bool mIsPlaceholder{false};

    UserCountCallback mUserCountCallBack{};

    u32 mUserCount{0};

    friend class TextureHandle;
};


// ========================================================================== //


class TextureHandle {
public:
    TextureHandle(std::shared_ptr<Texture> texture = nullptr);
    TextureHandle(const TextureHandle& other);
    TextureHandle(TextureHandle&& other) noexcept;

    TextureHandle& operator=(const TextureHandle& other);
    TextureHandle& operator=(TextureHandle&& other) noexcept;

    ~TextureHandle();

    TextureHandle clone() const;

    void invalidate();
    bool isValid() const;

    std::shared_ptr<Texture> get() const;
    void set(const std::shared_ptr<Texture>& texture);

    TextureHandle& operator=(const std::shared_ptr<Texture>& texture);
    std::shared_ptr<Texture> operator->() const;

private:
    void incrementCount();
    void decrementCount();

private:
    std::shared_ptr<Texture> mTexturePtr;
};


// ========================================================================== //


} // namespace Ptcl
