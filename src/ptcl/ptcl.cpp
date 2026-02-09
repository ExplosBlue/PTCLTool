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


PtclBinaryWriter::PtclBinaryWriter(const QString& filePath) :
    mFile(filePath), mStream(&mFile) {
    if (!mFile.open(QIODevice::WriteOnly)) {
        throw("PtclBinaryWriter - Failed to open file.");
    }

    mStream.setByteOrder(QDataStream::LittleEndian);
    mStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
}

u32 PtclBinaryWriter::appendName(const QString& name) {
    const u32 offset = mNameTblCurOffset;

    auto sjis = StringUtil::qStringToShiftJIS(name);
    mNameTbl.insert(mNameTbl.end(), sjis.begin(), sjis.end());
    mNameTbl.push_back('\0');

    mNameTblCurOffset += sjis.size() + 1;
    return offset;
}

u32 PtclBinaryWriter::appendTexture(const TextureHandle& texture, u32& outSize) {
    const u32 id = texture->Id();

    if (auto it = mTextureOffsetMap.find(id); it != mTextureOffsetMap.end()) {
        outSize = texture->textureDataRaw().size();
        return it->second;
    }

    const auto& data = texture->textureDataRaw();
    const u32 offset = mTextureTblCurOffset;
    const u32 size = static_cast<u32>(data.size());

    mTextureOffsetMap.try_emplace(id, offset);
    mTextureTbl.insert(mTextureTbl.end(), data.begin(), data.end());

    const u32 padding = (128 - (size % 128)) % 128;
    mTextureTbl.insert(mTextureTbl.end(), padding, '\0');

    mTextureTblCurOffset += size + padding;
    outSize = size;
    return offset;
}

void PtclBinaryWriter::buildHeader(const PtclRes& res) {
    constexpr u32 headerBasePos = 0;
    constexpr u32 emitterSetsBasePos = headerBasePos + sizeof(BinHeaderData);
    const u32 emitterTblDataBasePos = emitterSetsBasePos + (res.emitterSetCount() * sizeof(BinEmitterSetData));
    const u32 emitterDataBasePos = emitterTblDataBasePos + (res.emitterCount() * sizeof(BinEmitterTblData));

    mEmitterSetsCurOffset = emitterSetsBasePos;
    mEmitterTblCurOffset = emitterTblDataBasePos;
    mEmitterDataCurOffset = emitterDataBasePos;
    mNameTblCurOffset = 0;
    mTextureTblCurOffset = 0;

     mHeader = {
        .magic          = {'S', 'P', 'B', 'D'},
        .version        = 11,
        .numEmitterSet  = res.emitterSetCount(),
        .namePos        = appendName(res.name()),
        .nameTblPos     = 0,
        .textureTblPos  = 0,
        .textureTblSize = 0
    };
}

void PtclBinaryWriter::buildEmitters(const PtclRes& res) {
    for (const auto& set : res.getEmitterSets()) {

        BinEmitterSetData setData{
            .userData       = set->userData(),
            .lastUpdateDate = set->lastUpdateDate(),
            .namePos        = appendName(set->name()),
            .namePtr        = 0,
            .numEmitter     = static_cast<u32>(set->emitterCount()),
            .emitterTblPos  = mEmitterTblCurOffset,
            .emitterTbl     = 0
        };

        for (const auto& emitter : set->emitters()) {
            mEmitterTbls.push_back({
                .emitterPos = mEmitterDataCurOffset,
                .emitterPtr = 0
            });
            mEmitterTblCurOffset += sizeof(BinEmitterTblData);

            if (emitter->type() == Ptcl::EmitterType::Simple) {
                writeSimpleEmitter(*emitter);
            } else {
                qDebug() << "Writing Complex Data";
                writeComplexEmitter(*emitter);
            }
        }

        mEmitterSets.emplace_back(setData);
        mEmitterSetsCurOffset += sizeof(BinEmitterSetData);
    }
}

void PtclBinaryWriter::writeSimpleEmitter(const Emitter& emitter) {
    BinCommonEmitterData emitterData(emitter);
    emitterData.namePos = appendName(emitter.name());
    emitterData.texturePos = appendTexture(emitter.textureHandle(), emitterData.textureSize);

    mEmitterDataCurOffset += sizeof(BinCommonEmitterData);
    mEmitterData.emplace_back(emitterData);
}

void PtclBinaryWriter::writeComplexEmitter(const Emitter& emitter) {
    BinComplexEmitterData emitterData(emitter);
    emitterData.namePos = appendName(emitter.name());
    u32 emitterDataSize = sizeof(BinComplexEmitterData);

    // Child
    const bool hasChild = emitterData.childFlag.isSet(ChildFlag::Enabled);

    BinChildData childData(emitter.childData());
    emitterData.childDataOffset = emitterDataSize;

    if (hasChild) {
        emitterDataSize += sizeof(BinChildData);
        childData.childTexturePos = appendTexture(emitter.childData().textureHandle(), childData.childTextureSize);
    }

    // Main Texture
    emitterData.texturePos = appendTexture(emitter.textureHandle(), emitterData.textureSize);

    // Field
    emitterData.fieldDataOffset = emitterDataSize;

    const auto& fieldFlags = emitter.complexProperties().fieldFlags;

    const bool hasRandom      = fieldFlags.isSet(FieldFlag::Random);
    const bool hasMagnet      = fieldFlags.isSet(FieldFlag::Magnet);
    const bool hasSpin        = fieldFlags.isSet(FieldFlag::Spin);
    const bool hasCollision   = fieldFlags.isSet(FieldFlag::Collision);
    const bool hasConvergence = fieldFlags.isSet(FieldFlag::Convergence);
    const bool hasPosAdd      = fieldFlags.isSet(FieldFlag::PosAdd);

    if (hasRandom)      { emitterDataSize += sizeof(BinFieldRandomData); }
    if (hasMagnet)      { emitterDataSize += sizeof(BinFieldMagnetData); }
    if (hasSpin)        { emitterDataSize += sizeof(BinFieldSpinData); }
    if (hasCollision)   { emitterDataSize += sizeof(BinFieldCollisionData); }
    if (hasConvergence) { emitterDataSize += sizeof(BinFieldConvergenceData); }
    if (hasPosAdd)      { emitterDataSize += sizeof(BinFieldPosAddData); }

    // Fluctuation
    const bool hasFluctuation = emitter.complexProperties().fluctuationFlags.isSet(FluctuationFlag::Enabled);

    if (hasFluctuation) {
        emitterData.fluctuationDataOffset = emitterDataSize;
        emitterDataSize += sizeof(BinFluctuationData);
    }

    // Stripe
    const bool hasStripe = emitter.hasStripeData();

    if (hasStripe) {
        emitterData.stripeDataOffset = emitterDataSize;
        emitterDataSize += sizeof(BinStripeData);
    }

    // Main Block
    emitterData.mDataSize = static_cast<s32>(emitterDataSize);
    mEmitterData.emplace_back(emitterData);
    mEmitterDataCurOffset += emitterDataSize;

    // Sub-Blocks
    if (hasChild)       { mEmitterData.emplace_back(childData); }
    if (hasRandom)      { mEmitterData.emplace_back(BinFieldRandomData{emitter.fieldData().randomData()}); }
    if (hasMagnet)      { mEmitterData.emplace_back(BinFieldMagnetData{emitter.fieldData().magnetData()}); }
    if (hasSpin)        { mEmitterData.emplace_back(BinFieldSpinData{emitter.fieldData().spinData()}); }
    if (hasCollision)   { mEmitterData.emplace_back(BinFieldCollisionData{emitter.fieldData().collisionData()}); }
    if (hasConvergence) { mEmitterData.emplace_back(BinFieldConvergenceData{emitter.fieldData().convergenceData()}); }
    if (hasPosAdd)      { mEmitterData.emplace_back(BinFieldPosAddData{emitter.fieldData().posAddData()}); }
    if (hasFluctuation) { mEmitterData.emplace_back(BinFluctuationData{emitter.fluctuationData()}); }
    if (hasStripe)      { mEmitterData.emplace_back(BinStripeData{emitter.stripeData()}); }
}

void PtclBinaryWriter::writeFile() {
    mStream << mHeader;

    for (auto& set : mEmitterSets) { mStream << set; }
    for (auto& tbl : mEmitterTbls) { mStream << tbl; }

    for (auto& block : mEmitterData) {
        std::visit([this](auto& data) { mStream << data; }, block);
    }

    const s64 padding = (128 - (mFile.pos() % 128)) % 128;
    if (padding) {
        QByteArray pad(padding, '\0');
        mStream.writeRawData(pad.constData(), pad.size());
    }

    mHeader.textureTblPos = mFile.pos();
    mStream.writeRawData(mTextureTbl.data(), static_cast<s64>(mTextureTbl.size()));
    mHeader.textureTblSize = mFile.pos() - mHeader.textureTblPos;

    mHeader.nameTblPos = mFile.pos();
    mStream.writeRawData(mNameTbl.data(), static_cast<s64>(mNameTbl.size()));

    mFile.seek(0);
    mStream << mHeader;
}

void PtclBinaryWriter::write(const PtclRes& res) {
    buildHeader(res);
    buildEmitters(res);
    writeFile();
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
    PtclBinaryWriter writer(filePath);
    writer.write(*this);
    return true;
}


const QString& PtclRes::name() const {
    return mName;
}


void PtclRes::setName(const QString& name) {
    mName = name;
}

const EmitterSetList& PtclRes::getEmitterSets() const {
    return mEmitterSets;
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
