#pragma once

#include "ptcl/ptclEnum.h"
#include "typedefs.h"
#include "util/bitflagUtil.h"

#include <QColor>
#include <QDataStream>
#include <QDebug>
#include <QMatrix3x4>
#include <QVector2D>
#include <QVector3D>


namespace Ptcl {


class Emitter;
class ChildData;
class FieldRandomData;
class FieldMagnetData;
class FieldSpinData;
class FieldCollisionData;
class FieldConvergenceData;
class FieldPosAddData;
class FluctuationData;
class StripeData;


// ========================================================================== //


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
    std::array<std::array<f32, 4>, 3> rows;   // 0x00 - 0x30
    std::array<f32, 12> cells;                // 0x00 - 0x30

    binMtx34f();
    binMtx34f(const QMatrix3x4& mtx);

    QMatrix3x4 toQMatrix3x4() const;

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
    binColor4f(f32 r, f32 g, f32 b, f32 a);
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
    u32 numEmitterSet;           // 0x08
    u32 namePos;                 // 0x0C
    u32 nameTblPos;              // 0x10
    u32 textureTblPos;           // 0x14
    u32 textureTblSize;          // 0x18

    friend QDataStream& operator>>(QDataStream& in, BinHeaderData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinHeaderData& item);

    void printData(u32 indentationLevel = 0);
};

static_assert(sizeof(BinHeaderData) == 0x1C, "BinHeaderData is incorrect size.");


// ========================================================================== //


// Size 0x1C
struct alignas(4) BinEmitterSetData {
    u32 userData;         // 0x00
    u32 lastUpdateDate;   // 0x04
    u32 namePos;          // 0x08
    uptr32 namePtr;       // 0x0C - a placeholder for a 32 bit const char* (should always be 0)
    u32 numEmitter;       // 0x10
    u32 emitterTblPos;    // 0x14
    u32 emitterTbl;       // 0x18

    friend QDataStream &operator>>(QDataStream& in, BinEmitterSetData& item);
    friend QDataStream &operator<<(QDataStream& out, const BinEmitterSetData& item);

    void printData(u32 indentationLevel = 0);
};

static_assert(sizeof(BinEmitterSetData) == 0x1C, "BinEmitterSetData is incorrect size.");


// ========================================================================== //


// Size 0x0C
struct alignas(4) BinTextureRes {
    u16 width;                            // 0x00
    u16 height;                           // 0x02
    TextureFormat format;                 // 0x04
    TextureWrap wrapT;                    // 0x05
    TextureWrap wrapS;                    // 0x06
    TextureFilter magFilter;              // 0x07
    u8 minMipFilter;                      // 0x08

    friend QDataStream& operator>>(QDataStream& in, BinTextureRes& item);
    friend QDataStream& operator<<(QDataStream& out, const BinTextureRes& item);

    void printData(u32 indentationLevel = 0);
};

static_assert(sizeof(BinTextureRes) == 0x0C, "BinTextureRes is incorrect size.");


// ========================================================================== //


// Size 0x1F4
struct alignas(4) BinCommonEmitterData {
    EmitterType type;                    // 0x00
    BitFlag<EmitterFlag> flag;           // 0x04
    u32 randomSeed;                      // 0x08
    u32 namePos;                         // 0x0C
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
    u8 volumeTblIndex;                   // 0x32
    u8 _33;                              // 0x33
    VolumeType volumeType;               // 0x34
    binVec3f volumeRadius;               // 0x38
    f32 _44;                             // 0x44 - volumeSweepParam? rotWidth?
    f32 _48;                             // 0x48 - volumeSweepStart? rotStart?
    f32 figureVel;                       // 0x4C
    binVec3f emitterVelDir;              // 0x50
    f32 _5C;                             // 0x5C
    f32 initVelRnd;                      // 0x60
    binVec3f spreadVec;                  // 0x64
    std::array<u32, 4> _70;              // 0x70
    u32 _80;                             // 0x80
    s32 ptclLife;                        // 0x84
    s32 ptclLifeRnd;                     // 0x88
    f32 _8C;                             // 0x8C
    u32 _90;                             // 0x90
    BillboardType billboardType;         // 0x94
    u32 _98;                             // 0x98
    f32 _9C;                             // 0x9C - this is probably vec3
    f32 _A0;                             // 0xA0 - ^
    f32 _A4;                             // 0xA4 - ^
    std::array<binColor4f, 3> color;     // 0xA8
    std::array<u32, 3> _D8;              // 0xD8
    s32 colorSection1;                   // 0xE4
    s32 colorSection2;                   // 0xE8
    s32 colorSection3;                   // 0xEC
    s32 colorNumRepeat;                  // 0xF0
    f32 initAlpha;                       // 0xF4
    f32 diffAlpha21;                     // 0xF8
    f32 diffAlpha32;                     // 0xFC
    s32 alphaSection1;                   // 0x100
    s32 alphaSection2;                   // 0x104
    binVec2f initScale;                  // 0x108
    binVec2f diffScale21;                // 0x110
    binVec2f diffScale32;                // 0x118
    s32 scaleSection1;                   // 0x120
    s32 scaleSection2;                   // 0x124
    f32 scaleRand;                       // 0x128
    u32 _12C;                            // 0x12C - indexes rot calc function table @ 0x00602C90
    FollowType followType;               // 0x130
    u32 _134;                            // 0x134
    binVec3f initRot;                    // 0x138 - these may be wrong
    binVec3f initRotRand;                // 0x144 - ^
    binVec3f rotVel;                     // 0x150 - ^
    binVec3f rotVelRand;                 // 0x15C - ^
    std::array<u32, 2> _168;             // 0x168
    binMtx34f transformSRT;              // 0x170
    binMtx34f transformRT;               // 0x1A0
    f32 alphaAddInFade;                  // 0x1D0
    u16 numTexPat;                       // 0x1D4
    u8 numTexDivX;                       // 0x1D6
    u8 numTexDivY;                       // 0x1D7
    binVec2f texUVScale;                 // 0x1D8
    std::array<u8, 16> texPatTbl;        // 0x1E0
    u16 texPatFreq;                      // 0x1F0
    u16 texPatTblUse;                    // 0x1F2

    BinCommonEmitterData() = default;
    BinCommonEmitterData(const Ptcl::Emitter& emitter);

    friend QDataStream& operator>>(QDataStream& in, BinCommonEmitterData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinCommonEmitterData& item);

    void printData(u32 indentationLevel = 0);
};

static_assert(sizeof(BinCommonEmitterData) == 0x1F4, "BinCommonEmitterData is incorrect size.");


// ========================================================================== //


// Size 0x208
struct alignas(4) BinComplexEmitterData : BinCommonEmitterData {
    BitFlag<ChildFlag> childFlag;               // 0x1F4
    BitFlag<FieldFlag> fieldFlag;               // 0x1F6
    BitFlag<FluctuationFlag> fluctuationFlag;   // 0x1F8
    BitFlag<StripeFlag> stripeFlag;             // 0x1FA

    u16 childDataOffset;                        // 0x1FC
    u16 fieldDataOffset;                        // 0x1FE
    u16 fluctuationDataOffset;                  // 0x200
    u16 stripeDataOffset;                       // 0x202
    s32 mDataSize;                              // 0x204

    BinComplexEmitterData() = default;
    BinComplexEmitterData(const Ptcl::Emitter& emitter);

    friend QDataStream& operator>>(QDataStream& in, BinComplexEmitterData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinComplexEmitterData& item);

    void printData(u32 indentationLevel = 0);
};

static_assert(sizeof(BinComplexEmitterData) == 0x208, "BinComplexEmitterData is incorrect size.");


// ========================================================================== //


// Size 0xEC
struct alignas(4) BinChildData {
    std::array<u8, 44> _0;      // 0x00
    u32 _2C;                    // 0x2C
    std::array<u8, 4> _30;      // 0x30
    BinTextureRes textureRes;   // 0x34
    u32 textureSize;            // 0x40
    u32 texturePos;             // 0x44
    uptr32 textureHandlePtr;    // 0x48 - placeholder for a 32 bit TextureHandle* (should always be 0)
    std::array<u8, 16> _4C;     // 0x4C
    f32 _5C;                    // 0x5C
    f32 _60;                    // 0x60
    f32 _64;                    // 0x64
    std::array<u8, 84> _68;     // 0x68
    binVec3f _BC;               // 0xBC
    std::array<u8, 32> _C8;     // 0xC8
    f32 _E8;                    // 0xE8

    BinChildData() = default;
    BinChildData(const Ptcl::ChildData& childData);

    friend QDataStream& operator>>(QDataStream& in, BinChildData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinChildData& item);

    void printData(u32 indentationLevel = 0);
};

static_assert(sizeof(BinChildData) == 0xEC, "BinChildData is incorrect size.");


// ========================================================================== //


// Size 0x10
struct alignas(4) BinFieldRandomData {
    s32 fieldRandomBlank;       // 0x00
    binVec3f fieldRandomVelAdd; // 0x04

    BinFieldRandomData() = default;
    BinFieldRandomData(const Ptcl::FieldRandomData& fieldRandomData);

    friend QDataStream& operator>>(QDataStream& in, BinFieldRandomData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinFieldRandomData& item);
};

static_assert(sizeof(BinFieldRandomData) == 0x10, "BinFieldRandomData is incorrect size.");


// ========================================================================== //

// Size 0x14
struct alignas(4) BinFieldMagnetData {
    f32 fieldMagnetPower;       // 0x00
    binVec3f fieldMagnetPos;    // 0x04
    u32 fieldMagnetFlag;        // 0x10

    BinFieldMagnetData() = default;
    BinFieldMagnetData(const Ptcl::FieldMagnetData& fieldMagnetData);

    friend QDataStream& operator>>(QDataStream& in, BinFieldMagnetData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinFieldMagnetData& item);
};

static_assert(sizeof(BinFieldMagnetData) == 0x14, "BinFieldMagnetData is incorrect size.");


// ========================================================================== //

// Size 0x08
struct alignas(4) BinFieldSpinData {
    s32 fieldSpinRotate;    // 0x00
    s32 fieldSpinAxis;      // 0x04

    BinFieldSpinData() = default;
    BinFieldSpinData(const Ptcl::FieldSpinData& fieldSpinData);

    friend QDataStream& operator>>(QDataStream& in, BinFieldSpinData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinFieldSpinData& item);
};

static_assert(sizeof(BinFieldSpinData) == 0x8, "BinFieldSpinData is incorrect size.");


// ========================================================================== //


// Size 0x0C
struct alignas(4) BinFieldCollisionData {
    u16 fieldCollisionType;     // 0x00
    u16 fieldCollisionIsWorld;  // 0x02
    f32 fieldCollisionCoord;    // 0x04
    f32 fieldCollisionCoef;     // 0x08

    BinFieldCollisionData() = default;
    BinFieldCollisionData(const Ptcl::FieldCollisionData& fieldCollisionData);

    friend QDataStream& operator>>(QDataStream& in, BinFieldCollisionData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinFieldCollisionData& item);
};

static_assert(sizeof(BinFieldCollisionData) == 0xC, "BinFieldCollisionData is incorrect size.");


// ========================================================================== //


// Size 0x10
struct alignas(4) BinFieldConvergenceData {
    FieldConvergenceType fieldConvergenceType;  // 0x00
    binVec3f fieldConvergencePos;               // 0x04

    BinFieldConvergenceData() = default;
    BinFieldConvergenceData(const Ptcl::FieldConvergenceData& fieldConvergenceData);

    friend QDataStream& operator>>(QDataStream& in, BinFieldConvergenceData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinFieldConvergenceData& item);
};

static_assert(sizeof(BinFieldConvergenceData) == 0x10, "BinFieldConvergenceData is incorrect size.");


// ========================================================================== //


// Size 0x0C
struct alignas(4) BinFieldPosAddData {
    binVec3f fieldPosAdd;   // 0x00

    BinFieldPosAddData() = default;
    BinFieldPosAddData(const Ptcl::FieldPosAddData& fieldPosAddData);

    friend QDataStream& operator>>(QDataStream& in, BinFieldPosAddData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinFieldPosAddData& item);
};

static_assert(sizeof(BinFieldPosAddData) == 0xC, "BinFieldPosAddData is incorrect size.");


// ========================================================================== //


// Size 0x0C
struct alignas(4) BinFluctuationData {
    f32 fluctuationScale;       // 0x00
    f32 fluctuationFreq;        // 0x04
    u32 fluctuationPhaseRnd;    // 0x08

    BinFluctuationData() = default;
    BinFluctuationData(const Ptcl::FluctuationData& fluctuationData);

    friend QDataStream& operator>>(QDataStream& in, BinFluctuationData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinFluctuationData& item);
};

static_assert(sizeof(BinFluctuationData) == 0xC, "BinFluctuationData is incorrect size.");


// ========================================================================== //


// Size 0x20
struct alignas(4) BinStripeData {
    std::array<u8, 32> _0;       // 0x00

    BinStripeData() = default;
    BinStripeData(const Ptcl::StripeData& stripeData);

    friend QDataStream& operator>>(QDataStream& in, BinStripeData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinStripeData& item);
};

static_assert(sizeof(BinStripeData) == 0x20, "BinStripeData is incorrect size.");


// ========================================================================== //


// Size 0x08
struct alignas(4) BinEmitterTblData {
    u32 emitterPos;      // 0x00
    uptr32 emitterPtr;   // 0x04 - placeholder for a 32 bit Emitter* (should always be 0)

    friend QDataStream& operator>>(QDataStream& in, BinEmitterTblData& item);
    friend QDataStream& operator<<(QDataStream& out, const BinEmitterTblData& item);

    void printData(u32 indentationLevel = 0);
};

static_assert(sizeof(BinEmitterTblData) == 0x08, "BinEmitterTblData is incorrect size.");

#pragma pack(pop)


// ========================================================================== //


} // namespace Ptcl
