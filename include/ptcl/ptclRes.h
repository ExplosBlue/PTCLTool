#ifndef PTCLRES_H
#define PTCLRES_H

#include "../typedefs.h"
#include "../printUtil.h"
#include "../gr.h"

#include "ptclEnums.h"

#include <QDebug>
#include <QString>
#include <QDataStream>
#include <QVector2D>
#include <QVector3D>
#include <QMatrix3x4>
#include <QColor>


namespace Ptcl {

#pragma pack(push, 4)

// Size 0x08
struct alignas(4) binVec2f {

    f32 x; //   0x00
    f32 y; //   0x04


    friend QDataStream& operator>>(QDataStream& in, binVec2f& item) {

        in >> item.x
           >> item.y;

        return in;
    }

    friend QDataStream& operator<<(QDataStream& out, const binVec2f& item) {

        out << item.x
            << item.y;

        return out;
    }

    friend QDebug operator<<(QDebug dbg, const binVec2f& item) {

        QDebugStateSaver stateSaver(dbg);
        dbg.nospace() << "{" << item.x << "," << item.y << "}";
        return dbg;
    }

    binVec2f() :
        x(1.0f), y(1.0f) {}

    binVec2f(const QVector2D& vec) :
        x(vec.x()), y(vec.y()) {}
};

static_assert(sizeof(binVec2f) == 0x08, "binVec2f is incorrect size.");


// Size 0x0C
struct alignas(4) binVec3f {

    f32 x;   // 0x00
    f32 y;   // 0x04
    f32 z;   // 0x08

    friend QDataStream& operator>>(QDataStream& in, binVec3f& item) {

        in >> item.x >> item.y >> item.z;
        return in;
    }

    friend QDataStream& operator<<(QDataStream& out, const binVec3f& item) {

        out << item.x << item.y << item.z;
        return out;
    }

    friend QDebug operator<<(QDebug dbg, const binVec3f& item) {

        QDebugStateSaver stateSaver(dbg);
        dbg.nospace() << "{" << item.x << "," << item.y << "," << item.z << "}";
        return dbg;
    }

    binVec3f() :
        x(1.0f), y(1.0f), z(1.0f) {}

    binVec3f(const QVector3D& vec) :
        x(vec.x()), y(vec.y()), z(vec.z()) {}
};

static_assert(sizeof(binVec3f) == 0x0C, "binVec3f is incorrect size.");


// Size 0x30
union alignas(4) binMtx34f {

    std::array<std::array<f32, 3>, 4> rows;   // 0x00 - 0x30
    std::array<f32, 12> cells;                // 0x00 - 0x30

    friend QDataStream& operator>>(QDataStream& in, binMtx34f& item) {

        in.readRawData(reinterpret_cast<char*>(item.cells.data()), sizeof(binMtx34f));
        return in;
    }

    friend QDataStream& operator<<(QDataStream& out, const binMtx34f& item) {

        out.writeRawData(reinterpret_cast<const char*>(item.cells.data()), sizeof(binMtx34f));
        return out;
    }

    friend QDebug operator<<(QDebug dbg, const binMtx34f& item) {

        QDebugStateSaver stateSaver(dbg);
        dbg.nospace() << "{{" << item.rows[0][0] << "," << item.rows[0][1] << "," << item.rows[0][2] << "," << item.rows[0][3] << "} \n"
                      << " {" << item.rows[1][0] << "," << item.rows[1][1] << "," << item.rows[1][2] << "," << item.rows[1][3] << "} \n"
                      << " {" << item.rows[2][0] << "," << item.rows[2][1] << "," << item.rows[2][2] << "," << item.rows[2][3] << "}}";
        return dbg;
    }

    binMtx34f() {}

    binMtx34f(const QMatrix3x4& mtx) {
        const f32* mtxData = mtx.data();
        for (int i = 0; i < 12; i++) {
            cells[i] = mtxData[i];
        }
    }
};

static_assert(sizeof(binMtx34f) == 0x30, "binMtx34f is incorrect size.");


// Size 0x10
struct alignas(4) binColor4f {

    f32 r;   // 0x00
    f32 g;   // 0x04
    f32 b;   // 0x08
    f32 a;   // 0x0C

    friend QDataStream& operator>>(QDataStream& in, binColor4f& item) {

        in >> item.r >> item.g >> item.b >> item.a;
        return in;
    }

    friend QDataStream& operator<<(QDataStream& out, const binColor4f& item) {

        out << item.r << item.g << item.b << item.a;
        return out;
    }

    friend QDebug operator<<(QDebug dbg, const binColor4f& item) {

        QDebugStateSaver stateSaver(dbg);
        dbg.nospace() << "{" << item.r << "," << item.g << "," << item.b << "," << item.a << "}";
        return dbg;
    }

    binColor4f() :
        r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}

    binColor4f(const QColor& color) :
        r(color.redF()), g(color.greenF()), b(color.blueF()), a(color.alphaF()) {}
};

static_assert(sizeof(binColor4f) == 0x10, "binColor4f is incorrect size.");


// Size 0x1C
struct alignas(4) BinHeaderData {

    std::array<char, 4> magic;   // 0x00
    u32 version;                 // 0x04
    s32 numEmitterSet;           // 0x08
    s32 namePos;                 // 0x0C
    s32 nameTblPos;              // 0x10
    s32 textureTblPos;           // 0x14
    s32 textureTblSize;          // 0x18

    friend QDataStream& operator>>(QDataStream& in, BinHeaderData& item) {

        in.readRawData(item.magic.data(), 4);
        in >> item.version
           >> item.numEmitterSet
           >> item.namePos
           >> item.nameTblPos
           >> item.textureTblPos
           >> item.textureTblSize;
        return in;
    }

    friend QDataStream& operator<<(QDataStream& out, const BinHeaderData& item) {

        out << item.magic.data()
            << item.version
            << item.numEmitterSet
            << item.namePos
            << item.nameTblPos
            << item.textureTblPos
            << item.textureTblSize;
        return out;
    }

    void printData(u32 indentationLevel = 0) {

        const char* indentation = PrintUtil::createIndentation(indentationLevel);

        qDebug() << indentation << "- magic:"          << QString::fromUtf8(magic);
        qDebug() << indentation << "- version:"        << Qt::hex << Qt::showbase << version;
        qDebug() << indentation << "- numEmitterSet:"  << Qt::hex << Qt::showbase << numEmitterSet;
        qDebug() << indentation << "- namePos:"        << Qt::hex << Qt::showbase << namePos;
        qDebug() << indentation << "- nameTblPos:"     << Qt::hex << Qt::showbase << nameTblPos;
        qDebug() << indentation << "- textureTblPos:"  << Qt::hex << Qt::showbase << textureTblPos;
        qDebug() << indentation << "- textureTblSize:" << Qt::hex << Qt::showbase << textureTblSize;
    }
};

static_assert(sizeof(BinHeaderData) == 0x1C, "BinHeaderData is incorrect size.");


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

    friend QDataStream &operator>>(QDataStream& in, BinEmitterSetData& item) {

        in >> item.userData
            >> item.lastUpdateDate
            >> item.namePos
            >> item.namePtr
            >> item.numEmitter
            >> item.emitterTblPos
            >> item.emitterTbl;
        return in;
    }

    friend QDataStream &operator<<(QDataStream& out, const BinEmitterSetData& item) {

        out << item.userData
            << item.lastUpdateDate
            << item.namePos
            << item.namePtr
            << item.numEmitter
            << item.emitterTblPos
            << item.emitterTbl;
        return out;
    }

    void printData(u32 indentationLevel = 0) {

        const char* indentation = PrintUtil::createIndentation(indentationLevel);

        qDebug() << indentation << "- userData:       " << Qt::hex << Qt::showbase << userData;
        qDebug() << indentation << "- lastUpdateDate: " << Qt::hex << Qt::showbase << lastUpdateDate;
        qDebug() << indentation << "- namePos:        " << Qt::hex << Qt::showbase << namePos;
        qDebug() << indentation << "- name:           " << Qt::hex << Qt::showbase << namePtr;
        qDebug() << indentation << "- numEmitter:     " << Qt::hex << Qt::showbase << numEmitter;
        qDebug() << indentation << "- emitterTblPos:  " << Qt::hex << Qt::showbase << emitterTblPos;
        qDebug() << indentation << "- emitterTbl:     " << Qt::hex << Qt::showbase << emitterTbl;
    }
};

static_assert(sizeof(BinEmitterSetData) == 0x1C, "BinEmitterSetData is incorrect size.");


// Size 0x0C
struct alignas(4) BinTextureRes
{
    u16 width;                                // 0x00
    u16 height;                               // 0x02
    gr::PicaDataTextureFormat format;         // 0x04 - PicaDataTextureFormat
    gr::PicaDataTextureWrap wrapT;            // 0x05 - PicaDataTextureWrap
    gr::PicaDataTextureWrap wrapS;            // 0x06 - PicaDataTextureWrap
    gr::PicaDataTextureMagFilter magFilter;   // 0x07 - PicaDataTextureMagFilter
    gr::PicaDataTextureMinFilter minFilter;   // 0x08 - PicaDataTextureMinFilter

    friend QDataStream& operator>>(QDataStream& in, BinTextureRes& item) {

        in >> item.width
           >> item.height
           >> item.format
           >> item.wrapT
           >> item.wrapS
           >> item.magFilter
           >> item.minFilter;

        // Account for padding
        in.readRawData(nullptr, 3);

        return in;
    }

    friend QDataStream& operator<<(QDataStream& out, const BinTextureRes& item) {

        out << item.width
            << item.height
            << item.format
            << item.wrapT
            << item.wrapS
            << item.magFilter
            << item.minFilter;

        // Account for padding
        out.writeRawData(nullptr, 3);

        return out;
    }

    void printData(u32 indentationLevel = 0) {

        const char* indentation = PrintUtil::createIndentation(indentationLevel);

        qDebug() << indentation << "- width:     " << width;
        qDebug() << indentation << "- height:    " << height;
        qDebug() << indentation << "- format:    " << format;
        qDebug() << indentation << "- wrapT:     " << wrapT;
        qDebug() << indentation << "- wrapS:     " << wrapS;
        qDebug() << indentation << "- magFilter: " << magFilter;
        qDebug() << indentation << "- minFilter: " << minFilter;
    }
};

static_assert(sizeof(BinTextureRes) == 0x0C, "BinTextureRes is incorrect size.");


// Size 0xB4
struct alignas(4) BinTextureHandle
{
    gr::Texture grTexture;   // 0x00
    u32 _A8;                 // 0xA8
    u32 _AC;                 // 0xAC
    u32 _B0;                 // 0xB0

    friend QDataStream& operator>>(QDataStream& in, BinTextureHandle& item) {

        in >> item.grTexture
           >> item._A8
           >> item._AC
           >> item._B0;

        return in;
    }

    friend QDataStream& operator<<(QDataStream& out, const BinTextureHandle& item) {

        out << item.grTexture
            << item._A8
            << item._AC
            << item._B0;

        return out;
    }

    void printData(u32 indentationLevel = 0) {

        const char* indentation = PrintUtil::createIndentation(indentationLevel);

        grTexture.printData(indentationLevel + 1);
        qDebug() << indentation << "- _A8: " << _A8;
        qDebug() << indentation << "- _AC: " << _AC;
        qDebug() << indentation << "- _B0: " << _B0;
    }
};

static_assert(sizeof(BinTextureHandle) == 0xB4, "BinTextureHandle is incorrect size.");


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


    friend QDataStream& operator>>(QDataStream& in, BinCommonEmitterData& item) {

        in >> item.type
           >> item.flag
           >> item.randomSeed
           >> item.namePos
           >> item.namePtr
           >> item.textureRes
           >> item.textureSize
           >> item.texturePos
           >> item.textureHandlePtr
           >> item._2C
           >> item._2D
           >> item._2E
           >> item._2F
           >> item._30
           >> item._31
           >> item._32
           >> item.volumeType
           >> item.volumeRadius
           >> item._44
           >> item._48
           >> item._4C;
        in.readRawData(reinterpret_cast<char*>(item._50), sizeof(item._50));
        in >> item.ptclLife
           >> item._88
           >> item._8C;
        in.readRawData(reinterpret_cast<char*>(item._90), sizeof(item._90));
        in >> item._9C
           >> item._A0
           >> item._A4
           >> item.color[0]
           >> item.color[1]
           >> item.color[2];
        in.readRawData(reinterpret_cast<char*>(item._D8), sizeof(item._D8));
        in >> item.colorSection1
           >> item.colorSection2
           >> item.colorSection3
           >> item.colorNumRepeat
           >> item.initAlpha;
        in.readRawData(reinterpret_cast<char*>(item._F8), sizeof(item._F8));
        in >> item.initScale;
        in.readRawData(reinterpret_cast<char*>(item._110), sizeof(item._110));
        in >> item._170
           >> item._1A0;
        in.readRawData(reinterpret_cast<char*>(item._1D0), sizeof(item._1D0));

        return in;
    }

    friend QDataStream& operator<<(QDataStream& out, const BinCommonEmitterData& item) {

        out << item.type
            << item.flag
            << item.randomSeed
            << item.namePos
            << item.namePtr
            << item.textureRes
            << item.textureSize
            << item.texturePos
            << item.textureHandlePtr
            << item._2C
            << item._2D
            << item._2E
            << item._2F
            << item._30
            << item._31
            << item._32
            << item.volumeType
            << item.volumeRadius
            << item._44
            << item._48
            << item._4C;
        out.writeRawData(reinterpret_cast<const char*>(item._50), sizeof(item._50));
        out << item.ptclLife
            << item._88
            << item._8C;
        out.writeRawData(reinterpret_cast<const char*>(item._90), sizeof(item._90));
        out << item._9C
            << item._A0
            << item._A4
            << item.color[0]
            << item.color[1]
            << item.color[2];
        out.writeRawData(reinterpret_cast<const char*>(item._D8), sizeof(item._D8));
        out << item.colorSection1
            << item.colorSection2
            << item.colorSection3
            << item.colorNumRepeat
            << item.initAlpha;
        out.writeRawData(reinterpret_cast<const char*>(item._F8), sizeof(item._F8));
        out << item.initScale;
        out.writeRawData(reinterpret_cast<const char*>(item._110), sizeof(item._110));
        out << item._170
            << item._1A0;
        out.writeRawData(reinterpret_cast<const char*>(item._1D0), sizeof(item._1D0));

        return out;
    }

    void printData(u32 indentationLevel = 0) {

        const char* indentation = PrintUtil::createIndentation(indentationLevel);

        qDebug() << indentation << "- type:          " << type;
        qDebug() << indentation << "- flag:          " << flag;
        qDebug() << indentation << "- randomSeed:    " << randomSeed;
        qDebug() << indentation << "- namePos:       " << namePos;
        qDebug() << indentation << "- namePtr:       " << namePtr;
        qDebug() << indentation << "- textureRes:    ";
        textureRes.printData(indentationLevel + 1);
        qDebug() << indentation << "- textureSize:      " << textureSize;
        qDebug() << indentation << "- texturePos:       " << texturePos;
        qDebug() << indentation << "- textureHandlePtr: " << textureHandlePtr;
        qDebug() << indentation << "- _2C:              " << _2C;
        qDebug() << indentation << "- _2D:              " << _2D;
        qDebug() << indentation << "- _2E:              " << _2E;
        qDebug() << indentation << "- _2F:              " << _2F;
        qDebug() << indentation << "- _30:              " << _30;
        qDebug() << indentation << "- _31:              " << _31;
        qDebug() << indentation << "- _32:              " << _32;
        qDebug() << indentation << "- volumeType:       " << volumeType;
        qDebug() << indentation << "- volumeRadius:     " << volumeRadius;
        qDebug() << indentation << "- _44:              " << _44;
        qDebug() << indentation << "- _48:              " << _48;
        qDebug() << indentation << "- _4C:              " << _4C;
        qDebug() << indentation << "- _50:              " << _50;
        qDebug() << indentation << "- ptclLife:         " << ptclLife;
        qDebug() << indentation << "- _88:              " << _88;
        qDebug() << indentation << "- _8C:              " << _8C;
        qDebug() << indentation << "- _90:              " << _90;
        qDebug() << indentation << "- _9C:              " << _9C;
        qDebug() << indentation << "- _A0:              " << _A0;
        qDebug() << indentation << "- _A4:              " << _A4;
        qDebug() << indentation << "- color:            " << color;
        qDebug() << indentation << "- _D8:              " << _D8;
        qDebug() << indentation << "- colorSection1:    " << colorSection1;
        qDebug() << indentation << "- colorSection2:    " << colorSection2;
        qDebug() << indentation << "- colorSection3:    " << colorSection3;
        qDebug() << indentation << "- colorNumRepeat:   " << colorNumRepeat;
        qDebug() << indentation << "- initAlpha:        " << initAlpha;
        qDebug() << indentation << "- _F8:              " << _F8;
        qDebug() << indentation << "- initScale:        " << initScale;
        qDebug() << indentation << "- _110:             " << _110;
        qDebug() << indentation << "- _170:             " << _170;
        qDebug() << indentation << "- _1A0:             " << _1A0;
        qDebug() << indentation << "- _1D0:             " << _1D0;
    }
};

static_assert(sizeof(BinCommonEmitterData) == 0x1F4, "BinCommonEmitterData is incorrect size.");


// Size 0x08
struct alignas(4) BinEmitterTblData {

    s32 emitterPos;      // 0x00
    uptr32 emitterPtr;   // 0x04 - placeholder for a 32 bit Emitter* (should always be 0)

    friend QDataStream& operator>>(QDataStream& in, BinEmitterTblData& item) {

        in >> item.emitterPos
           >> item.emitterPtr;

        return in;
    }

    friend QDataStream& operator<<(QDataStream& out, const BinEmitterTblData& item) {

        out << item.emitterPos
            << item.emitterPtr;

        return out;
    }

    void printData(u32 indentationLevel = 0) {

        const char* indentation = PrintUtil::createIndentation(indentationLevel);

        qDebug() << indentation << "- emitterPos: " << emitterPos;
        qDebug() << indentation << "- emitterPtr: " << emitterPtr;
    }
};

static_assert(sizeof(BinEmitterTblData) == 0x08, "BinEmitterTblData is incorrect size.");


// // Size 0x28
// struct alignas(4) BinResourceEmitterSet {

//     uptr32 setDataPtr;      // 0x00 - placeholder for a 32 bit BinEmitterSetData* (should always be 0)
//     uptr32 tblDataPtr;      // 0x04 - placeholder for a 32 bit BinEmitterTblData* (should always be 0)
//     s32 numEmitter;         // 0x08
//     u32 userData;           // 0x0C
//     uptr32 setDataROMPtr;   // 0x10 - placeholder for a 32 bit BinEmitterSetData* (should always be 0)
//     uptr32 tblDataROMPtr;   // 0x14 - placeholder for a 32 bit BinEmitterTblData* (should always be 0)
//     s32 numEmitterROM;      // 0x18
//     u32 userDataROM;        // 0x1C
//     uptr32 setNamePtr;      // 0x20 - placeholder for a 32 bit const char* (should always be 0)
//     bool isShowDetail;      // 0x24

//     void printData(u32 indentationLevel = 0) {

//         const char* indentation = PrintUtil::createIndentation(indentationLevel);

//         qDebug() << indentation << "- setDataPtr:    " << setDataPtr;
//         qDebug() << indentation << "- tblDataPtr:    " << tblDataPtr;
//         qDebug() << indentation << "- numEmitter:    " << numEmitter;
//         qDebug() << indentation << "- userData:      " << userData;
//         qDebug() << indentation << "- setDataROMPtr: " << setDataROMPtr;
//         qDebug() << indentation << "- tblDataROMPtr: " << setDataROMPtr;
//         qDebug() << indentation << "- numEmitterROM: " << numEmitterROM;
//         qDebug() << indentation << "- userDataROM:   " << userDataROM;
//         qDebug() << indentation << "- setNamePtr:    " << setNamePtr;
//         qDebug() << indentation << "- isShowDetail:  " << isShowDetail;
//         qDebug() << "\n";
//     }
// };

// static_assert(sizeof(BinResourceEmitterSet) == 0x28, "BinResourceEmitterSet is incorrect size.");


#pragma pack(pop)

}

#endif //PTCLRES_H
