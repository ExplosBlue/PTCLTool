#include "ptcl/ptclRes.h"

#include "printUtil.h"
#include "ptcl/ptcl.h"



namespace Ptcl {

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


BinHeaderData::BinHeaderData() {}

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

    out << item.magic.data()
    << item.version
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


BinEmitterSetData::BinEmitterSetData() {
}

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


BinTextureRes::BinTextureRes() {
}

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
    out.writeRawData(nullptr, 3);
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


BinCommonEmitterData::BinCommonEmitterData() {

}


BinCommonEmitterData::BinCommonEmitterData(const Ptcl::Emitter& emitter) {

    type = emitter.type();
    flag = emitter.flag();
    randomSeed = emitter.randomSeed();
    namePos = 0; // To be assigned after construction...
    namePtr = 0;
    textureRes.width = emitter.textureHandle()->textureData().width();
    textureRes.height = emitter.textureHandle()->textureData().height();
    textureRes.format = emitter.textureHandle()->textureFormat();
    textureRes.wrapT = emitter.textureWrapT();
    textureRes.wrapS = emitter.textureWrapS();
    textureRes.magFilter = emitter.textureMagFilter();
    textureRes.minFilter = emitter.textureMinFilter();

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
    _4C = emitter._4C();
    std::copy(emitter._50().begin(), emitter._50().end(), _50);
    ptclLife = emitter.ptclLife();
    _88 = emitter._88();
    _8C = emitter._8C();
    std::copy(emitter._90().begin(), emitter._90().end(), _90);
    _9C = emitter._9C();
    _A0 = emitter._A0();
    _A4 = emitter._A4();

    u32 idx = 0;
    for (auto& eColor : emitter.colors()) {
        color[idx] = eColor;
        ++idx;
    }

    std::copy(emitter._D8().begin(), emitter._D8().end(), _D8);

    colorSection1 = emitter.colorSection1();
    colorSection2 = emitter.colorSection2();
    colorSection3 = emitter.colorSection3();
    colorNumRepeat = emitter.colorNumRepeat();
    initAlpha = emitter.initAlpha();
    std::copy(emitter._F8().begin(), emitter._F8().end(), _F8);
    initScale = emitter.initScale();
    std::copy(emitter._110().begin(), emitter._110().end(), _110);
    _170 = emitter._170();
    _1A0 = emitter._1A0();
    std::copy(emitter._1D0().begin(), emitter._1D0().end(), _1D0);
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


// ========================================================================== //


BinComplexEmitterData::BinComplexEmitterData() {
}

QDataStream& operator>>(QDataStream& in, BinComplexEmitterData& item) {

    in >> item._0
        >> item._4
        >> item._8
        >> item._C
        >> item._10
        >> item._14
        >> item._18
        >> item._1C
        >> item._20;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinComplexEmitterData& item) {

    out << item._0
        << item._4
        << item._8
        << item._C
        << item._10
        << item._14
        << item._18
        << item._1C
        << item._20;
    return out;
}

void BinComplexEmitterData::printData(u32 indentationLevel) {

    const char* indentation = PrintUtil::createIndentation(indentationLevel);

    qDebug() << indentation << "- _0:  " << _0;
    qDebug() << indentation << "- _4:  " << _4;
    qDebug() << indentation << "- _8:  " << _8;
    qDebug() << indentation << "- _C:  " << _C;
    qDebug() << indentation << "- _10: " << _10;
    qDebug() << indentation << "- _14: " << _14;
    qDebug() << indentation << "- _18: " << _18;
    qDebug() << indentation << "- _1C: " << _1C;
    qDebug() << indentation << "- _20: " << _20;
}


// ========================================================================== //


BinEmitterDataType2::BinEmitterDataType2() {
}

QDataStream& operator>>(QDataStream& in, BinEmitterDataType2& item) {

    in >> item._0
        >> item._4
        >> item._8
        >> item._C
        >> item._10;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinEmitterDataType2& item) {

    out << item._0
        << item._4
        << item._8
        << item._C
        << item._10;
    return out;
}

void BinEmitterDataType2::printData(u32 indentationLevel) {

    const char* indentation = PrintUtil::createIndentation(indentationLevel);

    qDebug() << indentation << "- _0:  " << _0;
    qDebug() << indentation << "- _4:  " << _4;
    qDebug() << indentation << "- _8:  " << _8;
    qDebug() << indentation << "- _C:  " << _C;
    qDebug() << indentation << "- _10: " << _10;
}


// ========================================================================== //


BinEmitterTblData::BinEmitterTblData() {
}

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
