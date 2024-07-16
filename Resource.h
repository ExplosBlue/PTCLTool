#ifndef RESOURCE_H
#define RESOURCE_H

#include "typedefs.h"

#include <QDebug>
#include <QString>
#include <QDataStream>


namespace ptcl {

#pragma pack(push, 4)

struct alignas(4) HeaderData {

    char magic[4];
    u32 version;
    s32 numEmitterSet;
    s32 namePos;
    s32 nameTblPos;
    s32 textureTblPos;
    s32 textureTblSize;

    friend QDataStream &operator>>(QDataStream &in, HeaderData &item) {

        in.readRawData(item.magic, 4);
        in >> item.version
           >> item.numEmitterSet
           >> item.namePos
           >> item.nameTblPos
           >> item.textureTblPos
           >> item.textureTblSize;
        return in;
    }

    friend QDataStream &operator<<(QDataStream &out, const HeaderData &item) {

        out << item.magic
            << item.version
            << item.numEmitterSet
            << item.namePos
            << item.nameTblPos
            << item.textureTblPos
            << item.textureTblSize;
        return out;
    }

    void printData() {

        qDebug() << "Header:";
        qDebug() << " - magic:"          << QString::fromUtf8(magic);
        qDebug() << " - version:"        << Qt::hex << Qt::showbase << version;
        qDebug() << " - numEmitterSet:"  << Qt::hex << Qt::showbase << numEmitterSet;
        qDebug() << " - namePos:"        << Qt::hex << Qt::showbase << namePos;
        qDebug() << " - nameTblPos:"     << Qt::hex << Qt::showbase << nameTblPos;
        qDebug() << " - textureTblPos:"  << Qt::hex << Qt::showbase << textureTblPos;
        qDebug() << " - textureTblSize:" << Qt::hex << Qt::showbase << textureTblSize;
    }
};

const u32 RES_HEADER_DATA_SIZE = 0x1C;

struct alignas(4) EmitterSetData
{
    u32 userData;
    u32 lastUpdateDate;
    s32 namePos;
    const char *name = nullptr; // Check Alignment
    u32 numEmitter;
    u32 emitterTblPos;
    u32 emitterTbl;

    friend QDataStream &operator>>(QDataStream &in, EmitterSetData &item) {

        in >> item.userData
            >> item.lastUpdateDate
            >> item.namePos;
        in.skipRawData(4); // name
        in  >> item.numEmitter
            >> item.emitterTblPos
            >> item.emitterTbl;
        return in;
    }

    friend QDataStream &operator<<(QDataStream &out, const EmitterSetData &item) {

        out << item.userData
            << item.lastUpdateDate
            << item.namePos;
        out.writeBytes(0, 4); // name
        out << item.numEmitter
            << item.emitterTblPos
            << item.emitterTbl;
        return out;
    }

    void printData() {

        qDebug() << "     - userData:"        << Qt::hex << Qt::showbase << userData;
        qDebug() << "     - lastUpdateDate:"  << Qt::hex << Qt::showbase << lastUpdateDate;
        qDebug() << "     - namePos:"         << Qt::hex << Qt::showbase << namePos;
        qDebug() << "     - name:"            << Qt::hex << Qt::showbase << name;
        qDebug() << "     - numEmitter:"      << Qt::hex << Qt::showbase << numEmitter;
        qDebug() << "     - emitterTblPos:"   << Qt::hex << Qt::showbase << emitterTblPos;
        qDebug() << "     - emitterTbl:"      << Qt::hex << Qt::showbase << emitterTbl;
    }

};

const u32 EMITTER_SET_DATA_SIZE = 0x1C;
//static_assert(sizeof(EmitterSetData) == EMITTER_SET_DATA_SIZE, "EmitterSetData size incorrect");

enum EmitterType : u32
{
    EFT_EMITTER_TYPE_SIMPLE = 0x0,
    EFT_EMITTER_TYPE_COMPLEX = 0x1,
    EFT_EMITTER_TYPE_MAX = 0x2,
    EFT_EMITTER_TYPE_FORCE_4BYTE = 0x80000000,
};


struct alignas(4) TextureInfo
{
    u16 width;
    u16 height;
    u8 format;    // PicaDataTextureFormat
    u8 wrapT;     // PicaDataTextureWrap
    u8 wrapS;     // PicaDataTextureWrap
    u8 magFilter; // PicaDataTextureMagFilter
    u8 minFilter; // PicaDataTextureMinFilter

    friend QDataStream &operator>>(QDataStream &in, TextureInfo &item) {

        in >> item.width
           >> item.height
           >> item.format
           >> item.wrapT
           >> item.wrapS
           >> item.magFilter
           >> item.minFilter;

        return in;
    }

    friend QDataStream &operator<<(QDataStream &out, const TextureInfo &item) {

        out << item.width
            << item.height
            << item.format
            << item.wrapT
            << item.wrapS
            << item.magFilter
            << item.minFilter;

        return out;
    }

    void printData() {

        qDebug() << "           - width:     " << width;
        qDebug() << "           - height:    " << height;
        qDebug() << "           - format:    " << format;
        qDebug() << "           - wrapT:     " << wrapT;
        qDebug() << "           - wrapS:     " << wrapS;
        qDebug() << "           - magFilter: " << magFilter;
        qDebug() << "           - minFilter: " << minFilter;
    }

};

const u32 TEXTURE_INFO_SIZE = 0x0C;
//static_assert(sizeof(TextureInfo) == TEXTURE_INFO_SIZE, "TextureInfo size incorrect");

struct alignas(4) CommonEmitterData
{
    EmitterType type;
    u32 flag;
    u32 randomSeed;
    s32 namePos;
    char *name;
    TextureInfo textureInfo;
    u32 _20;
    u32 texturePos;
    void *texture;
    u32 _2C[138];

    friend QDataStream &operator>>(QDataStream &in, CommonEmitterData &item) {

        in >> item.type
           >> item.flag
           >> item.randomSeed
           >> item.namePos;
        in.skipRawData(4); // name
        in >> item.textureInfo
           >> item._20
           >> item.texturePos;
        in.skipRawData(4); // texture
        in.readRawData(reinterpret_cast<char*>(item._2C), 138 * 4);

        return in;
    }

    friend QDataStream &operator<<(QDataStream &out, const CommonEmitterData &item) {

        out << item.type
            << item.flag
            << item.randomSeed
            << item.namePos;
        out.writeBytes(0, 4); // name
        out << item.textureInfo
            << item._20
            << item.texturePos;
        out.writeBytes(0, 4); // Texture
        out.writeRawData(reinterpret_cast<const char*>(item._2C), 138 * 4);

        return out;
    }

    void printData() {

        qDebug() << "         - type:        " << type;
        qDebug() << "         - flag:        " << flag;
        qDebug() << "         - randomSeed:  " << randomSeed;
        qDebug() << "         - namePos:     " << namePos;
        qDebug() << "         - name:        " << name;
        qDebug() << "         - textureInfo: ";
        textureInfo.printData();
        qDebug() << "         - _20:         " << _20;
        qDebug() << "         - texturePos:  " << texturePos;
        qDebug() << "         - texture:     " << texture;
        qDebug() << "         - _2C:         " << _2C;
    }

};

const u32 COMMON_EMITTER_DATA_SIZE = 0x1F4;


struct alignas(4) EmitterTblData {

    s32 emitterPos;
    CommonEmitterData *emitter;

    friend QDataStream &operator>>(QDataStream &in, EmitterTblData &item) {

        in >> item.emitterPos;
        in.skipRawData(4); // emitter

        return in;
    }

    friend QDataStream &operator<<(QDataStream &out, const EmitterTblData &item) {

        out << item.emitterPos;
        out.writeBytes(0, 4); // emitter

        return out;
    }

    void printData() {

        qDebug() << "       - emitterPos: " <<  emitterPos;
        qDebug() << "       - emitter:    ";
        emitter->printData();
    }

};

const u32 EMITTER_TBL_DATA_SIZE = 0x8;

struct alignas(4) ResourceEmitterSet {

    EmitterSetData *setData = nullptr; // ptr in binary file
//    EmitterTblData *tblData = nullptr; // ptr in binary file
    QVector<EmitterTblData> tblData;
    s32 numEmitter;
    u32 userData;
    EmitterSetData *setDataROM; // ptr in binary file
//    EmitterTblData *tblDataROM; // ptr in binary file
    QVector<EmitterTblData> tblDataROM;
    s32 numEmitterROM;
    u32 userDataROM;
    const char *setName = nullptr;
    bool isShowDetail;

    void printData() {

        qDebug() << " - setData:       ";
        setData->printData();

        qDebug() << " - tblData:       ";
        for (int i = 0; i < tblData.length(); i++) {
            qDebug() << "    - tbl" << i << ":";
            EmitterTblData tbl = tblData.at(i);
            tbl.printData();
        }

        qDebug() << " - numEmitter:    "            << numEmitter;
        qDebug() << " - userData:      "            << userData;
        qDebug() << " - setDataROM:    "            << setDataROM;
//        qDebug() << " - tblDataROM: "       << tblDataROM;
        qDebug() << " - tblDataROM:     OMITTED";
        qDebug() << " - numEmitterROM: "            << numEmitterROM;
        qDebug() << " - userDataROM:   "            << userDataROM;
        qDebug() << " - setName:       "            << setName;
        qDebug() << " - isShowDetail:  "            << isShowDetail;
        qDebug() << "\n";
    }
};

const u32 RES_EMITTER_SET_SIZE = 0x28;

#pragma pack(pop)

}

#endif //RESOURCE_H
