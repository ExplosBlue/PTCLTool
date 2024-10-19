#include "ptcl/ptcl.h"

#include <QDataStream>
#include <QFile>

#include "ptcl/ptclRes.h"
#include "imageutil.h"

namespace Ptcl {

void Emitter::initFromBinary(const BinCommonEmitterData& emitterData) {

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

        // qDebug() << "BinColor {" << binColor.r << "," << binColor.r << "," << binColor.b << "," << binColor.a << "}";

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

    std::unordered_map<u32, u32> textureOffsetMap;

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

        auto emitterSet = std::make_unique<EmitterSet>();

        file.seek(emitterSetTblPos + (sizeof(BinEmitterSetData) * resEmitterSetIdx));
        BinEmitterSetData binEmitterSetData;
        stream >> binEmitterSetData;

        const char* name = nameTbl.data() + binEmitterSetData.namePos;
        emitterSet->setName(name);
        emitterSet->setUserData(binEmitterSetData.userData);
        emitterSet->setLastUpdateDate(binEmitterSetData.lastUpdateDate);

        emitterSet->emitters().reserve(binEmitterSetData.numEmitter);

        // Read Emitters
        if (binEmitterSetData.numEmitter > 0) {

            int emitterIdx = 0;
            while (binEmitterSetData.numEmitter > emitterIdx) {

                auto emitter = std::make_unique<Emitter>();

                file.seek(binEmitterSetData.emitterTblPos + (sizeof(BinEmitterTblData) * emitterIdx));
                BinEmitterTblData binEmitterTblData;
                stream >> binEmitterTblData;

                if (binEmitterTblData.emitterPos > 0) {

                    file.seek(binEmitterTblData.emitterPos);
                    BinCommonEmitterData binCommonEmitterData{};
                    stream >> binCommonEmitterData;

                    emitter->initFromBinary(binCommonEmitterData);

                    const char* name = nameTbl.data() + binCommonEmitterData.namePos;
                    emitter->setName(name);

                    // Read texture data
                    u32 textureOffset = textureTblPos + binCommonEmitterData.texturePos;

                    if (textureOffsetMap.find(textureOffset) == textureOffsetMap.end()) {

                        file.seek(textureOffset);
                        QByteArray textureData;

                        auto height = binCommonEmitterData.textureRes.height;
                        auto width = binCommonEmitterData.textureRes.width;
                        auto format = binCommonEmitterData.textureRes.format;
                        auto size = binCommonEmitterData.textureSize;

                        textureData.resize(size);
                        stream.readRawData(textureData.data(), size);

                        auto texture = std::make_shared<Texture>(ImageUtil::createTexture(textureData, width, height, format));
                        mTextures.push_back(std::move(texture));

                        u32 textureIdx = mTextures.size() - 1;
                        textureOffsetMap[textureOffset] = textureIdx;
                    }

                    emitter->setTexture(mTextures[textureOffsetMap[textureOffset]]);

                    if (binCommonEmitterData.type == EmitterType::Complex) {
                        file.seek(binEmitterTblData.emitterPos + sizeof(BinCommonEmitterData));
                        BinComplexEmitterData binComplexEmitterData;
                        stream >> binComplexEmitterData;

                        // TODO: handle complex emitter data
                        // if ((type = complex) && (_1F4 & 1) != 0) => textureHandle2 = textureTbl + unkTexturePos
                    }


                    if (binCommonEmitterData.type == EmitterType::UnkType2) {
                        file.seek(binEmitterTblData.emitterPos + sizeof(BinCommonEmitterData));
                        BinEmitterDataType2 binEmitterDataType2;
                        stream >> binEmitterDataType2;
                        // TODO: Do something with this data...
                    }
                }

                emitterSet->emitters().push_back(std::move(emitter));

                ++emitterIdx;
            }
        }
        mEmitterSets.push_back(std::move(emitterSet));
    }
}

/*
void PtclRes::save(const QString& filePath) {

    // Structure Overview
    // ╔═════════════════════════════════════╗
    // ║    HeaderData                       ║
    // ║    Offset: 0x00   Size: 0x1C        ║
    // ╠═════════════════════════════════════╣
    // ║    EmitterSetData                   ║
    // ║    Offset: 0x1C   Size: EmitterSet  ║
    // ╠═════════════════════════════════════╣
    // ║    EmitterSetTblData                ║
    // ║    Offset: X     Size: EmitterTbl   ║
    // ╠═════════════════════════════════════╣
    // ║    EmitterData                      ║
    // ║    Offset: Y     Size: EmitterData  ║
    // ╠═════════════════════════════════════╣
    // ║    TextureTbl                       ║
    // ║    Offset: Z     Size: ?            ║
    // ╠═════════════════════════════════════╣
    // ║    NameTbl                          ║
    // ║    Offset: W     Size: ?            ║
    // ╚═════════════════════════════════════╝

    // std::unordered_map<u32, u32> textureOffsetMap;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        throw("PtclRes::save - Failed to open file.");
        return;
    }

    file.seek(0);

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);

    QByteArray nameTbl;
    std::vector<BinEmitterTblData> emitterTables;

    QByteArray emitterDataTable;

    const u32 basePos = 0;
    const u32 emitterSetTblPos = basePos + sizeof(BinHeaderData);
    const u32 emitterTblDataTblPos = emitterSetTblPos + (sizeof(BinEmitterSetData) * mEmitterSets.size());

    // Create binary header + write to file

    BinHeaderData headerData;
    *headerData.magic.data() = *"BPBD"; // TODO - this but properly
    headerData.version = 11;
    headerData.numEmitterSet = mEmitterSets.size();

    // Add Name to name table
    headerData.namePos = nameTbl.size(); // name pos is always table size before appending name
    nameTbl.append(name().toUtf8());

    headerData.nameTblPos = 0;     // TODO - calculate this
    headerData.textureTblPos = 0;  // TODO - calculate this
    headerData.textureTblSize = 0; // TODO - calculate this

    for (auto& emitterSet : mEmitterSets) {

        BinEmitterSetData binEmitterSet;
        binEmitterSet.userData = emitterSet->userData();
        binEmitterSet.lastUpdateDate = emitterSet->lastUpdateDate();

        // Add Name to name table
        binEmitterSet.namePos = nameTbl.size(); // name pos is always table size before appending name
        nameTbl.append(emitterSet->name().toUtf8());
        binEmitterSet.namePtr = 0;

        binEmitterSet.numEmitter = emitterSet->emitterCount();
        binEmitterSet.emitterTblPos = 0 + emitterTblDataTblPos + (emitterTables.size() * sizeof(BinEmitterTblData));

        for (auto& emitter : emitterSet->emitters()) {

            const Ptcl::Emitter& e = *emitter;

            // Build emitter data
            BinCommonEmitterData binCommonEmitterData(e);

            // Add Name to name table
            binCommonEmitterData.namePos = nameTbl.size(); // name pos is always table size before appending name
            nameTbl.append(emitter->name().toUtf8());
            binCommonEmitterData.namePtr = 0;

            // TODO: not this...
            binCommonEmitterData.textureSize = ImageUtil::getTextureSize(binCommonEmitterData.textureRes.width, binCommonEmitterData.textureRes.height, binCommonEmitterData.textureRes.format, 32);
            binCommonEmitterData.texturePos = 0; // TODO
            binCommonEmitterData.textureHandlePtr = 0;

            emitterDataTable.size();

            // Build emitter table data
            BinEmitterTblData binTableData;
            binTableData.emitterPos = ;
            binTableData.emitterPtr = 0;
            emitterTables.push_back(binTableData);
        }

    }

    // Read EmitterSets
    mEmitterSets.reserve(binHeader.numEmitterSet);

    for (int resEmitterSetIdx = 0; resEmitterSetIdx < binHeader.numEmitterSet; resEmitterSetIdx++) {

        auto emitterSet = std::make_unique<EmitterSet>();

        file.seek(emitterSetTblPos + (sizeof(BinEmitterSetData) * resEmitterSetIdx));
        BinEmitterSetData binEmitterSetData;
        stream >> binEmitterSetData;

        const char* name = nameTbl.data() + binEmitterSetData.namePos;
        emitterSet->setName(name);
        emitterSet->setUserData(binEmitterSetData.userData);

        emitterSet->emitters().reserve(binEmitterSetData.numEmitter);

        // Read Emitters
        if (binEmitterSetData.numEmitter > 0) {

            int emitterIdx = 0;
            while (binEmitterSetData.numEmitter > emitterIdx) {

                auto emitter = std::make_unique<Emitter>();

                file.seek(binEmitterSetData.emitterTblPos + (sizeof(BinEmitterTblData) * emitterIdx));
                BinEmitterTblData binEmitterTblData;
                stream >> binEmitterTblData;

                if (binEmitterTblData.emitterPos > 0) {

                    file.seek(binEmitterTblData.emitterPos);
                    BinCommonEmitterData binCommonEmitterData;
                    stream >> binCommonEmitterData;

                    emitter->initFromBinary(binCommonEmitterData);

                    const char* name = nameTbl.data() + binCommonEmitterData.namePos;
                    emitter->setName(name);

                    // Read texture data
                    u32 textureOffset = textureTblPos + binCommonEmitterData.texturePos;

                    if (textureOffsetMap.find(textureOffset) == textureOffsetMap.end()) {

                        file.seek(textureOffset);
                        QByteArray textureData;

                        auto height = binCommonEmitterData.textureRes.height;
                        auto width = binCommonEmitterData.textureRes.width;
                        auto format = binCommonEmitterData.textureRes.format;
                        auto size = binCommonEmitterData.textureSize;

                        textureData.resize(size);
                        stream.readRawData(textureData.data(), size);

                        auto texture = std::make_shared<Texture>(ImageUtil::createTexture(textureData, width, height, format));
                        mTextures.push_back(std::move(texture));

                        u32 textureIdx = mTextures.size() - 1;
                        textureOffsetMap[textureOffset] = textureIdx;
                    }

                    emitter->setTexture(mTextures[textureOffsetMap[textureOffset]]);

                    if (binCommonEmitterData.type == EmitterType::COMPLEX) {
                        file.seek(binEmitterTblData.emitterPos + sizeof(BinCommonEmitterData));
                        BinComplexEmitterData binComplexEmitterData;
                        stream >> binComplexEmitterData;

                        // TODO: handle complex emitter data
                        // if ((type = complex) && (_1F4 & 1) != 0) => textureHandle2 = textureTbl + unkTexturePos
                    }


                    if (binCommonEmitterData.type == EmitterType::UNK_EMITTER_TYPE) {
                        file.seek(binEmitterTblData.emitterPos + sizeof(BinCommonEmitterData));
                        BinEmitterDataType2 binEmitterDataType2;
                        stream >> binEmitterDataType2;
                        // TODO: Do something with this data...
                    }
                }

                emitterSet->emitters().push_back(std::move(emitter));

                ++emitterIdx;
            }
        }
        mEmitterSets.push_back(std::move(emitterSet));
    }
}
*/

} // namespace Ptcl
