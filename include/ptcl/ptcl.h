#ifndef PTCL_H
#define PTCL_H

#include <vector>

#include <QString>
#include <QImage>
#include <QVector2D>
#include <QVector3D>
#include <QMatrix3x4>

#include "typedefs.h"

#include "ptclEnums.h"

#include "gr.h"

namespace Ptcl {

class BinEmitterSet;
class BinCommonEmitterData;

class Emitter
{
public:
    Emitter() = default;

    const EmitterType type() const { return mType; }
    void setType(const EmitterType type) { mType = type; }

    const u32 flag() const { return mFlag; }
    void setFlag(const u32 flag) { mFlag = flag; }

    const u32 randomSeed() const { return mRandomSeed; }
    void setRandomSeed(const u32 randomSeed) { mRandomSeed = randomSeed; }

    const QString& name() const { return mName; }
    void setName(const QString& name) { mName = name; }

    const QImage& texture() const { return mTexture; }
    void setTexture(const QImage& texture) { mTexture = texture; }

    const gr::PicaDataTextureFormat textureFormat() const { return mTextureFormat; }
    void setTextureFormat(const gr::PicaDataTextureFormat textureFormat) { mTextureFormat = textureFormat; }

    const u8 _2C() const { return m_2C; }
    void set_2C(const u8 _2C) { m_2C = _2C; }

    const u8 _2D() const { return m_2D; }
    void set_2D(const u8 _2D) { m_2D = _2D; }

    const u8 _2E() const { return m_2E; }
    void set_2E(const u8 _2E) { m_2E = _2E; }

    const u8 _2F() const { return m_2F; }
    void set_2F(const u8 _2F) { m_2F = _2F; }

    const u8 _30() const { return m_30; }
    void set_30(const u8 _30) { m_30 = _30; }

    const u8 _31() const { return m_31; }
    void set_31(const u8 _31) { m_31 = _31; }

    const u16 _32() const { return m_32; }
    void set_32(const u16 _32) { m_32 = _32; }

    const VolumeType volumeType() const { return mVolumeType; }
    void setVolumeType(const VolumeType volumeType) { mVolumeType = volumeType; }

    const QVector3D& volumeRadius() const { return mVolumeRadius; }
    void setVolumeRadius(const QVector3D& volumeRadius) { mVolumeRadius = volumeRadius; }

    const f32 _44() const { return m_44; }
    void set_44(const f32 _44) { m_44 = _44; }

    const f32 _48() const { return m_48; }
    void set_48(const f32 _48) { m_48 = _48; }

    const f32 _4C() const { return m_4C; }
    void set_4C(const f32 _4C) { m_4C = _4C; }

    const std::array<u32, 13>& _50() const { return m_50; }
    void set_50(const std::array<u32, 13>& _50) { m_50 = _50; }

    const s32 ptclLife() const { return mPtclLife; }
    void setPtclLife(const s32 ptclLife) { mPtclLife = ptclLife; }

    const u32 _88() const { return m_88; }
    void set_88(const u32 _88) { m_88 = _88; }

    const f32 _8C() const { return m_8C; }
    void set_8C(const f32 _8C) { m_8C = _8C; }

    const std::array<u32, 3>& _90() const { return m_90; }
    void set_90(const std::array<u32, 3>& _90) { m_90 = _90; }

    const f32 _9C() const { return m_9C; }
    void set_9C(const f32 _9C) { m_9C = _9C; }

    const f32 _A0() const { return m_A0; }
    void set_A0(const f32 _A0) { m_A0 = _A0; }

    const f32 _A4() const { return m_A4; }
    void set_A4(const f32 _A4) { m_A4 = _A4; }

    const std::array<QColor, 3>& colors() const { return mColors; }
    void setColors(const std::array<QColor, 3>& colors) { mColors = colors; }

    const std::array<u32, 3>& _D8() const { return m_D8; }
    void set_D8(const std::array<u32, 3>& _D8) { m_D8 = _D8; }

    const s32 colorSection1() const { return mColorSection1; }
    void setColorSection1(const s32 colorSection1) { mColorSection1 = colorSection1; }

    const s32 colorSection2() const { return mColorSection2; }
    void setColorSection2(const s32 colorSection2) { mColorSection2 = colorSection2; }

    const s32 colorSection3() const { return mColorSection3; }
    void setColorSection3(const s32 colorSection3) { mColorSection3 = colorSection3; }

    const s32 colorNumRepeat() const { return mColorNumRepeat; }
    void setColorNumRepeat(const s32 colorNumRepeat) { mColorNumRepeat = colorNumRepeat; }

    const f32 initAlpha() const { return mInitAlpha; }
    void setInitAlpha(const f32 initAlpha) { mInitAlpha = initAlpha; }

    const std::array<u32, 4>& _F8() const { return m_F8; }
    void set_F8(const std::array<u32, 4>& _F8) { m_F8 = _F8; }

    const QVector2D& initScale() const { return mInitScale; }
    void setInitScale(const QVector2D& initScale) { mInitScale = initScale; }

    const std::array<u32, 24>& _110() const { return m_110; }
    void set_110(const std::array<u32, 24>& _110) { m_110 = _110; }

    const QMatrix3x4& _170() const { return m_170; }
    void set_170(const QMatrix3x4& _170) { m_170 = _170; }

    const QMatrix3x4& _1A0() const { return m_1A0; }
    void set_1A0(const QMatrix3x4& _1A0) { m_1A0 = _1A0; }

    const std::array<u32, 9>& _1D0() const { return m_1D0; }
    void set_1D0(const std::array<u32, 9>& _1D0) { m_1D0 = _1D0; }

    void setFromBinEmitter(const BinCommonEmitterData& emitterData);

private:
    EmitterType mType;
    u32 mFlag;
    u32 mRandomSeed;

    QString mName; // namePos, name
    QImage mTexture; // - textureRes, texturePos, textureSize, textureHandle

    gr::PicaDataTextureFormat mTextureFormat;

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
};


class EmitterSet
{
public:
    EmitterSet() = default;
    EmitterSet(const BinEmitterSet& binEmitterSet);

    std::vector<Emitter>& emitters() { return mEmitters; }

    const QString& name() const { return mName; }
    void setName(const QString& name) { mName = name; }

    const u32 emitterCount() const { return mEmitters.size(); }

    const u32 userData() const { return mUserData; }
    void setUserData(u32 data) { mUserData = data; }

    const bool isShowDetail() const { return mIsShowDetail; }
    void setisShowDetail(bool isShowDetail) { mIsShowDetail = isShowDetail; }

private:
    QString mName;
    std::vector<Emitter> mEmitters;

    u32 mUserData;
    bool mIsShowDetail; // idk what this is
};


class PtclRes
{
public:
    PtclRes() = default;

    void load(const QString& filePath);
    // void save(const QString& filePath);

    const QString& name() const { return mName; }
    void setName(const QString& name) { mName = name; }

    std::vector<EmitterSet>& getEmitterSets() { return mEmitterSets; }

    // void printInfo();

private:
    QString mName;
    std::vector<EmitterSet> mEmitterSets;
};

}

#endif // PTCL_H
