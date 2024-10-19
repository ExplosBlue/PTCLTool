#ifndef PTCLRES_H
#define PTCLRES_H

#include <QVector2D>
#include <QVector3D>
#include <QDataStream>
#include <QColor>
#include <QMatrix3x4>
#include <QDebug>

#include "typedefs.h"
#include "ptclEnum.h"

namespace Ptcl {
    class Emitter;
}


// ========================================================================== //


namespace Ptcl {

#pragma pack(push, 4)

// Size 0x08
struct alignas(4) binVec2f {

    f32 x; //   0x00
    f32 y; //   0x04

    binVec2f();
    binVec2f(const QVector2D& vec);

    friend QDataStream& operator>>(QDataStream& in, binVec2f& item);
    friend QDataStream& operator<<(QDataStream& out, const binVec2f& item);

    friend QDebug operator<<(QDebug dbg, const binVec2f& item);
};

static_assert(sizeof(binVec2f) == 0x08, "binVec2f is incorrect size.");


// ========================================================================== //


// Size 0x0C
struct alignas(4) binVec3f {

    f32 x;   // 0x00
    f32 y;   // 0x04
    f32 z;   // 0x08

    binVec3f();
    binVec3f(const QVector3D& vec);

    friend QDataStream& operator>>(QDataStream& in, binVec3f& item);
    friend QDataStream& operator<<(QDataStream& out, const binVec3f& item);

    friend QDebug operator<<(QDebug dbg, const binVec3f& item);
};

static_assert(sizeof(binVec3f) == 0x0C, "binVec3f is incorrect size.");


// ========================================================================== //


// Size 0x30
union alignas(4) binMtx34f {

    std::array<std::array<f32, 3>, 4> rows;   // 0x00 - 0x30
    std::array<f32, 12> cells;                // 0x00 - 0x30

    binMtx34f();
    binMtx34f(const QMatrix3x4& mtx);

    friend QDataStream& operator>>(QDataStream& in, binMtx34f& item);
    friend QDataStream& operator<<(QDataStream& out, const binMtx34f& item);

    friend QDebug operator<<(QDebug dbg, const binMtx34f& item);
};

static_assert(sizeof(binMtx34f) == 0x30, "binMtx34f is incorrect size.");


// ========================================================================== //


// Size 0x10
struct alignas(4) binColor4f {

    f32 r;   // 0x00
    f32 g;   // 0x04
    f32 b;   // 0x08
    f32 a;   // 0x0C

    binColor4f();
    binColor4f(const QColor& color);

    friend QDataStream& operator>>(QDataStream& in, binColor4f& item);
    friend QDataStream& operator<<(QDataStream& out, const binColor4f& item);

    friend QDebug operator<<(QDebug dbg, const binColor4f& item);
};

static_assert(sizeof(binColor4f) == 0x10, "binColor4f is incorrect size.");


// ========================================================================== //


// Size 0x1C
struct alignas(4) BinHeaderData {

    std::array<char, 4> magic;   // 0x00
    u32 version;                 // 0x04
    s32 numEmitterSet;           // 0x08
    s32 namePos;                 // 0x0C
    s32 nameTblPos;              // 0x10
    s32 textureTblPos;           // 0x14
    s32 textureTblSize;          // 0x18

    BinHeaderData();

    friend QDataStream& operator>>(QDataStream& in, BinHeaderData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinHeaderData& item);

    void printData(u32 indentationLevel = 0);
};

static_assert(sizeof(BinHeaderData) == 0x1C, "BinHeaderData is incorrect size.");


// ========================================================================== //


// Size 0x1C
struct alignas(4) BinEmitterSetData
{
    u32 userData;         // 0x00
    u32 lastUpdateDate;   // 0x04
    s32 namePos;          // 0x08
    uptr32 namePtr;       // 0x0C - a placeholder for a 32 bit const char* (should always be 0)
    u32 numEmitter;       // 0x10
    u32 emitterTblPos;    // 0x14
    u32 emitterTbl;       // 0x18

    BinEmitterSetData();

    friend QDataStream &operator>>(QDataStream& in, BinEmitterSetData& item);
    friend QDataStream &operator<<(QDataStream& out, const BinEmitterSetData& item);

    void printData(u32 indentationLevel = 0);
};

static_assert(sizeof(BinEmitterSetData) == 0x1C, "BinEmitterSetData is incorrect size.");


// ========================================================================== //


// Size 0x0C
struct alignas(4) BinTextureRes
{
    u16 width;                            // 0x00
    u16 height;                           // 0x02
    TextureFormat format;                 // 0x04
    TextureWrap wrapT;                    // 0x05
    TextureWrap wrapS;                    // 0x06
    TextureMagFilter magFilter;           // 0x07
    TextureMinFilter minFilter;           // 0x08

    BinTextureRes();

    friend QDataStream& operator>>(QDataStream& in, BinTextureRes& item);
    friend QDataStream& operator<<(QDataStream& out, const BinTextureRes& item);

    void printData(u32 indentationLevel = 0);
};

static_assert(sizeof(BinTextureRes) == 0x0C, "BinTextureRes is incorrect size.");


// ========================================================================== //


// Size 0x1F4
struct alignas(4) BinCommonEmitterData
{
    EmitterType type;                    // 0x00
    u32 flag;                            // 0x04
    u32 randomSeed;                      // 0x08
    s32 namePos;                         // 0x0C
    uptr32 namePtr;                      // 0x10 - placeholder for a 32 bit const char* (should always be 0)
    BinTextureRes textureRes;            // 0x14
    u32 textureSize;                     // 0x20
    u32 texturePos;                      // 0x24
    uptr32 textureHandlePtr;             // 0x28 - placeholder for a 32 bit textureHandle* (should always be 0)
    u8 _2C;                              // 0x2C
    u8 _2D;                              // 0x2D
    u8 _2E;                              // 0x2E
    u8 _2F;                              // 0x2F
    u8 _30;                              // 0x30
    u8 _31;                              // 0x31
    u16 _32;                             // 0x32 - this is probably padding
    VolumeType volumeType;               // 0x34
    binVec3f volumeRadius;               // 0x38
    f32 _44;                             // 0x44 - volumeSweepParam? rotWidth?
    f32 _48;                             // 0x48 - volumeSweepStart? rotStart?
    f32 _4C;                             // 0x4C - maybe emitAnimValue? (is f32* in nw4c ptcl) related to figureVelocity
    u32 _50[13];                         // 0x50
    s32 ptclLife;                        // 0x84
    u32 _88;                             // 0x88
    f32 _8C;                             // 0x8C
    u32 _90[3];                          // 0x90
    f32 _9C;                             // 0x9C - this is probably vec3
    f32 _A0;                             // 0xA0 - ^
    f32 _A4;                             // 0xA4 - ^
    binColor4f color[3];                 // 0xA8
    u32 _D8[3];                          // 0xD8
    s32 colorSection1;                   // 0xE4
    s32 colorSection2;                   // 0xE8
    s32 colorSection3;                   // 0xEC
    s32 colorNumRepeat;                  // 0xF0
    f32 initAlpha;                       // 0xF4
    u32 _F8[4];                          // 0xF8
    binVec2f initScale;                  // 0x108
    u32 _110[24];                        // 0x110
    binMtx34f _170;                      // 0x170 - SRT related
    binMtx34f _1A0;                      // 0x1A0 - RT related
    u32 _1D0[9];                         // 0x1D0

    BinCommonEmitterData();
    BinCommonEmitterData(const Ptcl::Emitter& emitter);

    friend QDataStream& operator>>(QDataStream& in, BinCommonEmitterData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinCommonEmitterData& item);

    void printData(u32 indentationLevel = 0);
};

static_assert(sizeof(BinCommonEmitterData) == 0x1F4, "BinCommonEmitterData is incorrect size.");


// ========================================================================== //


// Size 0x24
struct alignas(4) BinComplexEmitterData
{
    u32 _0;    // 0x00
    u32 _4;    // 0x04
    u32 _8;    // 0x08
    u32 _C;    // 0x0C
    u32 _10;   // 0x10
    u32 _14;   // 0x14
    u32 _18;   // 0x18
    u32 _1C;   // 0x1C
    u32 _20;   // 0x20

    BinComplexEmitterData();

    friend QDataStream& operator>>(QDataStream& in, BinComplexEmitterData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinComplexEmitterData& item);

    void printData(u32 indentationLevel = 0);
};

static_assert(sizeof(BinComplexEmitterData) == 0x24, "BinComplexEmitterData is incorrect size.");


// ========================================================================== //


// Size 0x14
struct alignas(4) BinEmitterDataType2
{
    u32 _0;    // 0x00
    u32 _4;    // 0x04
    u32 _8;    // 0x08
    u32 _C;    // 0x0C
    u32 _10;   // 0x10

    BinEmitterDataType2();

    friend QDataStream& operator>>(QDataStream& in, BinEmitterDataType2& item);
    friend QDataStream& operator<<(QDataStream& out, const BinEmitterDataType2& item);

    void printData(u32 indentationLevel = 0);
};

static_assert(sizeof(BinEmitterDataType2) == 0x14, "BinEmitterDataType2 is incorrect size.");


// ========================================================================== //


// Size 0x08
struct alignas(4) BinEmitterTblData {

    s32 emitterPos;      // 0x00
    uptr32 emitterPtr;   // 0x04 - placeholder for a 32 bit Emitter* (should always be 0)

    BinEmitterTblData();

    friend QDataStream& operator>>(QDataStream& in, BinEmitterTblData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinEmitterTblData& item);

    void printData(u32 indentationLevel = 0);
};

static_assert(sizeof(BinEmitterTblData) == 0x08, "BinEmitterTblData is incorrect size.");

#pragma pack(pop)

}

#endif //PTCLRES_H
