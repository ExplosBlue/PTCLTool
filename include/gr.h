#ifndef GR_H
#define GR_H

#include "typedefs.h"
#include "printUtil.h"

#include <QDebug>
#include <QString>
#include <QDataStream>
#include <QImage>

namespace gr {

#pragma pack(push, 4)

enum class PicaDataTextureFormat : u8
{
    RGBA8888   = 0x0,
    RGB888     = 0x1,
    RGBA5551   = 0x2,
    RGB565     = 0x3,
    RGBA4444   = 0x4,
    LA88       = 0x5,
    HL8        = 0x6,
    L8         = 0x7,
    A8         = 0x8,
    LA44       = 0x9,
    L4         = 0xA,
    A4         = 0xB,
    ETC1       = 0xC,
    ETC1_A4    = 0xD,
};

inline QDebug operator<<(QDebug dbg, const PicaDataTextureFormat& type)
{
    QDebugStateSaver stateSaver(dbg);

    switch (type) {
    case PicaDataTextureFormat::RGBA8888:
        dbg.nospace() << "RGBA8888";
        break;
    case PicaDataTextureFormat::RGB888:
        dbg.nospace() << "RGB888";
        break;
    case PicaDataTextureFormat::RGBA5551:
        dbg.nospace() << "RGBA5551";
        break;
    case PicaDataTextureFormat::RGB565:
        dbg.nospace() << "RGB565";
        break;
    case PicaDataTextureFormat::RGBA4444:
        dbg.nospace() << "RGBA4444";
        break;
    case PicaDataTextureFormat::LA88:
        dbg.nospace() << "LA88";
        break;
    case PicaDataTextureFormat::HL8:
        dbg.nospace() << "HL8";
        break;
    case PicaDataTextureFormat::L8:
        dbg.nospace() << "L8";
        break;
    case PicaDataTextureFormat::A8:
        dbg.nospace() << "A8";
        break;
    case PicaDataTextureFormat::LA44:
        dbg.nospace() << "LA44";
        break;
    case PicaDataTextureFormat::L4:
        dbg.nospace() << "L4";
        break;
    case PicaDataTextureFormat::A4:
        dbg.nospace() << "A4";
        break;
    case PicaDataTextureFormat::ETC1:
        dbg.nospace() << "ETC1";
        break;
    case PicaDataTextureFormat::ETC1_A4:
        dbg.nospace() << "ETC1_A4";
        break;
    }

    return dbg;
}

inline QString toQString(PicaDataTextureFormat type)
{
    switch (type) {
    case PicaDataTextureFormat::RGBA8888:
        return("RGBA8888");
        break;
    case PicaDataTextureFormat::RGB888:
        return("RGB888");
        break;
    case PicaDataTextureFormat::RGBA5551:
        return("RGBA5551");
        break;
    case PicaDataTextureFormat::RGB565:
        return("RGB565");
        break;
    case PicaDataTextureFormat::RGBA4444:
        return("RGBA4444");
        break;
    case PicaDataTextureFormat::LA88:
        return("LA88");
        break;
    case PicaDataTextureFormat::HL8:
        return("HL8");
        break;
    case PicaDataTextureFormat::L8:
        return("L8");
        break;
    case PicaDataTextureFormat::A8:
        return("A8");
        break;
    case PicaDataTextureFormat::LA44:
        return("LA44");
        break;
    case PicaDataTextureFormat::L4:
        return("L4");
        break;
    case PicaDataTextureFormat::A4:
        return("A4");
        break;
    case PicaDataTextureFormat::ETC1:
        return("ETC1");
        break;
    case PicaDataTextureFormat::ETC1_A4:
        return("ETC1_A4");
        break;
    }
}

enum PicaDataTextureWrap : u8
{
    PICA_DATA_TEXTURE_WRAP_CLAMP_TO_EDGE = 0x0,
    PICA_DATA_TEXTURE_WRAP_CLAMP_TO_BORDER = 0x1,
    PICA_DATA_TEXTURE_WRAP_REPEAT = 0x2,
    PICA_DATA_TEXTURE_WRAP_MIRRORED_REPEAT = 0x3,
};

enum PicaDataTextureMagFilter : u8
{
    PICA_DATA_TEXTURE_MAG_FILTER_NEAREST = 0x0,
    PICA_DATA_TEXTURE_MAG_FILTER_LINEAR = 0x1,
};

enum PicaDataTextureMinFilter : u8
{
    PICA_DATA_TEXTURE_MIN_FILTER_NEAREST = 0x0,
    PICA_DATA_TEXTURE_MIN_FILTER_NEAREST_MIPMAP_NEAREST = 0x1,
    PICA_DATA_TEXTURE_MIN_FILTER_NEAREST_MIPMAP_LINEAR = 0x2,
    PICA_DATA_TEXTURE_MIN_FILTER_LINEAR = 0x3,
    PICA_DATA_TEXTURE_MIN_FILTER_LINEAR_MIPMAP_NEAREST = 0x4,
    PICA_DATA_TEXTURE_MIN_FILTER_LINEAR_MIPMAP_LINEAR = 0x5,
};

// size: 0xA8
struct alignas(4) Texture {

    // size: 0x1C
    struct alignas(4) UnitBase
    {
        uptr32 physicalAddr;
        u16 width;
        u16 height;
        PicaDataTextureFormat format;
        PicaDataTextureWrap wrapT;
        PicaDataTextureWrap wrapS;
        PicaDataTextureMagFilter magFilter;
        PicaDataTextureMinFilter minFilter;
        f32 lodBias;
        u8 minLodLevel;
        u8 maxLodLevel;
        u8 borderColorR;
        u8 borderColorG;
        u8 borderColorB;
        u8 borderColorA;

        friend QDataStream &operator>>(QDataStream &in, UnitBase &item) {

            in >> item.physicalAddr
               >> item.width
               >> item.height
               >> item.format
               >> item.wrapT
               >> item.wrapS
               >> item.magFilter
               >> item.minFilter
               >> item.lodBias
               >> item.minLodLevel
               >> item.maxLodLevel
               >> item.borderColorR
               >> item.borderColorG
               >> item.borderColorB
               >> item.borderColorA;

            return in;
        }

        friend QDataStream &operator<<(QDataStream &out, const UnitBase &item) {

            out << item.physicalAddr
                << item.width
                << item.height
                << item.format
                << item.wrapT
                << item.wrapS
                << item.magFilter
                << item.minFilter
                << item.lodBias
                << item.minLodLevel
                << item.maxLodLevel
                << item.borderColorR
                << item.borderColorG
                << item.borderColorB
                << item.borderColorA;

            return out;
        }
    };

    static_assert(sizeof(UnitBase) == 0x1C, "UnitBase has incorrect size.");


    // size: 0x3C
    struct alignas(4) Unit0 : UnitBase
    {
        u8 texType;               // 0x1C
        uptr32 cubeMapAddrPosX;   // 0x20
        uptr32 cubeMapAddrNegX;   // 0x24
        uptr32 cubeMapAddrPosY;   // 0x28
        uptr32 cubeMapAddrNegY;   // 0x2C
        uptr32 cubeMapAddrPosZ;   // 0x30
        uptr32 cubeMapAddrNegZ;   // 0x34
        uptr32 m_TexturePtr;      // 0x38 - 32 bit const char* placeholder (should always be 0)

        friend QDataStream &operator>>(QDataStream &in, Unit0 &item) {

            in >> (UnitBase&)item
               >> item.texType
               >> item.cubeMapAddrPosX
               >> item.cubeMapAddrNegX
               >> item.cubeMapAddrPosY
               >> item.cubeMapAddrNegY
               >> item.cubeMapAddrPosZ
               >> item.cubeMapAddrNegZ
               >> item.m_TexturePtr;

            return in;
        }

        friend QDataStream &operator<<(QDataStream &out, const Unit0 &item) {

            out << (UnitBase&)item
                << item.texType
                << item.cubeMapAddrPosX
                << item.cubeMapAddrNegX
                << item.cubeMapAddrPosY
                << item.cubeMapAddrNegY
                << item.cubeMapAddrPosZ
                << item.cubeMapAddrNegZ
                << item.m_TexturePtr;

            return out;
        }

        void printData(u32 indentationLevel = 0) {

            const char* indentation = PrintUtil::createIndentation(indentationLevel);

            qDebug() << indentation << "- physicalAddr:    " << physicalAddr;
            qDebug() << indentation << "- width:           " << width;
            qDebug() << indentation << "- height:          " << height;
            qDebug() << indentation << "- format:          " << format;
            qDebug() << indentation << "- wrapT:           " << wrapT;
            qDebug() << indentation << "- wrapS:           " << wrapS;
            qDebug() << indentation << "- magFilter:       " << magFilter;
            qDebug() << indentation << "- minFilter:       " << minFilter;
            qDebug() << indentation << "- lodBias:         " << lodBias;
            qDebug() << indentation << "- minLodLevel:     " << minLodLevel;
            qDebug() << indentation << "- maxLodLevel:     " << maxLodLevel;
            qDebug() << indentation << "- borderColorR:    " << borderColorR;
            qDebug() << indentation << "- borderColorG:    " << borderColorG;
            qDebug() << indentation << "- borderColorB:    " << borderColorB;
            qDebug() << indentation << "- borderColorA:    " << borderColorA;

            qDebug() << indentation << "- texType:         " << texType;
            qDebug() << indentation << "- cubeMapAddrPosX: " << cubeMapAddrPosX;
            qDebug() << indentation << "- cubeMapAddrNegX: " << cubeMapAddrNegX;
            qDebug() << indentation << "- cubeMapAddrPosY: " << cubeMapAddrPosY;
            qDebug() << indentation << "- cubeMapAddrNegY: " << cubeMapAddrNegY;
            qDebug() << indentation << "- cubeMapAddrPosZ: " << cubeMapAddrPosZ;
            qDebug() << indentation << "- cubeMapAddrNegZ: " << cubeMapAddrNegZ;
            qDebug() << indentation << "- m_Texture:       " << m_TexturePtr;
        }
    };

    static_assert(sizeof(Unit0) == 0x3C, "Unit0 has incorrect size.");


    // size: 0x24
    struct alignas(4) Unit1 : UnitBase
    {
        u8 texType;            // 0x1C
        uptr32 m_TexturePtr;   // 0x20 - 32 bit const char* placeholder (should always be 0)

        friend QDataStream &operator>>(QDataStream &in, Unit1 &item) {

            in >> (UnitBase&)item
               >> item.texType
               >> item.m_TexturePtr;

            return in;
        }

        friend QDataStream &operator<<(QDataStream &out, const Unit1 &item) {

            out << (UnitBase&)item;

            out << item.texType;
            out.writeRawData(0, 4); // m_Texture

            return out;
        }

        void printData(u32 indentationLevel = 0) {

            const char* indentation = PrintUtil::createIndentation(indentationLevel);

            qDebug() << indentation << "- physicalAddr: " << physicalAddr;
            qDebug() << indentation << "- width:        " << width;
            qDebug() << indentation << "- height:       " << height;
            qDebug() << indentation << "- format:       " << format;
            qDebug() << indentation << "- wrapT:        " << wrapT;
            qDebug() << indentation << "- wrapS:        " << wrapS;
            qDebug() << indentation << "- magFilter:    " << magFilter;
            qDebug() << indentation << "- minFilter:    " << minFilter;
            qDebug() << indentation << "- lodBias:      " << lodBias;
            qDebug() << indentation << "- minLodLevel:  " << minLodLevel;
            qDebug() << indentation << "- maxLodLevel:  " << maxLodLevel;
            qDebug() << indentation << "- borderColorR: " << borderColorR;
            qDebug() << indentation << "- borderColorG: " << borderColorG;
            qDebug() << indentation << "- borderColorB: " << borderColorB;
            qDebug() << indentation << "- borderColorA: " << borderColorA;

            qDebug() << indentation << "- texType:      " << texType;
            qDebug() << indentation << "- m_Texture:    " << m_TexturePtr;
        }
    };

    static_assert(sizeof(Unit1) == 0x24, "Unit1 has incorrect size.");


    // size: 0x24
    struct alignas(4) Unit2 : UnitBase
    {
        u8 texType;            // 0x1C
        u8 texCoord;           // 0x1D
        uptr32 m_TexturePtr;   // 0x20 - 32 bit const char* placeholder (should always be 0)

        friend QDataStream &operator>>(QDataStream &in, Unit2 &item) {

            in >> (UnitBase&)item
               >> item.texType
               >> item.texCoord
               >> item.m_TexturePtr;

            return in;
        }

        friend QDataStream &operator<<(QDataStream &out, const Unit2 &item) {

            out << (UnitBase&)item
                << item.texType
                << item.texCoord
                << item.m_TexturePtr;

            return out;
        }

        void printData(u32 indentationLevel = 0) {

            const char* indentation = PrintUtil::createIndentation(indentationLevel);

            qDebug() << indentation << "- physicalAddr: " << physicalAddr;
            qDebug() << indentation << "- width:        " << width;
            qDebug() << indentation << "- height:       " << height;
            qDebug() << indentation << "- format:       " << format;
            qDebug() << indentation << "- wrapT:        " << wrapT;
            qDebug() << indentation << "- wrapS:        " << wrapS;
            qDebug() << indentation << "- magFilter:    " << magFilter;
            qDebug() << indentation << "- minFilter:    " << minFilter;
            qDebug() << indentation << "- lodBias:      " << lodBias;
            qDebug() << indentation << "- minLodLevel:  " << minLodLevel;
            qDebug() << indentation << "- maxLodLevel:  " << maxLodLevel;
            qDebug() << indentation << "- borderColorR: " << borderColorR;
            qDebug() << indentation << "- borderColorG: " << borderColorG;
            qDebug() << indentation << "- borderColorB: " << borderColorB;
            qDebug() << indentation << "- borderColorA: " << borderColorA;

            qDebug() << indentation << "- texType:      " << texType;
            qDebug() << indentation << "- texCoord:     " << texCoord;
            qDebug() << indentation << "- m_Texture:    " << m_TexturePtr;
        }
    };

    static_assert(sizeof(Unit2) == 0x24, "Unit2 has incorrect size.");


    // size: 0x24
    struct alignas(4) Unit3 : UnitBase
    {
        u8 texType;            // 0x1C
        u8 texCoord;           // 0x1D
        uptr32 m_TexturePtr;   // 0x20 - 32 bit const char* placeholder (should always be 0)

        friend QDataStream &operator>>(QDataStream &in, Unit3 &item) {

            in >> (UnitBase&)item
                >> item.texType
                >> item.texCoord
                >> item.m_TexturePtr;

            return in;
        }

        friend QDataStream &operator<<(QDataStream &out, const Unit3 &item) {

            out << (UnitBase&)item
                << item.texType
                << item.texCoord
                << item.m_TexturePtr;

            return out;
        }

        void printData(u32 indentationLevel = 0) {

            const char* indentation = PrintUtil::createIndentation(indentationLevel);

            qDebug() << indentation << "- physicalAddr: " << physicalAddr;
            qDebug() << indentation << "- width:        " << width;
            qDebug() << indentation << "- height:       " << height;
            qDebug() << indentation << "- format:       " << format;
            qDebug() << indentation << "- wrapT:        " << wrapT;
            qDebug() << indentation << "- wrapS:        " << wrapS;
            qDebug() << indentation << "- magFilter:    " << magFilter;
            qDebug() << indentation << "- minFilter:    " << minFilter;
            qDebug() << indentation << "- lodBias:      " << lodBias;
            qDebug() << indentation << "- minLodLevel:  " << minLodLevel;
            qDebug() << indentation << "- maxLodLevel:  " << maxLodLevel;
            qDebug() << indentation << "- borderColorR: " << borderColorR;
            qDebug() << indentation << "- borderColorG: " << borderColorG;
            qDebug() << indentation << "- borderColorB: " << borderColorB;
            qDebug() << indentation << "- borderColorA: " << borderColorA;

            qDebug() << indentation << "- texType:      " << texType;
            qDebug() << indentation << "- texCoord:     " << texCoord;
            qDebug() << indentation << "- m_Texture:    " << m_TexturePtr;
        }
    };

    static_assert(sizeof(Unit3) == 0x24, "Unit3 has incorrect size.");


    Unit0 unit0;
    Unit1 unit1;
    Unit2 unit2;
    Unit3 unit3;

    friend QDataStream &operator>>(QDataStream &in, Texture &item) {

        in >> item.unit0
           >> item.unit1
           >> item.unit2
           >> item.unit3;

        return in;
    }

    friend QDataStream &operator<<(QDataStream &out, const Texture &item) {

        out << item.unit0
            << item.unit1
            << item.unit2
            << item.unit3;

        return out;
    }

    void printData(u32 indentationLevel = 0) {

        const char* indentation = PrintUtil::createIndentation(indentationLevel);

        qDebug() << indentation << "- unit0:";
        unit0.printData(indentationLevel + 1);
        qDebug() << indentation << "- unit1:";
        unit1.printData(indentationLevel + 1);
        qDebug() << indentation << "- unit2:";
        unit2.printData(indentationLevel + 1);
        qDebug() << indentation << "- unit3:";
        unit3.printData(indentationLevel + 1);
    }
};

static_assert(sizeof(Texture) == 0xA8, "Texture has incorrect size.");


#pragma pack(pop)

} // namespace gr

#endif // GR_H
