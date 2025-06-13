#include "ptcl/ptclBinary.h"
#include "ptcl/ptclEmitter.h"
#include "util/printUtil.h"


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


binMtx34f::binMtx34f() {}

binMtx34f::binMtx34f(const QMatrix3x4& mtx) {
    const f32* mtxData = mtx.data();
    for (int i = 0; i < 12; i++) {
        cells[i] = mtxData[i];
    }
}

QMatrix3x4 binMtx34f::toQMatrix3x4() const {
    const f32* srcData = cells.data();
    std::array<f32, 12> transposed;

    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 4; ++c) {
            transposed[c * 3 + r] = srcData[r * 4 + c];
        }
    }

    return QMatrix3x4(transposed.data());
}

QDataStream& operator>>(QDataStream& in, binMtx34f& item) {
    for (f32& val : item.cells) {
        in >> val;
    }
    return in;
}

QDataStream& operator<<(QDataStream& out, const binMtx34f& item) {
    for (const f32& val : item.cells) {
        out << val;
    }

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

binColor4f::binColor4f(f32 r, f32 g, f32 b, f32 a) :
    r(r), g(g), b(b), a(a) {
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
        >> item.minMipFilter;
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
        << item.minMipFilter;
    static const std::array<char, 3> padding = {0, 0, 0};
    out.writeRawData(padding.data(), 3);
    return out;
}

void BinTextureRes::printData(u32 indentationLevel) {
    const char* indentation = PrintUtil::createIndentation(indentationLevel);

    qDebug() << indentation << "- width:        " << width;
    qDebug() << indentation << "- height:       " << height;
    qDebug() << indentation << "- format:       " << format;
    qDebug() << indentation << "- wrapT:        " << wrapT;
    qDebug() << indentation << "- wrapS:        " << wrapS;
    qDebug() << indentation << "- magFilter:    " << magFilter;
    qDebug() << indentation << "- minMipFilter: " << minMipFilter;
}


// ========================================================================== //


BinCommonEmitterData::BinCommonEmitterData(const Ptcl::Emitter& emitter) {
    type = emitter.type();
    flag = emitter.flags();
    randomSeed = emitter.randomSeed().raw();
    namePos = 0; // To be assigned after construction...
    namePtr = 0;

    textureRes = {
        .width = static_cast<u16>(emitter.textureHandle()->textureData().width()),
        .height = static_cast<u16>(emitter.textureHandle()->textureData().height()),
        .format = emitter.textureHandle()->textureFormat(),
        .wrapT = emitter.textureWrapT(),
        .wrapS = emitter.textureWrapS(),
        .magFilter = emitter.textureMagFilter(),
        .minMipFilter = static_cast<u8>((static_cast<u8>(emitter.textureMinFilter()) & 0x1) | ((static_cast<u8>(emitter.textureMipFilter()) & 0x3) << 1)),
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
    volumeTblIndex = emitter.volumeTblIndex();
    _33 = emitter._33();
    volumeType = emitter.volumeType();
    volumeRadius = emitter.volumeRadius();
    volumeSweepStart = emitter.volumeSweepStart();
    volumeSweepParam = emitter.volumeSweepParam();
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
    color = emitter.colors();
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
    rotCombinerType = static_cast<u32>(emitter.rotType()) + 5 * static_cast<u32>(emitter.combinerType());
    followType = emitter.followType();
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
        >> item._31
        >> item.volumeTblIndex
        >> item._33
        >> item.volumeType
        >> item.volumeRadius
        >> item.volumeSweepStart
        >> item.volumeSweepParam
        >> item.figureVel
        >> item.emitterVelDir
        >> item._5C
        >> item.initVelRnd
        >> item.spreadVec;

    for (u32& val : item._70) {
        in >> val;
    }

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

    for (u32& val : item._D8) {
        in >> val;
    }

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
        >> item.rotCombinerType
        >> item.followType
        >> item._134
        >> item.initRot
        >> item.initRotRand
        >> item.rotVel
        >> item.rotVelRand;

    for (u32& val : item._168) {
        in >> val;
    }

    in >> item.transformSRT
        >> item.transformRT
        >> item.alphaAddInFade
        >> item.numTexPat
        >> item.numTexDivX
        >> item.numTexDivY
        >> item.texUVScale;

    for (u8& val : item.texPatTbl) {
        in >> val;
    }

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
        << item._31
        << item.volumeTblIndex
        << item._33
        << item.volumeType
        << item.volumeRadius
        << item.volumeSweepStart
        << item.volumeSweepParam
        << item.figureVel
        << item.emitterVelDir
        << item._5C
        << item.initVelRnd
        << item.spreadVec;

    for (const u32& val : item._70) {
        out << val;
    }

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

    for (const u32& val : item._D8) {
        out << val;
    }

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
        << item.rotCombinerType
        << item.followType
        << item._134
        << item.initRot
        << item.initRotRand
        << item.rotVel
        << item.rotVelRand;

    for (const u32& val : item._168) {
        out << val;
    }

    out << item.transformSRT
        << item.transformRT
        << item.alphaAddInFade
        << item.numTexPat
        << item.numTexDivX
        << item.numTexDivY
        << item.texUVScale;

    for (const u8& val : item.texPatTbl) {
        out << val;
    }

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
    qDebug() << indentation << "- volumeTblIndex:   " << volumeTblIndex;
    qDebug() << indentation << "- _33:              " << _33;
    qDebug() << indentation << "- volumeType:       " << volumeType;
    qDebug() << indentation << "- volumeRadius:     " << volumeRadius;
    qDebug() << indentation << "- volumeSweepStart: " << volumeSweepStart;
    qDebug() << indentation << "- volumeSweepParam: " << volumeSweepParam;
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
    qDebug() << indentation << "- rotCombinerType:  " << rotCombinerType;
    qDebug() << indentation << "- followType:       " << followType;
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
    childFlag = emitter.childFlags();
    fieldFlag = emitter.fieldFlags();
    fluctuationFlag = emitter.fluctuationFlags();
    stripeFlag = emitter.stripeFlags();

    childDataOffset = 0;
    fieldDataOffset = 0;
    fluctuationDataOffset = 0;
    stripeDataOffset = 0;
    mDataSize = 0;
}

QDataStream& operator>>(QDataStream& in, BinComplexEmitterData& item) {
    in >> item.childFlag
        >> item.fieldFlag
        >> item.fluctuationFlag
        >> item.stripeFlag
        >> item.childDataOffset
        >> item.fieldDataOffset
        >> item.fluctuationDataOffset
        >> item.stripeDataOffset
        >> item.mDataSize;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinComplexEmitterData& item) {
    out << static_cast<const BinCommonEmitterData&>(item);
    out << item.childFlag
        << item.fieldFlag
        << item.fluctuationFlag
        << item.stripeFlag
        << item.childDataOffset
        << item.fieldDataOffset
        << item.fluctuationDataOffset
        << item.stripeDataOffset
        << item.mDataSize;
    return out;
}

void BinComplexEmitterData::printData(u32 indentationLevel) {
    const char* indentation = PrintUtil::createIndentation(indentationLevel);

    qDebug() << indentation << "- childFlag:             " << childFlag;
    qDebug() << indentation << "- fieldFlag:             " << fieldFlag;
    qDebug() << indentation << "- fluctuationFlag:       " << fluctuationFlag;
    qDebug() << indentation << "- stripeFlag:            " << stripeFlag;
    qDebug() << indentation << "- childDataOffset:       " << childDataOffset;
    qDebug() << indentation << "- fieldDataOffset:       " << fieldDataOffset;
    qDebug() << indentation << "- fluctuationDataOffset: " << fluctuationDataOffset;
    qDebug() << indentation << "- stripeDataOffset:      " << stripeDataOffset;
    qDebug() << indentation << "- mDataSize:             " << mDataSize;
}


// ========================================================================== //


BinChildData::BinChildData(const Ptcl::ChildData& childData) {
    _0 = childData._0();
    _2C = childData._2C();
    _30 = childData._30();

    if (childData.textureHandle().isValid()) {
        textureRes = {
            .width = static_cast<u16>(childData.textureHandle()->textureData().width()),
            .height = static_cast<u16>(childData.textureHandle()->textureData().height()),
            .format = childData.textureHandle()->textureFormat(),
            .wrapT = childData.textureWrapT(),
            .wrapS = childData.textureWrapS(),
            .magFilter = childData.textureMagFilter(),
            .minMipFilter = static_cast<u8>((static_cast<u8>(childData.textureMinFilter()) & 0x1) | ((static_cast<u8>(childData.textureMipFilter()) & 0x3) << 1)),
        };
    } else {
        textureRes = {};
    }

    textureSize = 0; // To be assigned after construction...
    texturePos = 0; // To be assigned after construction...
    textureHandlePtr = 0;
    _4C = childData._4C();
    _5C = childData._5C();
    _60 = childData._60();
    _64 = childData._64();
    _68 = childData._68();
    _BC = childData._BC();
    _C8 = childData._C8();
    _E8 = childData._E8();
}


QDataStream& operator>>(QDataStream& in, BinChildData& item) {
    for (u8& val : item._0) {
        in >> val;
    }

    in >> item._2C;

    for (u8& val : item._30) {
        in >> val;
    }

    in >> item.textureRes
        >> item.textureSize
        >> item.texturePos
        >> item.textureHandlePtr;

    for (u8& val : item._4C) {
        in >> val;
    }

    in >> item._5C
        >> item._60
        >> item._64;

    for (u8& val : item._68) {
        in >> val;
    }

    in >> item._BC;

    for (u8& val : item._C8) {
        in >> val;
    }

    in >> item._E8;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinChildData& item) {
    for (const u8& val : item._0) {
        out << val;
    }

    out << item._2C;

    for (const u8& val : item._30) {
        out << val;
    }

    out << item.textureRes
        << item.textureSize
        << item.texturePos
        << item.textureHandlePtr;

    for (const u8& val : item._4C) {
        out << val;
    }

    out << item._5C
        << item._60
        << item._64;

    for (const u8& val : item._68) {
        out << val;
    }

    out << item._BC;

    for (const u8& val : item._C8) {
        out << val;
    }

    out << item._E8;
    return out;
}

void BinChildData::printData(u32 indentationLevel) {
    const char* indentation = PrintUtil::createIndentation(indentationLevel);

    qDebug() << indentation << "- _0:             " << _0.data();
    qDebug() << indentation << "- _2C:             " << _2C;
    qDebug() << indentation << "- _30:       " << _30.data();
    textureRes.printData(indentationLevel + 1);
    qDebug() << indentation << "- textureSize:       " << textureSize;
    qDebug() << indentation << "- texturePos:       " << texturePos;
    qDebug() << indentation << "- textureHandlePtr: " << textureHandlePtr;
    qDebug() << indentation << "- _4C:      " << _4C.data();
    qDebug() << indentation << "- _5C:             " << _5C;
    qDebug() << indentation << "- _60:             " << _60;
    qDebug() << indentation << "- _64:             " << _64;
    qDebug() << indentation << "- _68:             " << _68.data();
    qDebug() << indentation << "- _BC:             " << _BC;
    qDebug() << indentation << "- _C8:             " << _C8.data();
    qDebug() << indentation << "- _E8:             " << _E8;
}


// ========================================================================== //


BinFieldRandomData::BinFieldRandomData(const Ptcl::FieldRandomData& fieldRandomData) {
    fieldRandomBlank = fieldRandomData.randomBlank();
    fieldRandomVelAdd = fieldRandomData.randomVelAdd();
}

QDataStream& operator>>(QDataStream& in, BinFieldRandomData& item) {
    in >> item.fieldRandomBlank
        >> item.fieldRandomVelAdd;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinFieldRandomData& item) {
    out << item.fieldRandomBlank
        << item.fieldRandomVelAdd;
    return out;
}


// ========================================================================== //


BinFieldMagnetData::BinFieldMagnetData(const Ptcl::FieldMagnetData& fieldMagnetData) {
    fieldMagnetPower = fieldMagnetData.magnetPower();
    fieldMagnetPos = fieldMagnetData.magnetPos();
    fieldMagnetFlag = fieldMagnetData.magnetFlag();
}

QDataStream& operator>>(QDataStream& in, BinFieldMagnetData& item) {
    in >> item.fieldMagnetPower
        >> item.fieldMagnetPos
        >> item.fieldMagnetFlag;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinFieldMagnetData& item) {
    out << item.fieldMagnetPower
        << item.fieldMagnetPos
        << item.fieldMagnetFlag;
    return out;
}


// ========================================================================== //


BinFieldSpinData::BinFieldSpinData(const Ptcl::FieldSpinData& fieldSpinData) {
    fieldSpinRotate = fieldSpinData.spinRotate();
    fieldSpinAxis = fieldSpinData.spinAxis();
}

QDataStream& operator>>(QDataStream& in, BinFieldSpinData& item) {
    in >> item.fieldSpinRotate
        >> item.fieldSpinAxis;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinFieldSpinData& item) {
    out << item.fieldSpinRotate
        << item.fieldSpinAxis;
    return out;
}


// ========================================================================== //


BinFieldCollisionData::BinFieldCollisionData(const Ptcl::FieldCollisionData& fieldCollisionData) {
    fieldCollisionType = fieldCollisionData.collisionType();
    fieldCollisionIsWorld = fieldCollisionData.collisionIsWorld();
    fieldCollisionCoord = fieldCollisionData.collisionCoord();
    fieldCollisionCoef = fieldCollisionData.collisionCoef();
}

QDataStream& operator>>(QDataStream& in, BinFieldCollisionData& item) {
    in >> item.fieldCollisionType
        >> item.fieldCollisionIsWorld
        >> item.fieldCollisionCoord
        >> item.fieldCollisionCoef;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinFieldCollisionData& item) {
    out << item.fieldCollisionType
        << item.fieldCollisionIsWorld
        << item.fieldCollisionCoord
        << item.fieldCollisionCoef;
    return out;
}


// ========================================================================== //


BinFieldConvergenceData::BinFieldConvergenceData(const Ptcl::FieldConvergenceData& fieldConvergenceData) {
    fieldConvergenceType = fieldConvergenceData.convergenceType();
    fieldConvergencePos = fieldConvergenceData.convergencePos();
}

QDataStream& operator>>(QDataStream& in, BinFieldConvergenceData& item) {
    in >> item.fieldConvergenceType
        >> item.fieldConvergencePos;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinFieldConvergenceData& item) {
    out << item.fieldConvergenceType
        << item.fieldConvergencePos;
    return out;
}


// ========================================================================== //


BinFieldPosAddData::BinFieldPosAddData(const Ptcl::FieldPosAddData& fieldPosAddData) {
    fieldPosAdd = fieldPosAddData.posAdd();
}

QDataStream& operator>>(QDataStream& in, BinFieldPosAddData& item) {
    in >> item.fieldPosAdd;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinFieldPosAddData& item) {
    out << item.fieldPosAdd;
    return out;
}


// ========================================================================== //


BinFluctuationData::BinFluctuationData(const Ptcl::FluctuationData& fluctuationData) {
    fluctuationScale = fluctuationData.scale();
    fluctuationFreq = fluctuationData.freq();
    fluctuationPhaseRnd = fluctuationData.phaseRnd();
}

QDataStream& operator>>(QDataStream& in, BinFluctuationData& item) {
    in >> item.fluctuationScale
        >> item.fluctuationFreq
        >> item.fluctuationPhaseRnd;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinFluctuationData& item) {
    out << item.fluctuationScale
        << item.fluctuationFreq
        << item.fluctuationPhaseRnd;
    return out;
}


// ========================================================================== //


BinStripeData::BinStripeData(const Ptcl::StripeData& stripeData) {
    _0 = stripeData._0();
}

QDataStream& operator>>(QDataStream& in, BinStripeData& item) {
    for (u8& val : item._0) {
        in >> val;
    }

    return in;
}

QDataStream& operator<<(QDataStream& out, const BinStripeData& item) {
    for (const u8& val : item._0) {
        out << val;
    }

    return out;
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
