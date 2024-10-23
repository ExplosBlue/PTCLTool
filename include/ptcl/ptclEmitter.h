#ifndef PTCLEMITTER_H
#define PTCLEMITTER_H

#include <array>

#include <QString>
#include <QVector2D>
#include <QVector3D>
#include <QMatrix3x4>

#include "typedefs.h"
#include "ptclEnum.h"
#include "ptclTexture.h"

// TODO: This is temporary, replace with better solution
#include "ptclBinary.h"


namespace Ptcl {
    // class TextureHandle;
    class Texture;
}


// ========================================================================== //


namespace Ptcl {

class Emitter
{
public:
    Emitter();

    Emitter(const Emitter&) = delete;
    Emitter& operator=(const Emitter&) = delete;

    EmitterType type() const;
    void setType(const EmitterType type);

    u32 flag() const;
    void setFlag(const u32 flag);

    u32 randomSeed() const;
    void setRandomSeed(const u32 randomSeed);

    const QString& name() const;
    void setName(const QString& name);

    const TextureHandle& textureHandle() const;
    void setTexture(std::shared_ptr<Texture> texture);

    TextureWrap textureWrapT() const;
    void setTextureWrapT(TextureWrap wrap);

    TextureWrap textureWrapS() const;
    void setTextureWrapS(TextureWrap wrap);

    TextureMagFilter textureMagFilter() const;
    void setTextureMagFilter(TextureMagFilter filter);

    TextureMinFilter textureMinFilter() const;
    void setTextureMinFilter(TextureMinFilter filter);

    u8 _2C() const;
    void set_2C(const u8 _2C);

    u8 _2D() const;
    void set_2D(const u8 _2D);

    u8 _2E() const;
    void set_2E(const u8 _2E);

    u8 _2F() const;
    void set_2F(const u8 _2F);

    u8 _30() const;
    void set_30(const u8 _30);

    u8 _31() const;
    void set_31(const u8 _31);

    u16 _32() const;
    void set_32(const u16 _32);

    VolumeType volumeType() const;
    void setVolumeType(const VolumeType volumeType);

    const QVector3D& volumeRadius() const;
    void setVolumeRadius(const QVector3D& volumeRadius);

    f32 _44() const;
    void set_44(const f32 _44);

    f32 _48() const;
    void set_48(const f32 _48);

    f32 _4C() const;
    void set_4C(const f32 _4C);

    const std::array<u32, 13>& _50() const;
    void set_50(const std::array<u32, 13>& _50);

    s32 ptclLife() const;
    void setPtclLife(const s32 ptclLife);

    u32 _88() const;
    void set_88(const u32 _88);

    f32 _8C() const;
    void set_8C(const f32 _8C);

    const std::array<u32, 3>& _90() const;
    void set_90(const std::array<u32, 3>& _90);

    f32 _9C() const;
    void set_9C(const f32 _9C);

    f32 _A0() const;
    void set_A0(const f32 _A0);

    f32 _A4() const;
    void set_A4(const f32 _A4);

    const std::array<QColor, 3>& colors() const;
    void setColors(const std::array<QColor, 3>& colors);

    const std::array<u32, 3>& _D8() const;
    void set_D8(const std::array<u32, 3>& _D8);

    s32 colorSection1() const;
    void setColorSection1(const s32 colorSection1);

    s32 colorSection2() const;
    void setColorSection2(const s32 colorSection2);

    s32 colorSection3() const;
    void setColorSection3(const s32 colorSection3);

    s32 colorNumRepeat() const;
    void setColorNumRepeat(const s32 colorNumRepeat);

    f32 initAlpha() const;
    void setInitAlpha(const f32 initAlpha);

    const std::array<u32, 4>& _F8() const;
    void set_F8(const std::array<u32, 4>& _F8);

    const QVector2D& initScale() const;
    void setInitScale(const QVector2D& initScale);

    const std::array<u32, 24>& _110() const;
    void set_110(const std::array<u32, 24>& _110);

    const QMatrix3x4& _170() const;
    void set_170(const QMatrix3x4& _170);

    const QMatrix3x4& _1A0() const;
    void set_1A0(const QMatrix3x4& _1A0);

    const std::array<u32, 9>& _1D0() const;
    void set_1D0(const std::array<u32, 9>& _1D0);

    void initFromBinary(const BinCommonEmitterData& emitterData);

    // TODO: This is temporary, replace with better solution
    const BinComplexEmitterData& complexEmitterData() const;
    void setComplexEmitterData(const BinComplexEmitterData& emitterData);

    const std::vector<char>& complexData() const;
    void setComplexData(const std::vector<char>& complexData);

    const BinEmitterDataType2& emitterDataType2() const;
    void setEmitterDataType2(const BinEmitterDataType2& emitterData);

private:
    EmitterType mType;
    u32 mFlag;
    u32 mRandomSeed;

    QString mName; // namePos, name

    TextureHandle mTextureHande;
    // Texture Wrap etc should go here...
    TextureWrap mTextureWrapT;
    TextureWrap mTextureWrapS;
    TextureMagFilter mTextureMagFilter;
    TextureMinFilter mTextureMinFilter;

    u8 m_2C;
    u8 m_2D;
    u8 m_2E;
    u8 m_2F;
    u8 m_30;
    u8 m_31;
    u16 m_32; // probably padding...
    VolumeType mVolumeType;
    QVector3D mVolumeRadius;
    f32 m_44;
    f32 m_48;
    f32 m_4C;
    std::array<u32, 13> m_50;
    s32 mPtclLife;
    u32 m_88;
    f32 m_8C;
    std::array<u32, 3> m_90;
    f32 m_9C;
    f32 m_A0;
    f32 m_A4;
    std::array<QColor, 3> mColors;
    std::array<u32, 3> m_D8;
    s32 mColorSection1; // Should these be QColor?
    s32 mColorSection2; // Should these be QColor?
    s32 mColorSection3; // Should these be QColor?
    s32 mColorNumRepeat;
    f32 mInitAlpha;
    std::array<u32, 4> m_F8;
    QVector2D mInitScale;
    std::array<u32, 24> m_110;
    QMatrix3x4 m_170; // SRT related
    QMatrix3x4 m_1A0; // RT related
    std::array<u32, 9> m_1D0;

    // TODO: This is temporary, replace with better solution
    BinComplexEmitterData mComplexEmitterData;
    std::vector<char> mComplexData;  // idk what this is

    BinEmitterDataType2 mEmitterDataType2;
};


} // namespace Ptcl

#endif // PTCLEMITTER_H
