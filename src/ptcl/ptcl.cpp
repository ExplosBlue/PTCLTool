#include "ptcl/ptcl.h"
#include "ptcl/ptclBinary.h"
#include "util/stringUtil.h"

#include <QDataStream>
#include <QFile>

#include <variant>


namespace Ptcl {


// ========================================================================== //


PtclBinaryReader::PtclBinaryReader(const QString& filePath) :
    mFile{filePath}, mStream{&mFile} {

    if (!mFile.open(QIODevice::ReadOnly)) {
        throw("PtclBinaryReader - Failed to open file.");
    }

    mStream.setByteOrder(QDataStream::LittleEndian);
    mStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
}

void PtclBinaryReader::readHeader() {
    mStream >> mHeaderData;

    mEmitterSetTblPos = mFile.pos();

    mFile.seek(mHeaderData.nameTblPos);
    mNameTbl = mFile.readAll();

    mFile.seek(mHeaderData.textureTblPos);
    mTextureTblPos = mFile.pos();
}

QString PtclBinaryReader::readProjectName() {
    return readName(mHeaderData.namePos);
}

EmitterSetList PtclBinaryReader::readEmitterSets() {
    EmitterSetList setList{};

    setList.reserve(mHeaderData.numEmitterSet);
    for (s32 idx = 0; idx < mHeaderData.numEmitterSet; idx++) {
        setList.push_back(readEmitterSet(idx));
    }

    return std::move(setList);
}

std::shared_ptr<Texture> PtclBinaryReader::loadTexture(u32 texturePos, u32 size, u32 width, u32 height, TextureFormat format) {
    const u32 offset = mTextureTblPos + texturePos;

    auto it = mTextureOffsetMap.find(offset);
    if (it != mTextureOffsetMap.end()) {
        return mTextures[it->second];
    }

    mFile.seek(offset);

    std::vector<u8> textureData(size);
    const s64 bytesRead = mStream.readRawData(reinterpret_cast<char*>(textureData.data()), size);

    if (bytesRead != static_cast<s64>(size)) {
        qWarning() << "Expected to read" << size << "bytes, got" << bytesRead << "bytes.";
    }

    auto texture = std::make_shared<Texture>(&textureData, width, height, format);

    const u32 idx = static_cast<u32>(mTextures.size());
    mTextures.push_back(texture);
    mTextureOffsetMap.emplace(offset, idx);

    return texture;
}

std::unique_ptr<EmitterSet> PtclBinaryReader::readEmitterSet(s32 index) {
    mFile.seek(mEmitterSetTblPos + index * static_cast<s64>((sizeof(BinEmitterSetData))));

    BinEmitterSetData setData{};
    mStream >> setData;

    auto set = std::make_unique<EmitterSet>();
    set->setName(readName(setData.namePos));

    set->setUserData(setData.userData);
    set->setLastUpdateDate(setData.lastUpdateDate);

    set->emitters().reserve(setData.numEmitter);

    for (s32 idx = 0; idx < setData.numEmitter; ++idx) {
        mFile.seek(setData.emitterTblPos + idx * static_cast<s64>((sizeof(BinEmitterTblData))));

        BinEmitterTblData tblData;
        mStream >> tblData;

        auto emitter = std::make_unique<Emitter>();
        if (tblData.emitterPos <= 0) {
            set->emitters().push_back(std::move(emitter));
            continue;
        }

        mFile.seek(tblData.emitterPos);

        BinCommonEmitterData commonData{};
        mStream >> commonData;

        emitter->initFromBinary(commonData);
        emitter->setName(readName(commonData.namePos));

        emitter->setTexture(
            loadTexture(
                commonData.texturePos,
                commonData.textureSize,
                commonData.textureRes.width,
                commonData.textureRes.height,
                commonData.textureRes.format
            )
        );

        if (commonData.type == EmitterType::Complex || commonData.type == EmitterType::Compact) {
            mFile.seek(tblData.emitterPos + static_cast<qint64>(sizeof(BinCommonEmitterData)));
            readComplexData(*emitter, commonData);
        }

        set->emitters().push_back(std::move(emitter));
    }
    return set;
}

void PtclBinaryReader::readComplexData(Emitter& emitter, const BinCommonEmitterData& common) {
    BinComplexEmitterData complex{};
    mStream >> complex;

    emitter.initComplexFromBinary(complex);

    // ChildData
    if (complex.childFlag.isSet(ChildFlag::Enabled)) {
        BinChildData childData{};
        mStream >> childData;
        emitter.childData().initFromBinary(childData);

        emitter.childData().setTexture(
            loadTexture(
                childData.childTexturePos,
                childData.childTextureSize,
                childData.childTextureRes.width,
                childData.childTextureRes.height,
                childData.childTextureRes.format
            )
        );
    }

    // FieldData
    Ptcl::FieldData fieldData;
    if (complex.fieldFlag.isSet(FieldFlag::Random)) {
        BinFieldRandomData randomData{};
        mStream >> randomData;
        fieldData.initRandomData(randomData);
    }
    if (complex.fieldFlag.isSet(FieldFlag::Magnet)) {
        BinFieldMagnetData magnetData{};
        mStream >> magnetData;
        fieldData.initMagnetData(magnetData);
    }
    if (complex.fieldFlag.isSet(FieldFlag::Spin)) {
        BinFieldSpinData spinData{};
        mStream >> spinData;
        fieldData.initSpinData(spinData);
    }
    if (complex.fieldFlag.isSet(FieldFlag::Collision)) {
        BinFieldCollisionData collisionData{};
        mStream >> collisionData;
        fieldData.initCollisionData(collisionData);
    }
    if (complex.fieldFlag.isSet(FieldFlag::Convergence)) {
        BinFieldConvergenceData convergenceData{};
        mStream >> convergenceData;
        fieldData.initConvergenceData(convergenceData);
    }
    if (complex.fieldFlag.isSet(FieldFlag::PosAdd)) {
        BinFieldPosAddData posAddData{};
        mStream >> posAddData;
        fieldData.initPosAddData(posAddData);
    }
    emitter.setFieldData(fieldData);

    // FluctuationData
    if (complex.fluctuationFlag.isSet(FluctuationFlag::Enabled)) {
        BinFluctuationData fluctuationData{};
        mStream >> fluctuationData;
        emitter.initFluctuationData(fluctuationData);
    }

    // StripeData
    const auto billboardType = common.billboardType;
    if (billboardType == BillboardType::Stripe || billboardType == BillboardType::ComplexStripe) {
        BinStripeData stripeData{};
        mStream >> stripeData;
        emitter.initStripeData(stripeData);
    }
}

TextureList PtclBinaryReader::takeTextures() {
    return std::move(mTextures);
}

QString PtclBinaryReader::readName(u32 namePos) {
    return StringUtil::shiftJISToQString(mNameTbl.data() + namePos);
}

// ========================================================================== //


bool PtclRes::load(const QString& filePath) {
    PtclBinaryReader reader(filePath);

    reader.readHeader();
    mName = reader.readProjectName();
    mEmitterSets = reader.readEmitterSets();
    mTextures = reader.takeTextures();

    return true;
}

u32 PtclRes::emitterSetCount() const {
    return mEmitterSets.size();
}

u32 PtclRes::emitterCount() const {
    u32 count = 0;

    for (auto& emitterSet : mEmitterSets) {
        count += emitterSet->emitterCount();
    }
    return count;
}

bool PtclRes::save(const QString& filePath) {
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
        return false;
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
            .numEmitter     = static_cast<u32>(emitterSet->emitterCount()),
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
                    qint64 dataSize = static_cast<qint64>(emitter->textureHandle()->textureDataRaw().size());
                    qint64 paddingNeeded = (128 - (dataSize % 128)) % 128;

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

            } else if (emitter->type() == Ptcl::EmitterType::Complex || emitter->type() == Ptcl::EmitterType::Compact) {

                // Create Complex EmitterData
                BinComplexEmitterData emitterData(*emitter.get());

                emitterData.namePos = nameTblCurOffset;
                appendToNameTbl(emitter->name());

                u32 emitterDataSize = sizeof(BinComplexEmitterData);

                BinChildData binChildData(emitter->childData());

                const auto& fieldData = emitter->fieldData();

                BinFieldRandomData binRandomData(fieldData.randomData());
                BinFieldMagnetData binMagnetData(fieldData.magnetData());
                BinFieldSpinData binSpinData(fieldData.spinData());
                BinFieldCollisionData binCollisionData(fieldData.collisionData());
                BinFieldConvergenceData binConvergenceData(fieldData.convergenceData());
                BinFieldPosAddData binPosAddData(fieldData.posAddData());

                BinFluctuationData binFluctuationData(emitter->fluctuationData());

                BinStripeData binStripeData(emitter->stripeData());

                emitterData.childDataOffset = emitterDataSize;

                // ChildData
                if (emitterData.childFlag.isSet(ChildFlag::Enabled)) {
                    emitterDataSize += sizeof(BinChildData);

                    // Store texture data
                    if (textureOffsetMap.contains(emitter->childData().textureHandle()->Id())) {
                        binChildData.childTexturePos = textureOffsetMap[emitter->childData().textureHandle()->Id()];
                        binChildData.childTextureSize = emitter->childData().textureHandle()->textureDataRaw().size();
                    } else {
                        qint64 dataSize = static_cast<qint64>(emitter->childData().textureHandle()->textureDataRaw().size());
                        qint64 paddingNeeded = (128 - (dataSize % 128)) % 128;

                        binChildData.childTexturePos = textureTblCurOffset;
                        textureOffsetMap.try_emplace(emitter->childData().textureHandle()->Id(), textureTblCurOffset);
                        appendToTextureTbl(emitter->childData().textureHandle()->textureDataRaw());

                        if (paddingNeeded > 0) {
                            QByteArray padding(paddingNeeded, '\0');
                            textureTbl.insert(textureTbl.end(), padding.begin(), padding.end());
                        }

                        binChildData.childTextureSize = emitter->childData().textureHandle()->textureDataRaw().size();
                        textureTblCurOffset += binChildData.childTextureSize + paddingNeeded;
                    }
                }

                // Store texture data
                if (textureOffsetMap.contains(emitter->textureHandle()->Id())) {
                    emitterData.texturePos = textureOffsetMap[emitter->textureHandle()->Id()];
                    emitterData.textureSize = emitter->textureHandle()->textureDataRaw().size();
                } else {
                    qint64 dataSize = static_cast<qint64>(emitter->textureHandle()->textureDataRaw().size());
                    qint64 paddingNeeded = (128 - (dataSize % 128)) % 128;

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

                emitterData.fieldDataOffset = emitterDataSize;

                // FieldData
                if (emitter->complexProperties().fieldFlags.isSet(FieldFlag::Random)) {
                    emitterDataSize += sizeof(BinFieldRandomData);
                }
                if (emitter->complexProperties().fieldFlags.isSet(FieldFlag::Magnet)) {
                    emitterDataSize += sizeof(BinFieldMagnetData);
                }
                if (emitter->complexProperties().fieldFlags.isSet(FieldFlag::Spin)) {
                    emitterDataSize += sizeof(BinFieldSpinData);
                }
                if (emitter->complexProperties().fieldFlags.isSet(FieldFlag::Collision)) {
                    emitterDataSize += sizeof(BinFieldCollisionData);
                }
                if (emitter->complexProperties().fieldFlags.isSet(FieldFlag::Convergence)) {
                    emitterDataSize += sizeof(BinFieldConvergenceData);
                }
                if (emitter->complexProperties().fieldFlags.isSet(FieldFlag::PosAdd)) {
                    emitterDataSize += sizeof(BinFieldPosAddData);
                }

                // FluctuationData
                if (emitter->complexProperties().fluctuationFlags.isSet(FluctuationFlag::Enabled)) {
                    emitterData.fluctuationDataOffset = emitterDataSize;
                    emitterDataSize += sizeof(BinFluctuationData);
                }

                // StripeData
                if (emitter->hasStripeData()) {
                    emitterData.stripeDataOffset = emitterDataSize;
                    emitterDataSize += sizeof(BinStripeData);
                }

                // Append to list
                emitterData.mDataSize = static_cast<s32>(emitterDataSize);

                emitterDataCurOffset += emitterDataSize;
                binEmitterDataList.emplace_back(emitterData);

                if (emitterData.childFlag.isSet(ChildFlag::Enabled)) {
                    binEmitterDataList.emplace_back(binChildData);
                }

                if (emitter->complexProperties().fieldFlags.isSet(FieldFlag::Random)) {
                    binEmitterDataList.emplace_back(binRandomData);
                }
                if (emitter->complexProperties().fieldFlags.isSet(FieldFlag::Magnet)) {
                    binEmitterDataList.emplace_back(binMagnetData);
                }
                if (emitter->complexProperties().fieldFlags.isSet(FieldFlag::Spin)) {
                    binEmitterDataList.emplace_back(binSpinData);
                }
                if (emitter->complexProperties().fieldFlags.isSet(FieldFlag::Collision)) {
                    binEmitterDataList.emplace_back(binCollisionData);
                }
                if (emitter->complexProperties().fieldFlags.isSet(FieldFlag::Convergence)) {
                    binEmitterDataList.emplace_back(binConvergenceData);
                }
                if (emitter->complexProperties().fieldFlags.isSet(FieldFlag::PosAdd)) {
                    binEmitterDataList.emplace_back(binPosAddData);
                }

                if (emitter->complexProperties().fluctuationFlags.isSet(FluctuationFlag::Enabled)) {
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
    stream.writeRawData(textureTbl.data(), static_cast<qint64>(textureTbl.size()));
    qint64 textureTblSize = file.pos() - textureTblPos;

    qint64 nameTblPos = file.pos();
    stream.writeRawData(nameTbl.data(), static_cast<qint64>(nameTbl.size()));

    // Update header with nameTbl/textureTbl pos & size
    headerData.textureTblPos = textureTblPos;
    headerData.textureTblSize = textureTblSize;
    headerData.nameTblPos = nameTblPos;

    file.seek(0);
    stream << headerData; // would be better to avoid doing this twice...

    file.flush();
    file.close();

    return true;
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

const TextureList& PtclRes::textures() const {
    return mTextures;
}

TextureList& PtclRes::textures() {
    return mTextures;
}

void PtclRes::addNewEmitterSet() {
    auto newSet = std::make_unique<Ptcl::EmitterSet>();
    newSet->setName("New_EmitterSet_" + QString::number(emitterSetCount()));
    newSet->addNewEmitter();
    mEmitterSets.push_back(std::move(newSet));
}

void PtclRes::removeEmitterSet(s32 setIndex) {
    if (setIndex >= mEmitterSets.size()) {
        return;
    }

    mEmitterSets.erase(mEmitterSets.begin() + setIndex);
}

const std::unique_ptr<EmitterSet>& PtclRes::appendEmitterSet(std::unique_ptr<EmitterSet>& newSet) {
    mEmitterSets.push_back(std::move(newSet));
    return mEmitterSets.at(emitterSetCount() - 1);
}


// ========================================================================== //


} // namespace Ptcl
