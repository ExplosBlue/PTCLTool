#include "ptcl/ptclBinary.h"

#include "printUtil.h"
#include "ptcl/ptclEmitter.h"


namespace Ptcl {


// ========================================================================== //


binVec2f::binVec2f() :
    x(1.0f), y(1.0f) {
}

binVec2f::binVec2f(const QVector2D& vec) :
    x(vec.x()), y(vec.y()) {
}

QDataStream& operator>>(QDataStream& in, binVec2f& item) {

    in >> item.x >> item.y;
    return in;
}

QDataStream& operator<<(QDataStream& out, const binVec2f& item) {

    out << item.x << item.y;
    return out;
}

QDebug operator<<(QDebug dbg, const binVec2f& item) {

    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << "{" << item.x << "," << item.y << "}";
    return dbg;
}


// ========================================================================== //


binVec3f::binVec3f() :
    x(1.0f), y(1.0f), z(1.0f) {
}

binVec3f::binVec3f(const QVector3D& vec) :
    x(vec.x()), y(vec.y()), z(vec.z()) {
}

QDataStream& operator>>(QDataStream& in, binVec3f& item) {

    in >> item.x >> item.y >> item.z;
    return in;
}

QDataStream& operator<<(QDataStream& out, const binVec3f& item) {

    out << item.x << item.y << item.z;
    return out;
}

QDebug operator<<(QDebug dbg, const binVec3f& item) {

    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << "{" << item.x << "," << item.y << "," << item.z << "}";
    return dbg;
}


// ========================================================================== //


binMtx34f::binMtx34f() {
}

binMtx34f::binMtx34f(const QMatrix3x4& mtx) {

    const f32* mtxData = mtx.data();
    for (int i = 0; i < 12; i++) {
        cells[i] = mtxData[i];
    }
}

QDataStream& operator>>(QDataStream& in, binMtx34f& item) {

    in.readRawData(reinterpret_cast<char*>(item.cells.data()), sizeof(binMtx34f));
    return in;
}

QDataStream& operator<<(QDataStream& out, const binMtx34f& item) {

    out.writeRawData(reinterpret_cast<const char*>(item.cells.data()), sizeof(binMtx34f));
    return out;
}

QDebug operator<<(QDebug dbg, const binMtx34f& item) {

    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << "{{" << item.rows[0][0] << "," << item.rows[0][1] << "," << item.rows[0][2] << "," << item.rows[0][3] << "} \n"
                  << " {" << item.rows[1][0] << "," << item.rows[1][1] << "," << item.rows[1][2] << "," << item.rows[1][3] << "} \n"
                  << " {" << item.rows[2][0] << "," << item.rows[2][1] << "," << item.rows[2][2] << "," << item.rows[2][3] << "}}";
    return dbg;
}


// ========================================================================== //


binColor4f::binColor4f() :
    r(0.0f), g(0.0f), b(0.0f), a(0.0f) {
}

binColor4f::binColor4f(const QColor& color) :
    r(color.redF()), g(color.greenF()), b(color.blueF()), a(color.alphaF()) {
}

QDataStream& operator>>(QDataStream& in, binColor4f& item) {

    in >> item.r >> item.g >> item.b >> item.a;
    return in;
}

QDataStream& operator<<(QDataStream& out, const binColor4f& item) {

    out << item.r << item.g << item.b << item.a;
    return out;
}

QDebug operator<<(QDebug dbg, const binColor4f& item) {

    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << "{" << item.r << "," << item.g << "," << item.b << "," << item.a << "}";
    return dbg;
}


// ========================================================================== //


// BinHeaderData::BinHeaderData() {}

QDataStream& operator>>(QDataStream& in, BinHeaderData& item) {

    in.readRawData(item.magic.data(), 4);
    in >> item.version
        >> item.numEmitterSet
        >> item.namePos
        >> item.nameTblPos
        >> item.textureTblPos
        >> item.textureTblSize;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinHeaderData& item) {

    out.writeRawData(item.magic.data(), 4);
    out << item.version
        << item.numEmitterSet
        << item.namePos
        << item.nameTblPos
        << item.textureTblPos
        << item.textureTblSize;
    return out;
}

void BinHeaderData::printData(u32 indentationLevel) {

    const char* indentation = PrintUtil::createIndentation(indentationLevel);

    qDebug() << indentation << "- magic:"          << QString::fromUtf8(magic);
    qDebug() << indentation << "- version:"        << Qt::hex << Qt::showbase << version;
    qDebug() << indentation << "- numEmitterSet:"  << Qt::hex << Qt::showbase << numEmitterSet;
    qDebug() << indentation << "- namePos:"        << Qt::hex << Qt::showbase << namePos;
    qDebug() << indentation << "- nameTblPos:"     << Qt::hex << Qt::showbase << nameTblPos;
    qDebug() << indentation << "- textureTblPos:"  << Qt::hex << Qt::showbase << textureTblPos;
    qDebug() << indentation << "- textureTblSize:" << Qt::hex << Qt::showbase << textureTblSize;
}


// ========================================================================== //


QDataStream &operator>>(QDataStream& in, BinEmitterSetData& item) {

    in >> item.userData
        >> item.lastUpdateDate
        >> item.namePos
        >> item.namePtr
        >> item.numEmitter
        >> item.emitterTblPos
        >> item.emitterTbl;
    return in;
}

QDataStream &operator<<(QDataStream& out, const BinEmitterSetData& item) {

    out << item.userData
        << item.lastUpdateDate
        << item.namePos
        << item.namePtr
        << item.numEmitter
        << item.emitterTblPos
        << item.emitterTbl;
    return out;
}

void BinEmitterSetData::printData(u32 indentationLevel) {

    const char* indentation = PrintUtil::createIndentation(indentationLevel);

    qDebug() << indentation << "- userData:       " << Qt::hex << Qt::showbase << userData;
    qDebug() << indentation << "- lastUpdateDate: " << Qt::hex << Qt::showbase << lastUpdateDate;
    qDebug() << indentation << "- namePos:        " << Qt::hex << Qt::showbase << namePos;
    qDebug() << indentation << "- name:           " << Qt::hex << Qt::showbase << namePtr;
    qDebug() << indentation << "- numEmitter:     " << Qt::hex << Qt::showbase << numEmitter;
    qDebug() << indentation << "- emitterTblPos:  " << Qt::hex << Qt::showbase << emitterTblPos;
    qDebug() << indentation << "- emitterTbl:     " << Qt::hex << Qt::showbase << emitterTbl;
}


// ========================================================================== //


QDataStream& operator>>(QDataStream& in, BinTextureRes& item) {

    in >> item.width
        >> item.height
        >> item.format
        >> item.wrapT
        >> item.wrapS
        >> item.magFilter
        >> item.minFilter;
    in.readRawData(nullptr, 3);
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinTextureRes& item) {

    out << item.width
        << item.height
        << item.format
        << item.wrapT
        << item.wrapS
        << item.magFilter
        << item.minFilter;

    static const std::array<char, 3> padding = {0, 0, 0};
    out.writeRawData(padding.data(), 3);

    return out;
}

void BinTextureRes::printData(u32 indentationLevel) {

    const char* indentation = PrintUtil::createIndentation(indentationLevel);

    qDebug() << indentation << "- width:     " << width;
    qDebug() << indentation << "- height:    " << height;
    qDebug() << indentation << "- format:    " << format;
    qDebug() << indentation << "- wrapT:     " << wrapT;
    qDebug() << indentation << "- wrapS:     " << wrapS;
    qDebug() << indentation << "- magFilter: " << magFilter;
    qDebug() << indentation << "- minFilter: " << minFilter;
}


// ========================================================================== //


BinCommonEmitterData::BinCommonEmitterData(const Ptcl::Emitter& emitter) {

    type = emitter.type();
    flag = emitter.flag();
    randomSeed = emitter.randomSeed();
    namePos = 0; // To be assigned after construction...
    namePtr = 0;

    textureRes = {
        .width = static_cast<u16>(emitter.textureHandle()->textureData().width()),
        .height = static_cast<u16>(emitter.textureHandle()->textureData().height()),
        .format = emitter.textureHandle()->textureFormat(),
        .wrapT = emitter.textureWrapT(),
        .wrapS = emitter.textureWrapS(),
        .magFilter = emitter.textureMagFilter(),
        .minFilter = emitter.textureMinFilter(),
    };

    textureSize = 0; // To be assigned after construction...
    texturePos = 0; // To be assigned after construction...
    textureHandlePtr = 0;
    _2C = emitter._2C();
    _2D = emitter._2D();
    _2E = emitter._2E();
    _2F = emitter._2F();
    _30 = emitter._30();
    _31 = emitter._31();
    _32 = emitter._32();
    volumeType = emitter.volumeType();
    volumeRadius = emitter.volumeRadius();
    _44 = emitter._44();
    _48 = emitter._48();
    figureVel = emitter.figureVel();
    emitterVelDir = emitter.emitterVelDir();
    _5C = emitter._5C();
    initVelRnd = emitter.initVelRnd();
    spreadVec = emitter.spreadVec();
    std::copy(emitter._70().begin(), emitter._70().end(), _70.data());
    _80 = emitter._80();
    ptclLife = emitter.ptclLife();
    ptclLifeRnd = emitter.ptclLifeRnd();
    _8C = emitter._8C();
    _90 = emitter._90();
    billboardType = emitter.billboardType();
    _98 = emitter._98();
    _9C = emitter._9C();
    _A0 = emitter._A0();
    _A4 = emitter._A4();

    u32 idx = 0;
    for (auto& eColor : emitter.colors()) {
        color[idx] = eColor;
        ++idx;
    }

    std::copy(emitter._D8().begin(), emitter._D8().end(), _D8.data());

    colorSection1 = emitter.colorSection1();
    colorSection2 = emitter.colorSection2();
    colorSection3 = emitter.colorSection3();
    colorNumRepeat = emitter.colorNumRepeat();
    initAlpha = emitter.initAlpha();
    diffAlpha21 = emitter.diffAlpha21();
    diffAlpha32 = emitter.diffAlpha32();
    alphaSection1 = emitter.alphaSection1();
    alphaSection2 = emitter.alphaSection2();
    initScale = emitter.initScale();
    diffScale21 = emitter.diffScale21();
    diffScale32 = emitter.diffScale32();
    scaleSection1 = emitter.scaleSection1();
    scaleSection2 = emitter.scaleSection2();
    scaleRand = emitter.scaleRand();
    _12C = emitter._12C();
    _130 = emitter._130();
    _134 = emitter._134();
    initRot = emitter.initRot();
    initRotRand = emitter.initRotRand();
    rotVel = emitter.rotVel();
    rotVelRand = emitter.rotVelRand();
    std::copy(emitter._168().begin(), emitter._168().end(), _168.data());
    transformSRT = emitter.transformSRT();
    transformRT = emitter.transformRT();
    alphaAddInFade = emitter.alphaAddInFade();
    numTexPat = emitter.numTexPat();
    numTexDivX = emitter.numTexDivX();
    numTexDivY = emitter.numTexDivY();
    texUVScale = emitter.texUVScale();
    // texPatTbl = emitter.texPatTbl();
    std::copy(emitter.texPatTbl().begin(), emitter.texPatTbl().end(), texPatTbl.data());
    texPatFreq = emitter.texPatFreq();
    texPatTblUse = emitter.texPatTblUse();
}

QDataStream& operator>>(QDataStream& in, BinCommonEmitterData& item) {

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
        >> item._31;
    in.readRawData(reinterpret_cast<char*>(item._32.data()), sizeof(item._32));
    in >> item.volumeType
        >> item.volumeRadius
        >> item._44
        >> item._48
        >> item.figureVel
        >> item.emitterVelDir
        >> item._5C
        >> item.initVelRnd
        >> item.spreadVec;
    in.readRawData(reinterpret_cast<char*>(item._70.data()), sizeof(item._70));
    in >> item._80
        >> item.ptclLife
        >> item.ptclLifeRnd
        >> item._8C
        >> item._90
        >> item.billboardType
        >> item._98
        >> item._9C
        >> item._A0
        >> item._A4
        >> item.color[0]
        >> item.color[1]
        >> item.color[2];
    in.readRawData(reinterpret_cast<char*>(item._D8.data()), sizeof(item._D8));
    in >> item.colorSection1
        >> item.colorSection2
        >> item.colorSection3
        >> item.colorNumRepeat
        >> item.initAlpha
        >> item.diffAlpha21
        >> item.diffAlpha32
        >> item.alphaSection1
        >> item.alphaSection2
        >> item.initScale
        >> item.diffScale21
        >> item.diffScale32
        >> item.scaleSection1
        >> item.scaleSection2
        >> item.scaleRand
        >> item._12C
        >> item._130
        >> item._134
        >> item.initRot
        >> item.initRotRand
        >> item.rotVel
        >> item.rotVelRand;
    in.readRawData(reinterpret_cast<char*>(item._168.data()), sizeof(item._168));
    in >> item.transformSRT
        >> item.transformRT
        >> item.alphaAddInFade
        >> item.numTexPat
        >> item.numTexDivX
        >> item.numTexDivY
        >> item.texUVScale;
    in.readRawData(reinterpret_cast<char*>(item.texPatTbl.data()), sizeof(item.texPatTbl));
    in >> item.texPatFreq
        >> item.texPatTblUse;

    return in;
}

QDataStream& operator<<(QDataStream& out, const BinCommonEmitterData& item) {

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
        << item._31;
    out.writeRawData(reinterpret_cast<const char*>(item._32.data()), sizeof(item._32));
    out << item.volumeType
        << item.volumeRadius
        << item._44
        << item._48
        << item.figureVel
        << item.emitterVelDir
        << item._5C
        << item.initVelRnd
        << item.spreadVec;
    out.writeRawData(reinterpret_cast<const char*>(item._70.data()), sizeof(item._70));
    out << item._80
        << item.ptclLife
        << item.ptclLifeRnd
        << item._8C
        << item._90
        << item.billboardType
        << item._98
        << item._9C
        << item._A0
        << item._A4
        << item.color[0]
        << item.color[1]
        << item.color[2];
    out.writeRawData(reinterpret_cast<const char*>(item._D8.data()), sizeof(item._D8));
    out << item.colorSection1
        << item.colorSection2
        << item.colorSection3
        << item.colorNumRepeat
        << item.initAlpha
        << item.diffAlpha21
        << item.diffAlpha32
        << item.alphaSection1
        << item.alphaSection2
        << item.initScale
        << item.diffScale21
        << item.diffScale32
        << item.scaleSection1
        << item.scaleSection2
        << item.scaleRand
        << item._12C
        << item._130
        << item._134
        << item.initRot
        << item.initRotRand
        << item.rotVel
        << item.rotVelRand;
    out.writeRawData(reinterpret_cast<const char*>(item._168.data()), sizeof(item._168));
    out << item.transformSRT
        << item.transformRT
        << item.alphaAddInFade
        << item.numTexPat
        << item.numTexDivX
        << item.numTexDivY
        << item.texUVScale;
    out.writeRawData(reinterpret_cast<const char*>(item.texPatTbl.data()), sizeof(item.texPatTbl));
    out << item.texPatFreq
        << item.texPatTblUse;

    return out;
}

void BinCommonEmitterData::printData(u32 indentationLevel) {

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
    qDebug() << indentation << "- _32:              " << _32.data();
    qDebug() << indentation << "- volumeType:       " << volumeType;
    qDebug() << indentation << "- volumeRadius:     " << volumeRadius;
    qDebug() << indentation << "- _44:              " << _44;
    qDebug() << indentation << "- _48:              " << _48;
    qDebug() << indentation << "- figureVel:        " << figureVel;
    qDebug() << indentation << "- emitterVelDir:    " << emitterVelDir;
    qDebug() << indentation << "- _5C:              " << _5C;
    qDebug() << indentation << "- initVelRnd:       " << initVelRnd;
    qDebug() << indentation << "- spreadVec:        " << spreadVec;
    qDebug() << indentation << "- _70:              " << _70.data();
    qDebug() << indentation << "- _80:              " << _80;
    qDebug() << indentation << "- ptclLife:         " << ptclLife;
    qDebug() << indentation << "- ptclLifeRnd:      " << ptclLifeRnd;
    qDebug() << indentation << "- _8C:              " << _8C;
    qDebug() << indentation << "- _90:              " << _90;
    qDebug() << indentation << "- billboardType:    " << billboardType;
    qDebug() << indentation << "- _98:              " << _98;
    qDebug() << indentation << "- _9C:              " << _9C;
    qDebug() << indentation << "- _A0:              " << _A0;
    qDebug() << indentation << "- _A4:              " << _A4;
    qDebug() << indentation << "- color:            " << color.data();
    qDebug() << indentation << "- _D8:              " << _D8.data();
    qDebug() << indentation << "- colorSection1:    " << colorSection1;
    qDebug() << indentation << "- colorSection2:    " << colorSection2;
    qDebug() << indentation << "- colorSection3:    " << colorSection3;
    qDebug() << indentation << "- colorNumRepeat:   " << colorNumRepeat;
    qDebug() << indentation << "- initAlpha:        " << initAlpha;
    qDebug() << indentation << "- diffAlpha21:      " << diffAlpha21;
    qDebug() << indentation << "- diffAlpha32:      " << diffAlpha32;
    qDebug() << indentation << "- alphaSection1:    " << alphaSection1;
    qDebug() << indentation << "- alphaSection2:    " << alphaSection2;
    qDebug() << indentation << "- initScale:        " << initScale;
    qDebug() << indentation << "- diffScale21:      " << diffScale21;
    qDebug() << indentation << "- diffScale32:      " << diffScale32;
    qDebug() << indentation << "- scaleSection1:    " << scaleSection1;
    qDebug() << indentation << "- scaleSection2:    " << scaleSection2;
    qDebug() << indentation << "- scaleRand:        " << scaleRand;
    qDebug() << indentation << "- _12C:             " << _12C;
    qDebug() << indentation << "- _130:             " << _130;
    qDebug() << indentation << "- _134:             " << _134;
    qDebug() << indentation << "- initRot:          " << initRot;
    qDebug() << indentation << "- initRotRand:      " << initRotRand;
    qDebug() << indentation << "- rotVel:           " << rotVel;
    qDebug() << indentation << "- rotVelRand:       " << rotVelRand;
    qDebug() << indentation << "- _168:             " << _168.data();
    qDebug() << indentation << "- transformSRT:     " << transformSRT;
    qDebug() << indentation << "- transformRT:      " << transformRT;
    qDebug() << indentation << "- alphaAddInFade:   " << alphaAddInFade;
    qDebug() << indentation << "- numTexPat:        " << numTexPat;
    qDebug() << indentation << "- numTexDivX:       " << numTexDivX;
    qDebug() << indentation << "- numTexDivY:       " << numTexDivY;
    qDebug() << indentation << "- texUVScale:       " << texUVScale;
    qDebug() << indentation << "- texPatTbl:        " << texPatTbl.data();
    qDebug() << indentation << "- texPatFreq:       " << texPatFreq;
    qDebug() << indentation << "- texPatTblUse:     " << texPatTblUse;
}


// ========================================================================== //


BinComplexEmitterData::BinComplexEmitterData(const Ptcl::Emitter& emitter) :
    BinCommonEmitterData{emitter} {

    _1F4 = emitter.complexEmitterData()._1F4;
    _1F8 = emitter.complexEmitterData()._1F8;
    _1FC = emitter.complexEmitterData()._1FC;
    _200 = emitter.complexEmitterData()._200;
    mDataSize = emitter.complexEmitterData().mDataSize;
}

QDataStream& operator>>(QDataStream& in, BinComplexEmitterData& item) {

    in >> item._1F4
        >> item._1F8
        >> item._1FC
        >> item._200
        >> item.mDataSize;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinComplexEmitterData& item) {

    out << static_cast<const BinCommonEmitterData&>(item);

    out << item._1F4
        << item._1F8
        << item._1FC
        << item._200
        << item.mDataSize;

    return out;
}

void BinComplexEmitterData::printData(u32 indentationLevel) {

    const char* indentation = PrintUtil::createIndentation(indentationLevel);

    qDebug() << indentation << "- _1F4:      " << _1F4;
    qDebug() << indentation << "- _1F8:      " << _1F8;
    qDebug() << indentation << "- _1FC:      " << _1FC;
    qDebug() << indentation << "- _200:      " << _200;
    qDebug() << indentation << "- mDataSize: " << mDataSize;
}


// ========================================================================== //


BinEmitterDataType2::BinEmitterDataType2(const Ptcl::Emitter& emitter) :
    BinCommonEmitterData{emitter} {

    _0 = emitter.emitterDataType2()._0;
    _4 = emitter.emitterDataType2()._4;
    _8 = emitter.emitterDataType2()._8;
    _C = emitter.emitterDataType2()._C;
    mDataSize = emitter.emitterDataType2().mDataSize;
}

QDataStream& operator>>(QDataStream& in, BinEmitterDataType2& item) {

    in >> item._0
        >> item._4
        >> item._8
        >> item._C
        >> item.mDataSize;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinEmitterDataType2& item) {

    out << static_cast<const BinCommonEmitterData&>(item);

    out << item._0
        << item._4
        << item._8
        << item._C
        << item.mDataSize;
    return out;
}

void BinEmitterDataType2::printData(u32 indentationLevel) {

    const char* indentation = PrintUtil::createIndentation(indentationLevel);

    qDebug() << indentation << "- _0:        " << _0;
    qDebug() << indentation << "- _4:        " << _4;
    qDebug() << indentation << "- _8:        " << _8;
    qDebug() << indentation << "- _C:        " << _C;
    qDebug() << indentation << "- mDataSize: " << mDataSize;
}


// ========================================================================== //


QDataStream& operator>>(QDataStream& in, BinEmitterTblData& item) {

    in >> item.emitterPos
        >> item.emitterPtr;

    return in;
}

QDataStream& operator<<(QDataStream& out, const BinEmitterTblData& item) {

    out << item.emitterPos
        << item.emitterPtr;

    return out;
}

void BinEmitterTblData::printData(u32 indentationLevel) {

    const char* indentation = PrintUtil::createIndentation(indentationLevel);

    qDebug() << indentation << "- emitterPos: " << emitterPos;
    qDebug() << indentation << "- emitterPtr: " << emitterPtr;
}


// ========================================================================== //


} // namespace Ptcl
