#include "ptcl/ptcl.h"

#include <QDataStream>
#include <QFile>
#include <ranges>

#include "ptcl/ptclRes.h"
#include "imageutil.h"

namespace Ptcl {

void Emitter::setFromBinEmitter(const BinCommonEmitterData& emitterData) {

    mType = emitterData.type;
    mFlag = emitterData.flag;
    mRandomSeed = emitterData.randomSeed;
    // mName
    // mTexture
    // mTextureFormat
    m_2C = emitterData._2C;
    m_2D = emitterData._2D;
    m_2E = emitterData._2E;
    m_2F = emitterData._2F;
    m_30 = emitterData._30;
    m_31 = emitterData._31;
    m_32 = emitterData._32;
    mVolumeType = emitterData.volumeType;
    mVolumeRadius = QVector3D(emitterData.volumeRadius.x, emitterData.volumeRadius.y, emitterData.volumeRadius.z);
    m_44 = emitterData._44;
    m_48 = emitterData._48;
    m_4C = emitterData._4C;
    std::ranges::copy(emitterData._50, m_50.begin());
    mPtclLife = emitterData.ptclLife;
    m_88 = emitterData._88;
    m_8C = emitterData._8C;
    std::ranges::copy(emitterData._90, m_90.begin());
    m_9C = emitterData._9C;
    m_A0 = emitterData._A0;
    m_A4 = emitterData._A4;

    for (size_t idx = 0; idx < mColors.size(); ++idx) {
        auto& color = mColors[idx];
        auto& binColor = emitterData.color[idx];

        qDebug() << "BinColor {" << binColor.r << "," << binColor.r << "," << binColor.b << "," << binColor.a << "}";

        color.fromRgbF(binColor.r, binColor.g, binColor.b, binColor.a);
    }

    std::ranges::copy(emitterData._D8, m_D8.begin());

    mColorSection1 = emitterData.colorSection1;
    mColorSection2 = emitterData.colorSection2;
    mColorSection3 = emitterData.colorSection3;
    mColorNumRepeat = emitterData.colorNumRepeat;
    mInitAlpha = emitterData.initAlpha;

    std::ranges::copy(emitterData._F8, m_F8.begin());

    mInitScale = QVector2D(emitterData.initScale.x, emitterData.initScale.y);

    std::ranges::copy(emitterData._110, m_110.begin());

    m_170 = QMatrix3x4(emitterData._170.cells.data());
    m_1A0 = QMatrix3x4(emitterData._1A0.cells.data());

    std::ranges::copy(emitterData._1D0, m_1D0.begin());
}

void PtclRes::load(const QString& filePath) {

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        throw("PtclRes::load - Failed to open file.");
        return;
    }

    file.seek(0);

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);

    // Read Binary Header
    BinHeaderData binHeader;
    stream >> binHeader;

    // Read tables
    s64 emitterSetTblPos = file.pos();

    file.seek(binHeader.nameTblPos);
    QByteArray nameTbl = file.readAll();

    file.seek(binHeader.textureTblPos);
    s64 textureTblPos = file.pos();

    // Read name
    const char* name = nameTbl.data() + binHeader.namePos;
    setName(name);

    // Read EmitterSets
    mEmitterSets.reserve(binHeader.numEmitterSet);

    for (int resEmitterSetIdx = 0; resEmitterSetIdx < binHeader.numEmitterSet; resEmitterSetIdx++) {

        EmitterSet emitterSet;

        file.seek(emitterSetTblPos + (sizeof(BinEmitterSetData) * resEmitterSetIdx));
        BinEmitterSetData binEmitterSetData;
        stream >> binEmitterSetData;

        const char* name = nameTbl.data() + binEmitterSetData.namePos;
        emitterSet.setName(name);
        emitterSet.setUserData(binEmitterSetData.userData);

        emitterSet.emitters().reserve(binEmitterSetData.numEmitter);

        // Read Emitters
        if (binEmitterSetData.numEmitter > 0) {

            int emitterIdx = 0;
            while (binEmitterSetData.numEmitter > emitterIdx) {

                Emitter emitter;

                file.seek(binEmitterSetData.emitterTblPos + (sizeof(BinEmitterTblData) * emitterIdx));
                BinEmitterTblData binEmitterTblData;
                stream >> binEmitterTblData;

                if (binEmitterTblData.emitterPos > 0) {

                    file.seek(binEmitterTblData.emitterPos);
                    BinCommonEmitterData binCommonEmitterData;
                    stream >> binCommonEmitterData;

                    const char* name = nameTbl.data() + binCommonEmitterData.namePos;
                    emitter.setName(name);

                    file.seek(textureTblPos + binCommonEmitterData.texturePos);
                    QByteArray textureData;

                    auto height = binCommonEmitterData.textureRes.height;
                    auto width = binCommonEmitterData.textureRes.width;
                    auto format = binCommonEmitterData.textureRes.format;
                    auto size = binCommonEmitterData.textureSize;

                    textureData.resize(size);
                    stream.readRawData(textureData.data(), size);

                    // TODO: Textures should be stored externally from emitter and referenced to allow for texture reuse
                    emitter.setTexture(ImageUtil::picaTextureToQImage(textureData, width, height, format));
                    emitter.setTextureFormat(format);

                    emitter.setFromBinEmitter(binCommonEmitterData);

                    // TODO: handle complex emitter data
                    // if ((type = complex) && (_1F4 & 1) != 0) => textureHandle2 = textureTbl + unkTexturePos
                }

                emitterSet.emitters().push_back(emitter);

                ++emitterIdx;
            }
        }
        mEmitterSets.push_back(emitterSet);
    }
}

} // namespace Ptcl
