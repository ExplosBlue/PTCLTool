#include "ptcl/ptcl.h"

#include <variant>

#include <QDataStream>
#include <QFile>
#include <QStringDecoder>

#include "ptcl/ptclBinary.h"
#include "imageUtil.h"
#include "stringUtil.h"

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
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

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
        QString decodedName = StringUtil::shiftJISToQString(name);
        emitterSet->setName(decodedName);

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
                    QString decodedName = StringUtil::shiftJISToQString(name);
                    emitter->setName(decodedName);

                    // Read texture data
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

                    if (binCommonEmitterData.type == EmitterType::Complex || binCommonEmitterData.type == EmitterType::UnkType2) {
                        file.seek(binEmitterTblData.emitterPos + sizeof(BinCommonEmitterData));
                        BinComplexEmitterData binComplexEmitterData;
                        stream >> binComplexEmitterData;

                        // Store ComplexData flags
                        emitter->childFlags() = binComplexEmitterData.childFlag;
                        emitter->fieldFlags() = binComplexEmitterData.fieldFlag;
                        emitter->fluctuationFlags() = binComplexEmitterData.fluctuationFlag;
                        emitter->stripeFlags() = binComplexEmitterData.stripeFlag;
                        emitter->setHasStripeData(binComplexEmitterData.stripeDataOffset != 0);

                        // ChildData
                        if (binComplexEmitterData.childFlag.isSet(ChildFlag::Enabled)) {
                            BinChildData binChildData;
                            stream >> binChildData;
                            emitter->childData().initFromBinary(binChildData);

                            // Read texture data
                            u32 textureOffset = textureTblPos + binChildData.texturePos;

                            if (textureOffsetMap.find(textureOffset) == textureOffsetMap.end()) {

                                auto height = binChildData.textureRes.height;
                                auto width = binChildData.textureRes.width;
                                auto format = binChildData.textureRes.format;
                                auto size = binChildData.textureSize;

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

                            emitter->childData().setTexture(mTextures[textureOffsetMap[textureOffset]]);
                        }

                        // FieldData
                        if (binComplexEmitterData.fieldFlag.isSet(FieldFlag::Random)) {
                            BinFieldRandomData binRandomData;
                            stream >> binRandomData;
                            emitter->fieldRandomData().initFromBinary(binRandomData);
                        }
                        if (binComplexEmitterData.fieldFlag.isSet(FieldFlag::Magnet)) {
                            BinFieldMagnetData binMagnetData;
                            stream >> binMagnetData;
                            emitter->fieldMagnetData().initFromBinary(binMagnetData);
                        }
                        if (binComplexEmitterData.fieldFlag.isSet(FieldFlag::Spin)) {
                            BinFieldSpinData binSpinData;
                            stream >> binSpinData;
                            emitter->fieldSpinData().initFromBinary(binSpinData);
                        }
                        if (binComplexEmitterData.fieldFlag.isSet(FieldFlag::Collision)) {
                            BinFieldCollisionData binCollisionData;
                            stream >> binCollisionData;
                            emitter->fieldCollisionData().initFromBinary(binCollisionData);
                        }
                        if (binComplexEmitterData.fieldFlag.isSet(FieldFlag::Convergence)) {
                            BinFieldConvergenceData binConvergenceData;
                            stream >> binConvergenceData;
                            emitter->fieldConvergenceData().initFromBinary(binConvergenceData);
                        }
                        if (binComplexEmitterData.fieldFlag.isSet(FieldFlag::PosAdd)) {
                            BinFieldPosAddData binPosAddData;
                            stream >> binPosAddData;
                            emitter->fieldPosAddData().initFromBinary(binPosAddData);
                        }

                        // FluctuationData
                        if (binComplexEmitterData.fluctuationFlag.isSet(FluctuationFlag::Enabled)) {
                            BinFluctuationData binFluctuationData;
                            stream >> binFluctuationData;
                            emitter->fluctuationData().initFromBinary(binFluctuationData);
                        }

                        // StripeData
                        // TODO: Should this be checking billboard type?
                        if (binComplexEmitterData.stripeDataOffset != 0) {
                            BinStripeData binStripeData;
                            stream >> binStripeData;
                            emitter->stripeData().initFromBinary(binStripeData);
                        }
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

    using DataBlockVariant = std::variant<
        BinCommonEmitterData,
        BinComplexEmitterData,
        BinChildData,
        BinFieldRandomData,
        BinFieldMagnetData,
        BinFieldSpinData,
        BinFieldCollisionData,
        BinFieldConvergenceData,
        BinFieldPosAddData,
        BinFluctuationData,
        BinStripeData
    >;

    std::vector<BinEmitterSetData> binEmitterSetsList;
    std::vector<BinEmitterTblData> binEmitterTblsList;
    std::vector<DataBlockVariant> binEmitterDataList;
    std::vector<char> nameTbl;
    std::vector<char> textureTbl;

    std::unordered_map<u32, u32> textureOffsetMap;

    constexpr u32 headerBasePos = 0;
    constexpr u32 emitterSetsBasePos = headerBasePos + sizeof(BinHeaderData);
    const u32 emitterTblDataBasePos = emitterSetsBasePos + (emitterSetCount() * sizeof(BinEmitterSetData));
    const u32 emitterDataBasePos = emitterTblDataBasePos + (emitterCount() * sizeof(BinEmitterTblData));

    u32 emitterSetsCurOffset = emitterSetsBasePos;
    u32 emitterTblDataCurOffset = emitterTblDataBasePos;
    u32 emitterDataCurOffset = emitterDataBasePos;
    u32 nameTblCurOffset = 0;
    u32 textureTblCurOffset = 0;

    auto appendToNameTbl = [&nameTbl, &nameTblCurOffset](const QString& name) {
        QByteArray byteArray = StringUtil::qStringToShiftJIS(name);
        const char* cStr = byteArray.constData();

        nameTbl.insert(nameTbl.end(), cStr, cStr + byteArray.size());
        nameTbl.push_back('\0');

        nameTblCurOffset += byteArray.size() + 1;
    };

    auto appendToTextureTbl = [&textureTbl, &textureTblCurOffset](const std::vector<u8>& textureData) {
        textureTbl.insert(textureTbl.end(), textureData.begin(), textureData.end());
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

                // Create Simple EmitterData
                BinCommonEmitterData emitterData(*emitter.get());

                emitterData.namePos = nameTblCurOffset;
                appendToNameTbl(emitter->name());

                // Store texture data
                if (textureOffsetMap.contains(emitter->textureHandle()->Id())) {
                    emitterData.texturePos = textureOffsetMap[emitter->textureHandle()->Id()];
                    emitterData.textureSize = emitter->textureHandle()->textureDataRaw().size();
                } else {
                    qint64 paddingNeeded = (128 - (emitter->textureHandle()->textureDataRaw().size() % 128)) % 128;

                    emitterData.texturePos = textureTblCurOffset;
                    textureOffsetMap.try_emplace(emitter->textureHandle()->Id(), textureTblCurOffset);
                    appendToTextureTbl(emitter->textureHandle()->textureDataRaw());

                    if (paddingNeeded > 0) {
                        QByteArray padding(paddingNeeded, '\0');
                        textureTbl.insert(textureTbl.end(), padding.begin(), padding.end());
                    }

                    emitterData.textureSize = emitter->textureHandle()->textureDataRaw().size();
                    textureTblCurOffset += emitterData.textureSize + paddingNeeded;
                }

                emitterDataCurOffset += sizeof(BinCommonEmitterData);
                binEmitterDataList.emplace_back(emitterData);

            } else if (emitter->type() == Ptcl::EmitterType::Complex || emitter->type() == Ptcl::EmitterType::UnkType2) {

                // Create Complex EmitterData
                BinComplexEmitterData emitterData(*emitter.get());

                emitterData.namePos = nameTblCurOffset;
                appendToNameTbl(emitter->name());

                // Store texture data
                if (textureOffsetMap.contains(emitter->textureHandle()->Id())) {
                    emitterData.texturePos = textureOffsetMap[emitter->textureHandle()->Id()];
                    emitterData.textureSize = emitter->textureHandle()->textureDataRaw().size();
                } else {
                    qint64 paddingNeeded = (128 - (emitter->textureHandle()->textureDataRaw().size() % 128)) % 128;

                    emitterData.texturePos = textureTblCurOffset;
                    textureOffsetMap.try_emplace(emitter->textureHandle()->Id(), textureTblCurOffset);
                    appendToTextureTbl(emitter->textureHandle()->textureDataRaw());

                    if (paddingNeeded > 0) {
                        QByteArray padding(paddingNeeded, '\0');
                        textureTbl.insert(textureTbl.end(), padding.begin(), padding.end());
                    }

                    emitterData.textureSize = emitter->textureHandle()->textureDataRaw().size();
                    textureTblCurOffset += emitterData.textureSize + paddingNeeded;
                }

                u32 emitterDataSize = sizeof(BinComplexEmitterData);

                BinChildData binChildData(emitter->childData());

                BinFieldRandomData binRandomData(emitter->fieldRandomData());
                BinFieldMagnetData binMagnetData(emitter->fieldMagnetData());
                BinFieldSpinData binSpinData(emitter->fieldSpinData());
                BinFieldCollisionData binCollisionData(emitter->fieldCollisionData());
                BinFieldConvergenceData binConvergenceData(emitter->fieldConvergenceData());
                BinFieldPosAddData binPosAddData(emitter->fieldPosAddData());

                BinFluctuationData binFluctuationData(emitter->fluctuationData());

                BinStripeData binStripeData(emitter->stripeData());

                emitterData.childDataOffset = emitterDataSize;

                // ChildData
                if (emitterData.childFlag.isSet(ChildFlag::Enabled)) {
                    emitterDataSize += sizeof(BinChildData);

                    // Store texture data
                    if (textureOffsetMap.contains(emitter->childData().textureHandle()->Id())) {
                        binChildData.texturePos = textureOffsetMap[emitter->childData().textureHandle()->Id()];
                        binChildData.textureSize = emitter->childData().textureHandle()->textureDataRaw().size();
                    } else {
                        qint64 paddingNeeded = (128 - (emitter->childData().textureHandle()->textureDataRaw().size() % 128)) % 128;

                        binChildData.texturePos = textureTblCurOffset;
                        textureOffsetMap.try_emplace(emitter->childData().textureHandle()->Id(), textureTblCurOffset);
                        appendToTextureTbl(emitter->childData().textureHandle()->textureDataRaw());

                        if (paddingNeeded > 0) {
                            QByteArray padding(paddingNeeded, '\0');
                            textureTbl.insert(textureTbl.end(), padding.begin(), padding.end());
                        }

                        binChildData.textureSize = emitter->childData().textureHandle()->textureDataRaw().size();
                        textureTblCurOffset += binChildData.textureSize + paddingNeeded;
                    }
                }

                emitterData.fieldDataOffset = emitterDataSize;

                // FieldData
                if (emitter->fieldFlags().isSet(FieldFlag::Random)) {
                    emitterDataSize += sizeof(BinFieldRandomData);
                }
                if (emitter->fieldFlags().isSet(FieldFlag::Magnet)) {
                    emitterDataSize += sizeof(BinFieldMagnetData);
                }
                if (emitter->fieldFlags().isSet(FieldFlag::Spin)) {
                    emitterDataSize += sizeof(BinFieldSpinData);
                }
                if (emitter->fieldFlags().isSet(FieldFlag::Collision)) {
                    emitterDataSize += sizeof(BinFieldCollisionData);
                }
                if (emitter->fieldFlags().isSet(FieldFlag::Convergence)) {
                    emitterDataSize += sizeof(BinFieldConvergenceData);
                }
                if (emitter->fieldFlags().isSet(FieldFlag::PosAdd)) {
                    emitterDataSize += sizeof(BinFieldPosAddData);
                }

                // FluctuationData
                if (emitter->fluctuationFlags().isSet(FluctuationFlag::Enabled)) {
                    emitterData.fluctuationDataOffset = emitterDataSize;
                    emitterDataSize += sizeof(BinFluctuationData);
                }

                // StripeData
                // TODO: Should this be checking billboard type?
                if (emitter->hasStripeData()) {
                    emitterData.stripeDataOffset = emitterDataSize;
                    emitterDataSize += sizeof(BinStripeData);
                }

                // Append to list
                emitterData.mDataSize = emitterDataSize;

                emitterDataCurOffset += emitterDataSize;
                binEmitterDataList.emplace_back(emitterData);

                if (emitterData.childFlag.isSet(ChildFlag::Enabled)) {
                    binEmitterDataList.emplace_back(binChildData);
                }

                if (emitter->fieldFlags().isSet(FieldFlag::Random)) {
                    binEmitterDataList.emplace_back(binRandomData);
                }
                if (emitter->fieldFlags().isSet(FieldFlag::Magnet)) {
                    binEmitterDataList.emplace_back(binMagnetData);
                }
                if (emitter->fieldFlags().isSet(FieldFlag::Spin)) {
                    binEmitterDataList.emplace_back(binSpinData);
                }
                if (emitter->fieldFlags().isSet(FieldFlag::Collision)) {
                    binEmitterDataList.emplace_back(binCollisionData);
                }
                if (emitter->fieldFlags().isSet(FieldFlag::Convergence)) {
                    binEmitterDataList.emplace_back(binConvergenceData);
                }
                if (emitter->fieldFlags().isSet(FieldFlag::PosAdd)) {
                    binEmitterDataList.emplace_back(binPosAddData);
                }

                if (emitter->fluctuationFlags().isSet(FluctuationFlag::Enabled)) {
                    binEmitterDataList.emplace_back(binFluctuationData);
                }

                if (emitter->hasStripeData()) {
                    binEmitterDataList.emplace_back(binStripeData);
                }
            }
        }

        // Increment current offset
        emitterSetsCurOffset += sizeof(BinEmitterSetData);
        binEmitterSetsList.emplace_back(binEmitterSet);
    }

    // TODO: Build texture table

    // Write to the file
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    stream << headerData;

    for (auto& emtSet : binEmitterSetsList) {
        stream << emtSet;
    }

    for (auto& emtTbl : binEmitterTblsList) {
        stream << emtTbl;
    }

    for (auto& emtData : binEmitterDataList) {
        std::visit([&stream](auto& dataBlock) {
            stream << dataBlock;
        }, emtData);
    }

    // Align texture data to 128 bytes
    qint64 paddingNeeded = (128 - (file.pos() % 128)) % 128;

    if (paddingNeeded > 0) {
        QByteArray padding(paddingNeeded, '\0');
        stream.writeRawData(padding.constData(), padding.size());
    }

    qint64 textureTblPos = file.pos();
    stream.writeRawData(textureTbl.data(), textureTbl.size());
    qint64 textureTblSize = file.pos() - textureTblPos;

    qint64 nameTblPos = file.pos();
    stream.writeRawData(nameTbl.data(), nameTbl.size());

    // Update header with nameTbl/textureTbl pos & size
    headerData.textureTblPos = textureTblPos;
    headerData.textureTblSize = textureTblSize;
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
    static_assert("Not implemented: PtclRes::addTexture");
}

void PtclRes::removeTexture(u32 textureIndex) {
    static_assert("Not implemented: PtclRes::removeTexture");
}

const TextureList& PtclRes::textures() const {

    return mTextures;
}


TextureList& PtclRes::textures() {

    return mTextures;
}

} // namespace Ptcl
