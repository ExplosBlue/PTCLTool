#ifndef RESOURCE_H
#define RESOURCE_H

#include "typedefs.h"

#include <QDebug>
#include <QString>
#include <QDataStream>


namespace ptcl {

#pragma pack(push, 4)

struct HeaderData {

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

struct EmitterSetData
{
    u32 userData;
    u32 lastUpdateDate;
    s32 namePos;
    const char *name = nullptr;
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

        qDebug() << "EmitterSetData:";
        qDebug() << " - userData:"        << Qt::hex << Qt::showbase << userData;
        qDebug() << " - lastUpdateDate:"  << Qt::hex << Qt::showbase << lastUpdateDate;
        qDebug() << " - namePos:"         << Qt::hex << Qt::showbase << namePos;
        qDebug() << " - name:"            << Qt::hex << Qt::showbase << name;
        qDebug() << " - numEmitter:"      << Qt::hex << Qt::showbase << numEmitter;
        qDebug() << " - emitterTblPos:"   << Qt::hex << Qt::showbase << emitterTblPos;
        qDebug() << " - emitterTbl:"      << Qt::hex << Qt::showbase << emitterTbl;
    }

};

const u32 EMITTER_SET_DATA_SIZE = 0x1C;

struct EmitterTblData {

    s32 emitterOffset;
    void *emitter;

    friend QDataStream &operator>>(QDataStream &in, EmitterTblData &item) {

        in >> item.emitterOffset;
        in.skipRawData(4); // emitter

        return in;
    }

    friend QDataStream &operator<<(QDataStream &out, const EmitterTblData &item) {

        out << item.emitterOffset;
        out.writeBytes(0, 4); // emitter

        return out;
    }

    void printData() {

        qDebug() << "EmitterTblData:";
        qDebug() << " - emitterOffset: "    <<  emitterOffset;
        qDebug() << " - emitter: "          << emitter;
    }

};

const u32 EMITTER_TBL_DATA_SIZE = 0x8;

struct ResourceEmitterSet {

    EmitterSetData *setData = nullptr;
    EmitterTblData *tblData = nullptr;
    s32 numEmitter;
    u32 userData;
    void *setDataROM = nullptr;
    void *tblDataROM = nullptr;
    s32 numEmitterROM;
    u32 userDataROM;
    const char *setName = nullptr;
    bool isShowDetail;

    friend QDataStream &operator>>(QDataStream &in, ResourceEmitterSet &item) {

        in.skipRawData(4); // setData
        in.skipRawData(4); // tblData
        in >> item.numEmitter
           >> item.userData;
        in.skipRawData(4); // setDataROM
        in.skipRawData(4); // tblDataROM
        in >> item.numEmitterROM
           >> item.userDataROM;
        in.skipRawData(4); // setName
        in >> item.isShowDetail;

        item.userDataROM = item.userData;
        item.setDataROM = item.setData;
        item.tblDataROM = item.tblData;
        item.numEmitterROM = item.numEmitter;

        return in;
    }

    friend QDataStream &operator<<(QDataStream &out, const ResourceEmitterSet &item) {

        out.writeBytes(0, 4); // setData
        out.writeBytes(0, 4); // tblData
        out << item.numEmitter
            << item.userData;
        out.writeBytes(0, 4); // setDataROM
        out.writeBytes(0, 4); // tblDataROM
        out << item.numEmitterROM
            << item.userDataROM;
        out.writeBytes(0, 4); // setName
        out << item.isShowDetail;

        return out;
    }

    void printData() {

        qDebug() << "ResourceEmitterSet:";
        qDebug() << " - setData: "          <<  setData;
        qDebug() << " - tblData: "          << tblData;
        qDebug() << " - numEmitter: "       << numEmitter;
        qDebug() << " - userData: "         << userData;
        qDebug() << " - setDataROM: "       << setDataROM;
        qDebug() << " - tblDataROM: "       << tblDataROM;
        qDebug() << " - numEmitterROM: "    << numEmitterROM;
        qDebug() << " - userDataROM: "      << userDataROM;
        qDebug() << " - setName: "          << setName;
        qDebug() << " - isShowDetail: "     << isShowDetail;
    }
};

const u32 RES_EMITTER_SET_SIZE = 0x28;

#pragma pack(pop)

}

#endif //RESOURCE_H
