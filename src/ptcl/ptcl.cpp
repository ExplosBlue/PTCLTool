#include "ptcl/ptcl.h"

#include <QDataStream>
#include <QFile>

#include "ptcl/ptclBinary.h"
#include "imageUtil.h"

namespace Ptcl {

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

                    qDebug() << binCommonEmitterData.texturePos;

                    u32 textureOffset = textureTblPos + binCommonEmitterData.texturePos;

                    if (textureOffsetMap.find(textureOffset) == textureOffsetMap.end()) {

                        auto height = binCommonEmitterData.textureRes.height;
                        auto width = binCommonEmitterData.textureRes.width;
                        auto format = binCommonEmitterData.textureRes.format;
                        auto size = binCommonEmitterData.textureSize;

                        file.seek(textureOffset);
                        std::vector<u8> textureData(size);
                        auto bytesRead = stream.readRawData(reinterpret_cast<char*>(textureData.data()), size);
                        if (bytesRead != size) {
                            qWarning() << "Warning: Expected to read" << size << "bytes but only read" << bytesRead;
                        }

                        auto texture = std::make_shared<Texture>(Texture(&textureData, width, height, format));
                        mTextures.push_back(texture);

                        u32 textureIdx = mTextures.size() - 1;
                        textureOffsetMap[textureOffset] = textureIdx;
                    }

                    emitter->setTexture(mTextures[textureOffsetMap[textureOffset]]);

                    if (binCommonEmitterData.type == EmitterType::Complex) {
                        file.seek(binEmitterTblData.emitterPos + sizeof(BinCommonEmitterData));
                        BinComplexEmitterData binComplexEmitterData;
                        stream >> binComplexEmitterData;

                        std::vector<char> complexData;
                        stream.readRawData(complexData.data(), binComplexEmitterData.mDataSize);
                        emitter->setComplexData(complexData);
                        emitter->setComplexEmitterData(binComplexEmitterData);

                        // TODO: handle complex emitter data
                        // if ((type = complex) && (_1F4 & 1) != 0) => textureHandle2 = textureTbl + unkTexturePos
                    }


                    if (binCommonEmitterData.type == EmitterType::UnkType2) {
                        file.seek(binEmitterTblData.emitterPos + sizeof(BinCommonEmitterData));
                        BinEmitterDataType2 binEmitterDataType2;
                        stream >> binEmitterDataType2;
                        // TODO: Do something with this data...
                        std::vector<char> complexData;
                        stream.readRawData(complexData.data(), binEmitterDataType2.mDataSize);
                        emitter->setComplexData(complexData);
                        emitter->setEmitterDataType2(binEmitterDataType2);
                    }
                }

                emitterSet->emitters().push_back(std::move(emitter));

                ++emitterIdx;
            }
        }
        mEmitterSets.push_back(std::move(emitterSet));
    }
}

u32 PtclRes::emitterSetCount() const {

    return mEmitterSets.size();
}

u32 PtclRes::emitterCount() const {

    // TODO: this could probably be optimized

    s32 count = 0;

    for (auto& emitterSet : mEmitterSets) {
        count += emitterSet->emitterCount();
    }
    return count;
}

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

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        throw("PtclRes::save - Failed to open file.");
        return;
    }

    std::vector<BinEmitterSetData> binEmitterSetsList;
    std::vector<BinEmitterTblData> binEmitterTblsList;
    std::vector<std::unique_ptr<BinCommonEmitterData>> binEmitterDataList;
    std::vector<char> nameTbl;

    constexpr u32 headerBasePos = 0;
    constexpr u32 emitterSetsBasePos = headerBasePos + sizeof(BinHeaderData);
    const u32 emitterTblDataBasePos = emitterSetsBasePos + (emitterSetCount() * sizeof(BinEmitterSetData));
    const u32 emitterDataBasePos = emitterTblDataBasePos + (emitterCount() * sizeof(BinEmitterTblData));

    u32 emitterSetsCurOffset = emitterSetsBasePos;
    u32 emitterTblDataCurOffset = emitterTblDataBasePos;
    u32 emitterDataCurOffset = emitterDataBasePos;
    u32 nameTblCurOffset = 0;

    auto appendToNameTbl = [&nameTbl, &nameTblCurOffset](const QString& name) {
        QByteArray byteArray = name.toUtf8();
        const char* cStr = byteArray.constData();

        nameTbl.insert(nameTbl.end(), cStr, cStr + byteArray.size());
        nameTbl.push_back('\0');

        nameTblCurOffset += byteArray.size() + 1;
    };

    // Create binary header
    BinHeaderData headerData = {
        .magic          = {'S', 'P', 'B', 'D'},
        .version        = 11,
        .numEmitterSet  = emitterSetCount(),
        .namePos        = nameTblCurOffset,
        .nameTblPos     = 0,
        .textureTblPos  = 0,
        .textureTblSize = 0
    };

    // Add PTCL name to name table
    appendToNameTbl(name());

    for (auto& emitterSet : mEmitterSets) {

        // Create Binary emitterSet
        BinEmitterSetData binEmitterSet = {
            .userData       = emitterSet->userData(),
            .lastUpdateDate = emitterSet->lastUpdateDate(),
            .namePos        = nameTblCurOffset,
            .namePtr        = 0,
            .numEmitter     = emitterSet->emitterCount(),
            .emitterTblPos  = emitterTblDataCurOffset,
            .emitterTbl     = 0
        };

        // Add emitterSet name to name table
        appendToNameTbl(emitterSet->name());

        for (auto& emitter : emitterSet->emitters()) {

            // Create emiterTblData
            BinEmitterTblData emitterTblData = {
                .emitterPos = emitterDataCurOffset,
                .emitterPtr = 0
            };

            emitterTblDataCurOffset += sizeof(BinEmitterTblData);
            binEmitterTblsList.push_back(emitterTblData);

            // Create emitterData
            if (emitter->type() == Ptcl::EmitterType::Simple) {

                qDebug() << "Creating Simple Emitter Data";

                // Create Simple EmitterData
                BinCommonEmitterData emitterData(*emitter.get());

                emitterData.namePos = nameTblCurOffset;
                appendToNameTbl(emitter->name());

                // TODO: set emitterData texture pos...

                emitterDataCurOffset += sizeof(BinCommonEmitterData);
                binEmitterDataList.push_back(std::make_unique<BinCommonEmitterData>(emitterData));

            } else if (emitter->type() == Ptcl::EmitterType::Complex) {

                qDebug() << "Creating Complex Emitter Data";

                // Create Complex EmitterData
                BinComplexEmitterData emitterData(*emitter.get());

                emitterData.namePos = nameTblCurOffset;
                appendToNameTbl(emitter->name());

                emitterDataCurOffset += emitterData.mDataSize; // This is dumb
                binEmitterDataList.push_back(std::make_unique<BinCommonEmitterData>(emitterData));

            } else if (emitter->type() == Ptcl::EmitterType::UnkType2) {

                qDebug() << "Creating Unk Emitter Data";

                // Create UnkType2 EmitterData
                BinEmitterDataType2 emitterData(*emitter.get());

                emitterData.namePos = nameTblCurOffset;
                appendToNameTbl(emitter->name());

                emitterDataCurOffset += emitterData.mDataSize;
                binEmitterDataList.push_back(std::make_unique<BinEmitterDataType2>(emitterData));
            }
        }

        // Increment current offset
        emitterSetsCurOffset += sizeof(BinEmitterSetData);
        binEmitterSetsList.push_back(binEmitterSet);
    }

    // TODO: Build texture table

    // Write to the file
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << headerData;

    for (auto& emtSet : binEmitterSetsList) {
        stream << emtSet;
    }

    for (auto& emtTbl : binEmitterTblsList) {
        stream << emtTbl;
    }

    for (auto& emtData : binEmitterDataList) {

        switch (emtData->type) {
        case Ptcl::EmitterType::Simple:
            stream << *emtData.get();
            break;
        case Ptcl::EmitterType::Complex:
            stream << static_cast<BinComplexEmitterData&>(*emtData.get());
            break;
        case Ptcl::EmitterType::UnkType2:
            stream << static_cast<BinEmitterDataType2&>(*emtData.get());
            break;
        default:
            break;
        }
    }

    // TODO: Write texture table

    s32 nameTblPos = file.pos();
    stream.writeRawData(nameTbl.data(), nameTbl.size());

    // Update header with nameTbl/textureTbl pos & size
    headerData.nameTblPos = nameTblPos;

    file.seek(0);
    stream << headerData; // would be better to avoid doing this twice...

    file.flush();
    file.close();
}


const QString& PtclRes::name() const {

    return mName;
}


void PtclRes::setName(const QString& name) {

    mName = name;
}

EmitterSetList& PtclRes::getEmitterSets() {

    return mEmitterSets;
}

void PtclRes::addTexture(const Texture& texture) {

}

void PtclRes::removeTexture(u32 textureIndex) {

}

const TextureList& PtclRes::textures() const {

    return mTextures;
}


TextureList& PtclRes::textures() {

    return mTextures;
}

} // namespace Ptcl
