#ifndef PTCLCHILDDATA_H
#define PTCLCHILDDATA_H

#include <array>

#include <QVector3D>

#include "typedefs.h"
#include "ptclEnum.h"
#include "ptclTexture.h"

#include"ptclBinary.h"

namespace Ptcl {

class ChildData
{
public:
    ChildData();

    ChildData(const ChildData&) = delete;
    ChildData& operator=(const ChildData&) = delete;

    const std::array<u8, 44>& _0() const;
    void set_0(const std::array<u8, 44>& _0);

    u32 _2C() const;
    void set_2C(const u32 _2C);

    const std::array<u8, 4>& _30() const;
    void set_30(const std::array<u8, 4>& _30);

    const TextureHandle& textureHandle() const;
    void setTexture(std::shared_ptr<Texture> texture);

    TextureWrap textureWrapT() const;
    void setTextureWrapT(TextureWrap wrap);

    TextureWrap textureWrapS() const;
    void setTextureWrapS(TextureWrap wrap);

    TextureFilter textureMagFilter() const;
    void setTextureMagFilter(TextureFilter filter);

    TextureFilter textureMinFilter() const;
    void setTextureMinFilter(TextureFilter filter);

    TextureMipFilter textureMipFilter() const;
    void setTextureMipFilter(TextureMipFilter filter);

    const std::array<u8, 16>& _4C() const;
    void set_4C(const std::array<u8, 16>& _4C);

    f32 _5C() const;
    void set_5C(const f32 _5C);

    f32 _60() const;
    void set_60(const f32 _60);

    f32 _64() const;
    void set_64(const f32 _64);

    const std::array<u8, 84>& _68() const;
    void set_68(const std::array<u8, 84>& _68);

    const QVector3D& _BC() const;
    void set_BC(const QVector3D& _BC);

    const std::array<u8, 32>& _C8() const;
    void set_C8(const std::array<u8, 32>& _C8);

    f32 _E8() const;
    void set_E8(const f32 _E8);

    void initFromBinary(const BinChildData& childData);

private:
    std::array<u8, 44> m_0;
    u32 m_2C;
    std::array<u8, 4> m_30;

    TextureHandle mTextureHande;
    TextureWrap mTextureWrapT;
    TextureWrap mTextureWrapS;
    TextureFilter mTextureMagFilter;
    TextureFilter mTextureMinFilter;
    TextureMipFilter mTextureMipFilter;

    std::array<u8, 16> m_4C;
    f32 m_5C;
    f32 m_60;
    f32 m_64;
    std::array<u8, 84> m_68;
    QVector3D m_BC;
    std::array<u8, 32> m_C8;
    f32 m_E8;
};


} // namespace Ptcl

#endif // PTCLCHILDDATA_H
