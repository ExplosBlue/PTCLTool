#include "ptcl/ptclBinary.h"
#include "ptcl/ptclEmitter.h"
#include "util/printUtil.h"
#include "ptcl/ptclFieldData.h"


namespace Ptcl {


// ========================================================================== //


binVec2f::binVec2f() :
    x(1.0f), y(1.0f) {
}

binVec2f::binVec2f(const Math::Vector2f& vec) :
    x(vec.getX()), y(vec.getY()) {
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

binVec3f::binVec3f(const Math::Vector3f& vec) :
    x(vec.getX()), y(vec.getY()), z(vec.getZ()) {
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


binVec3i::binVec3i() :
    x(1), y(1), z(1) {
}

binVec3i::binVec3i(const Math::Vector3i& vec) :
    x(vec.getX()), y(vec.getY()), z(vec.getZ()) {
}

QDataStream& operator>>(QDataStream& in, binVec3i& item) {
    in >> item.x >> item.y >> item.z;
    return in;
}

QDataStream& operator<<(QDataStream& out, const binVec3i& item) {
    out << item.x << item.y << item.z;
    return out;
}

QDebug operator<<(QDebug dbg, const binVec3i& item) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << "{" << item.x << "," << item.y << "," << item.z << "}";
    return dbg;
}


// ========================================================================== //


binMtx34f::binMtx34f() {}

binMtx34f::binMtx34f(const Math::Matrix34f& mtx) {
    std::copy(mtx.data(), mtx.data() + 12, cells.begin());
}

Math::Matrix34f binMtx34f::toMatrix34f() const {
    return Math::Matrix34f{ cells.data() };
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


binColor3f::binColor3f() :
    r(0.0f), g(0.0f), b(0.0f) {
}

binColor3f::binColor3f(f32 r, f32 g, f32 b) :
    r(r), g(g), b(b) {
}

binColor3f::binColor3f(const QColor& color) :
    r(color.redF()), g(color.greenF()), b(color.blueF()) {
}

QDataStream& operator>>(QDataStream& in, binColor3f& item) {
    in >> item.r >> item.g >> item.b;
    return in;
}

QDataStream& operator<<(QDataStream& out, const binColor3f& item) {
    out << item.r << item.g << item.b;
    return out;
}

QDebug operator<<(QDebug dbg, const binColor3f& item) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << "{" << item.r << "," << item.g << "," << item.b << "}";
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
    type = emitter.basicProperties().type;
    flag = emitter.flags();
    randomSeed = emitter.basicProperties().randomSeed.raw();
    namePos = 0; // To be assigned after construction...
    namePtr = 0;

    textureRes = {
        .width = static_cast<u16>(emitter.textureHandle()->textureData().width()),
        .height = static_cast<u16>(emitter.textureHandle()->textureData().height()),
        .format = emitter.textureHandle()->textureFormat(),
        .wrapT = emitter.textureProperties().textureWrapT,
        .wrapS = emitter.textureProperties().textureWrapS,
        .magFilter = emitter.textureProperties().textureMagFilter,
        .minMipFilter = static_cast<u8>((static_cast<u8>(emitter.textureProperties().textureMinFilter) & 0x1) | ((static_cast<u8>(emitter.textureProperties().textureMipFilter) & 0x3) << 1)),
    };

    textureSize = 0; // To be assigned after construction...
    texturePos = 0; // To be assigned after construction...
    textureHandlePtr = 0;
    isPolygon = emitter.basicProperties().isPolygon;
    isFollow = emitter.basicProperties().isFollow;
    isEmitterBillboardMtx = emitter.basicProperties().isEmitterBillboardMtx;
    isDirectional = emitter.gravityProperties().isDirectional;
    isTexPatAnim = emitter.textureProperties().isTexPatAnim;
    isVelLook = emitter.basicProperties().isVelLook;
    volumeTblIndex = emitter.volumeProperties().volumeTblIndex;
    isStopEmitInFade = emitter.terminationProperties().isStopEmitInFade;
    volumeType = emitter.volumeProperties().volumeType;
    volumeRadius = emitter.volumeProperties().volumeRadius;
    volumeSweepStart = emitter.volumeProperties().volumeSweepStart;
    volumeSweepParam = emitter.volumeProperties().volumeSweepParam;
    figureVel = emitter.velocityProperties().figureVel;
    emitterVelDir = emitter.velocityProperties().emitterVelDir;
    initVel = emitter.velocityProperties().initVel;
    initVelRnd = emitter.velocityProperties().initVelRnd;
    spreadVec = emitter.velocityProperties().spreadVec;

    startFrame = emitter.emissionProperties().startFrame;
    endFrame = emitter.emissionProperties().endFrame;
    lifeStep = emitter.emissionProperties().lifeStep;
    lifeStepRnd = emitter.emissionProperties().lifeStepRnd;

    emitRate = emitter.emissionProperties().emitRate;
    ptclLife = emitter.lifespanProperties().ptclLife;
    ptclLifeRnd = emitter.lifespanProperties().ptclLifeRnd;
    airResistance = emitter.velocityProperties().airResistance;
    blendFunc = emitter.combinerProperties().blendFunc;
    billboardType = emitter.basicProperties().billboardType;
    depthFunc = emitter.combinerProperties().depthFunc;
    gravity = emitter.gravityProperties().gravity;
    color = emitter.colorProperties().colors;
    color0 = emitter.colorProperties().color0;
    colorSection1 = emitter.colorProperties().colorSection1;
    colorSection2 = emitter.colorProperties().colorSection2;
    colorSection3 = emitter.colorProperties().colorSection3;
    colorNumRepeat = emitter.colorProperties().colorNumRepeat;
    initAlpha = emitter.alphaProperties().initAlpha;
    diffAlpha21 = emitter.alphaProperties().diffAlpha21;
    diffAlpha32 = emitter.alphaProperties().diffAlpha32;
    alphaSection1 = emitter.alphaProperties().alphaSection1;
    alphaSection2 = emitter.alphaProperties().alphaSection2;

    initScale = emitter.scaleProperties().initScale;
    diffScale21 = emitter.scaleProperties().diffScale21;
    diffScale32 = emitter.scaleProperties().diffScale32;
    scaleSection1 = emitter.scaleProperties().scaleSection1;
    scaleSection2 = emitter.scaleProperties().scaleSection2;
    scaleRand = emitter.scaleProperties().scaleRand;

    rotCalcType = static_cast<u32>(emitter.rotationProperties().rotType) + 5 * static_cast<u32>(emitter.colorProperties().colorCalcType);
    followType = emitter.basicProperties().followType;
    colorCombinerFunc = emitter.combinerProperties().combinerFunc;
    initRot = emitter.rotationProperties().initRot;
    initRotRand = emitter.rotationProperties().initRotRand;
    rotVel = emitter.rotationProperties().rotVel;
    rotVelRand = emitter.rotationProperties().rotVelRand;
    rotBasis = emitter.rotationProperties().rotBasis;
    transformSRT = emitter.transformProperties().transformSRT;
    transformRT = emitter.transformProperties().transformRT;
    alphaAddInFade = emitter.terminationProperties().alphaAddInFade;
    numTexPat = emitter.textureProperties().numTexPat;
    numTexDivX = emitter.textureProperties().numTexDivX;
    numTexDivY = emitter.textureProperties().numTexDivY;
    texUVScale = emitter.textureProperties().texUVScale;
    std::copy(emitter.textureProperties().texPatTbl.begin(), emitter.textureProperties().texPatTbl.end(), texPatTbl.data());
    texPatFreq = emitter.textureProperties().texPatFreq;
    texPatTblUse = emitter.textureProperties().texPatTblUse;
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
        >> item.isPolygon
        >> item.isFollow
        >> item.isEmitterBillboardMtx
        >> item.isDirectional
        >> item.isTexPatAnim
        >> item.isVelLook
        >> item.volumeTblIndex
        >> item.isStopEmitInFade
        >> item.volumeType
        >> item.volumeRadius
        >> item.volumeSweepStart
        >> item.volumeSweepParam
        >> item.figureVel
        >> item.emitterVelDir
        >> item.initVel
        >> item.initVelRnd
        >> item.spreadVec
        >> item.startFrame
        >> item.endFrame
        >> item.lifeStep
        >> item.lifeStepRnd
        >> item.emitRate
        >> item.ptclLife
        >> item.ptclLifeRnd
        >> item.airResistance
        >> item.blendFunc
        >> item.billboardType
        >> item.depthFunc
        >> item.gravity
        >> item.color[0]
        >> item.color[1]
        >> item.color[2]
        >> item.color0
        >> item.colorSection1
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
        >> item.rotCalcType
        >> item.followType
        >> item.colorCombinerFunc
        >> item.initRot
        >> item.initRotRand
        >> item.rotVel
        >> item.rotVelRand
        >> item.rotBasis
        >> item.transformSRT
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
        << item.isPolygon
        << item.isFollow
        << item.isEmitterBillboardMtx
        << item.isDirectional
        << item.isTexPatAnim
        << item.isVelLook
        << item.volumeTblIndex
        << item.isStopEmitInFade
        << item.volumeType
        << item.volumeRadius
        << item.volumeSweepStart
        << item.volumeSweepParam
        << item.figureVel
        << item.emitterVelDir
        << item.initVel
        << item.initVelRnd
        << item.spreadVec
        << item.startFrame
        << item.endFrame
        << item.lifeStep
        << item.lifeStepRnd
        << item.emitRate
        << item.ptclLife
        << item.ptclLifeRnd
        << item.airResistance
        << item.blendFunc
        << item.billboardType
        << item.depthFunc
        << item.gravity
        << item.color[0]
        << item.color[1]
        << item.color[2]
        << item.color0
        << item.colorSection1
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
        << item.rotCalcType
        << item.followType
        << item.colorCombinerFunc
        << item.initRot
        << item.initRotRand
        << item.rotVel
        << item.rotVelRand
        << item.rotBasis
        << item.transformSRT
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
    qDebug() << indentation << "- textureSize:           " << textureSize;
    qDebug() << indentation << "- texturePos:            " << texturePos;
    qDebug() << indentation << "- textureHandlePtr:      " << textureHandlePtr;
    qDebug() << indentation << "- isPolygon:             " << isPolygon;
    qDebug() << indentation << "- isFollow:              " << isFollow;
    qDebug() << indentation << "- isEmitterBillboardMtx: " << isEmitterBillboardMtx;
    qDebug() << indentation << "- isDirectional:         " << isDirectional;
    qDebug() << indentation << "- isTexPatAnim:          " << isTexPatAnim;
    qDebug() << indentation << "- isVelLook:             " << isVelLook;
    qDebug() << indentation << "- volumeTblIndex:        " << volumeTblIndex;
    qDebug() << indentation << "- isStopEmitInFade:      " << isStopEmitInFade;
    qDebug() << indentation << "- volumeType:            " << volumeType;
    qDebug() << indentation << "- volumeRadius:          " << volumeRadius;
    qDebug() << indentation << "- volumeSweepStart:      " << volumeSweepStart;
    qDebug() << indentation << "- volumeSweepParam:      " << volumeSweepParam;
    qDebug() << indentation << "- figureVel:             " << figureVel;
    qDebug() << indentation << "- emitterVelDir:         " << emitterVelDir;
    qDebug() << indentation << "- initVel:               " << initVel;
    qDebug() << indentation << "- initVelRnd:            " << initVelRnd;
    qDebug() << indentation << "- spreadVec:             " << spreadVec;
    qDebug() << indentation << "- startFrame:            " << startFrame;
    qDebug() << indentation << "- endFrame:              " << endFrame;
    qDebug() << indentation << "- lifeStep:              " << lifeStep;
    qDebug() << indentation << "- lifeStepRnd:           " << lifeStepRnd;
    qDebug() << indentation << "- emitRate:              " << emitRate;
    qDebug() << indentation << "- ptclLife:              " << ptclLife;
    qDebug() << indentation << "- ptclLifeRnd:           " << ptclLifeRnd;
    qDebug() << indentation << "- airResistance:         " << airResistance;
    qDebug() << indentation << "- blendFunc:             " << blendFunc;
    qDebug() << indentation << "- billboardType:         " << billboardType;
    qDebug() << indentation << "- depthFunc:             " << depthFunc;
    qDebug() << indentation << "- gravity:               " << gravity;
    qDebug() << indentation << "- color:                 " << color.data();
    qDebug() << indentation << "- color0:                " << color0;
    qDebug() << indentation << "- colorSection1:         " << colorSection1;
    qDebug() << indentation << "- colorSection2:         " << colorSection2;
    qDebug() << indentation << "- colorSection3:         " << colorSection3;
    qDebug() << indentation << "- colorNumRepeat:        " << colorNumRepeat;
    qDebug() << indentation << "- initAlpha:             " << initAlpha;
    qDebug() << indentation << "- diffAlpha21:           " << diffAlpha21;
    qDebug() << indentation << "- diffAlpha32:           " << diffAlpha32;
    qDebug() << indentation << "- alphaSection1:         " << alphaSection1;
    qDebug() << indentation << "- alphaSection2:         " << alphaSection2;
    qDebug() << indentation << "- initScale:             " << initScale;
    qDebug() << indentation << "- diffScale21:           " << diffScale21;
    qDebug() << indentation << "- diffScale32:           " << diffScale32;
    qDebug() << indentation << "- scaleSection1:         " << scaleSection1;
    qDebug() << indentation << "- scaleSection2:         " << scaleSection2;
    qDebug() << indentation << "- scaleRand:             " << scaleRand;
    qDebug() << indentation << "- rotCalcType:           " << rotCalcType;
    qDebug() << indentation << "- followType:            " << followType;
    qDebug() << indentation << "- colorCombinerFunc:     " << colorCombinerFunc;
    qDebug() << indentation << "- initRot:               " << initRot;
    qDebug() << indentation << "- initRotRand:           " << initRotRand;
    qDebug() << indentation << "- rotVel:                " << rotVel;
    qDebug() << indentation << "- rotVelRand:            " << rotVelRand;
    qDebug() << indentation << "- rotBasis:              " << rotBasis;
    qDebug() << indentation << "- transformSRT:          " << transformSRT;
    qDebug() << indentation << "- transformRT:           " << transformRT;
    qDebug() << indentation << "- alphaAddInFade:        " << alphaAddInFade;
    qDebug() << indentation << "- numTexPat:             " << numTexPat;
    qDebug() << indentation << "- numTexDivX:            " << numTexDivX;
    qDebug() << indentation << "- numTexDivY:            " << numTexDivY;
    qDebug() << indentation << "- texUVScale:            " << texUVScale;
    qDebug() << indentation << "- texPatTbl:             " << texPatTbl.data();
    qDebug() << indentation << "- texPatFreq:            " << texPatFreq;
    qDebug() << indentation << "- texPatTblUse:          " << texPatTblUse;
}


// ========================================================================== //


BinComplexEmitterData::BinComplexEmitterData(const Ptcl::Emitter& emitter) :
    BinCommonEmitterData{emitter} {
    childFlag = emitter.complexProperties().childFlags;
    fieldFlag = emitter.complexProperties().fieldFlags;
    fluctuationFlag = emitter.complexProperties().fluctuationFlags;
    stripeFlag = emitter.complexProperties().stripeFlags;

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
    childEmitRate = childData.emissionProperties().emitRate;
    childEmitTiming = childData.emissionProperties().emitTiming;
    childLife = childData.emissionProperties().life;
    childEmitStep = childData.emissionProperties().emitStep;
    childVelInheritRate = childData.velocityProperties().velInheritRate;
    childFigurVel = childData.velocityProperties().figurVel;
    childRandVel = childData.velocityProperties().randVel;
    childInitPosRand = childData.velocityProperties().initPosRand;

    childBlendType = childData.combinerProperties().blendFunc;
    childBillboardType = childData.basicProperties().billboardType;
    childDepthType = childData.combinerProperties().depthFunc;

    if (childData.textureHandle().isValid()) {
        childTextureRes = {
            .width = static_cast<u16>(childData.textureHandle()->textureData().width()),
            .height = static_cast<u16>(childData.textureHandle()->textureData().height()),
            .format = childData.textureHandle()->textureFormat(),
            .wrapT = childData.textureProperties().textureWrapT,
            .wrapS = childData.textureProperties().textureWrapS,
            .magFilter = childData.textureProperties().textureMagFilter,
            .minMipFilter = static_cast<u8>((static_cast<u8>(childData.textureProperties().textureMinFilter) & 0x1) | ((static_cast<u8>(childData.textureProperties().textureMipFilter) & 0x3) << 1)),
        };
    } else {
        childTextureRes = {};
    }

    childTextureSize = 0; // To be assigned after construction...
    childTexturePos = 0; // To be assigned after construction...
    childTextureHandlePtr = 0;

    childColor0 = childData.colorProperties().color0;
    childColor1 = childData.colorProperties().color1;
    childAlpha = childData.alphaProperties().alpha;
    childAlphaTarget = childData.alphaProperties().alphaTarget;
    childAlphaInit = childData.alphaProperties().alphaInit;
    childScaleInheritRate = childData.scaleProperties().scaleInheritRate;
    childScale = childData.scaleProperties().scale;
    childRotType = childData.rotationProperties().rotType;
    childInitRot = childData.rotationProperties().initRot;
    childInitRotRand = childData.rotationProperties().initRotRand;
    childRotVel = childData.rotationProperties().rotVel;
    childRotVelRand = childData.rotationProperties().rotVelRand;
    childRotBasis = childData.rotationProperties().rotBasis;
    childGravity = childData.velocityProperties().gravity;
    childAlphaStartFrame = childData.alphaProperties().alphaStartFrame;
    childAlphaBaseFrame = childData.alphaProperties().alphaBaseFrame;
    childScaleStartFrame = childData.scaleProperties().scaleStartFrame;
    childScaleTarget = childData.scaleProperties().scaleTarget;
    childTexUScale = childData.textureProperties().texUVScale.getX();
    childTexVScale = childData.textureProperties().texUVScale.getY();
    childCombinerType = childData.combinerProperties().combinerFunc;
    childAirResist = childData.velocityProperties().airResist;
}


QDataStream& operator>>(QDataStream& in, BinChildData& item) {
    in >> item.childEmitRate
        >> item.childEmitTiming
        >> item.childLife
        >> item.childEmitStep
        >> item.childVelInheritRate
        >> item.childFigurVel
        >> item.childRandVel
        >> item.childInitPosRand
        >> item.childBlendType
        >> item.childBillboardType
        >> item.childDepthType
        >> item.childTextureRes
        >> item.childTextureSize
        >> item.childTexturePos
        >> item.childTextureHandlePtr
        >> item.childColor0
        >> item.childColor1
        >> item.childAlpha
        >> item.childAlphaTarget
        >> item.childAlphaInit
        >> item.childScaleInheritRate
        >> item.childScale
        >> item.childRotType
        >> item.childInitRot
        >> item.childInitRotRand
        >> item.childRotVel
        >> item.childRotVelRand
        >> item.childRotBasis
        >> item.childGravity
        >> item.childAlphaStartFrame
        >> item.childAlphaBaseFrame
        >> item.childScaleStartFrame
        >> item.childScaleTarget
        >> item.childTexUScale
        >> item.childTexVScale
        >> item.childCombinerType
        >> item.childAirResist;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BinChildData& item) {
    out << item.childEmitRate
        << item.childEmitTiming
        << item.childLife
        << item.childEmitStep
        << item.childVelInheritRate
        << item.childFigurVel
        << item.childRandVel
        << item.childInitPosRand
        << item.childBlendType
        << item.childBillboardType
        << item.childDepthType
        << item.childTextureRes
        << item.childTextureSize
        << item.childTexturePos
        << item.childTextureHandlePtr
        << item.childColor0
        << item.childColor1
        << item.childAlpha
        << item.childAlphaTarget
        << item.childAlphaInit
        << item.childScaleInheritRate
        << item.childScale
        << item.childRotType
        << item.childInitRot
        << item.childInitRotRand
        << item.childRotVel
        << item.childRotVelRand
        << item.childRotBasis
        << item.childGravity
        << item.childAlphaStartFrame
        << item.childAlphaBaseFrame
        << item.childScaleStartFrame
        << item.childScaleTarget
        << item.childTexUScale
        << item.childTexVScale
        << item.childCombinerType
        << item.childAirResist;
    return out;
}


// ========================================================================== //


BinFieldRandomData::BinFieldRandomData(const Ptcl::FieldData::FieldRandomData& fieldRandomData) {
    fieldRandomBlank = fieldRandomData.randomBlank;
    fieldRandomVelAdd = fieldRandomData.randomVelAdd;
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


BinFieldMagnetData::BinFieldMagnetData(const Ptcl::FieldData::FieldMagnetData& fieldMagnetData) {
    fieldMagnetPower = fieldMagnetData.magnetPower;
    fieldMagnetPos = fieldMagnetData.magnetPos;
    fieldMagnetFlag = fieldMagnetData.magnetFlag;
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


BinFieldSpinData::BinFieldSpinData(const Ptcl::FieldData::FieldSpinData& fieldSpinData) {
    fieldSpinRotate = fieldSpinData.spinRotate;
    fieldSpinAxis = fieldSpinData.spinAxis;
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


BinFieldCollisionData::BinFieldCollisionData(const Ptcl::FieldData::FieldCollisionData& fieldCollisionData) {
    fieldCollisionType = fieldCollisionData.collisionType;
    fieldCollisionIsWorld = fieldCollisionData.collisionIsWorld;
    fieldCollisionCoord = fieldCollisionData.collisionCoord;
    fieldCollisionCoef = fieldCollisionData.collisionCoef;
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


BinFieldConvergenceData::BinFieldConvergenceData(const Ptcl::FieldData::FieldConvergenceData& fieldConvergenceData) {
    fieldConvergenceType = fieldConvergenceData.convergenceType;
    fieldConvergencePos = fieldConvergenceData.convergencePos;
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


BinFieldPosAddData::BinFieldPosAddData(const Ptcl::FieldData::FieldPosAddData& fieldPosAddData) {
    fieldPosAdd = fieldPosAddData.posAdd;
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
    fluctuationScale = fluctuationData.fluctuationScale;
    fluctuationFreq = fluctuationData.fluctuationFreq;
    fluctuationPhaseRnd = fluctuationData.fluctuationPhaseRnd;
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
