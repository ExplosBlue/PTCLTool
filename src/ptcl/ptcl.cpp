#include "ptcl/ptcl.h"
#include "ptcl/ptclBinary.h"
#include "ptcl/ptclValidator.h"
#include "ptcl/ptcljson.h"
#include "util/imageUtil.h"
#include "util/stringUtil.h"

#include <QDataStream>
#include <QFile>

#include <algorithm>
#include <bit>
#include <stdexcept>
#include <utility>
#include <variant>


namespace Ptcl {


// ========================================================================== //


PtclBinaryReader::PtclBinaryReader(const QString& filePath) :
    mFile{filePath}, mStream{&mFile} {

    if (!mFile.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("PtclBinaryReader - Failed to open file.");
    }

    mStream.setByteOrder(QDataStream::LittleEndian);
    mStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
}

PtclReadResult PtclBinaryReader::readAll() {
    readHeader();

    PtclReadResult result;
    result.name = readName(mHeaderData.namePos);
    result.emitterSets = readEmitterSets();
    result.textures = takeTextures();

    return result;
}

void PtclBinaryReader::readHeader() {
    mStream >> mHeaderData;

    constexpr std::array<char, 4> expectedMagic{'S', 'P', 'B', 'D'};
    if (mHeaderData.magic != expectedMagic) {
        throw std::runtime_error("PtclBinaryReader - Invalid file: bad magic.");
    }

    mEmitterSetTblPos = mFile.pos();

    if (mHeaderData.nameTblPos == 0 || mHeaderData.nameTblPos >= mFile.size()) {
        throw std::runtime_error("PtclBinaryReader - Invalid file: name table out of bounds.");
    }

    mFile.seek(mHeaderData.nameTblPos);
    mNameTbl = mFile.readAll();

    if (mHeaderData.textureTblPos >= mFile.size()) {
        throw std::runtime_error("PtclBinaryReader - Invalid file: texture table out of bounds.");
    }

    mFile.seek(mHeaderData.textureTblPos);
    mTextureTblPos = mFile.pos();
}

std::vector<RawEmitterSetRecord> PtclBinaryReader::readEmitterSets() {
    std::vector<RawEmitterSetRecord> setList{};

    const s64 maxSets = (mFile.size() - mEmitterSetTblPos) / static_cast<s64>(sizeof(BinEmitterSetData));
    const u32 numSets = std::min(mHeaderData.numEmitterSet, static_cast<u32>(std::max<s64>(maxSets, 0)));

    setList.reserve(numSets);
    for (u32 idx = 0; idx < numSets; idx++) {
        setList.push_back(readEmitterSet(static_cast<s32>(idx)));
    }

    return setList;
}

s32 PtclBinaryReader::loadTexture(u32 texturePos, u32 size, u32 width, u32 height, TextureFormat format) {
    if (size > ImageUtil::maxTextureBytes()) {
        throw std::runtime_error("PtclBinaryReader - Texture data size too large.");
    }

    const u32 offset = mTextureTblPos + texturePos;

    auto it = mTextureOffsetMap.find(offset);
    if (it != mTextureOffsetMap.end()) {
        return static_cast<s32>(it->second);
    }

    mFile.seek(offset);

    std::vector<u8> textureData(size);
    const s64 bytesRead = mStream.readRawData(std::bit_cast<char*>(textureData.data()), size);

    if (bytesRead != static_cast<s64>(size)) {
        qWarning() << "Expected to read" << size << "bytes, got" << bytesRead << "bytes.";
    }

    RawTextureData rawTexture{
        .pos = texturePos,
        .size = size,
        .width = width,
        .height = height,
        .format = format,
        .bytes = std::move(textureData)
    };

    const u32 idx = static_cast<u32>(mTextures.size());
    mTextures.push_back(std::move(rawTexture));
    mTextureOffsetMap.emplace(offset, idx);

    return static_cast<s32>(idx);
}

RawEmitterSetRecord PtclBinaryReader::readEmitterSet(s32 index) {
    mFile.seek(mEmitterSetTblPos + index * static_cast<s64>((sizeof(BinEmitterSetData))));

    BinEmitterSetData setData{};
    mStream >> setData;

    RawEmitterSetRecord set;
    set.name = readName(setData.namePos);
    set.data = setData;

    const s64 maxEmitters = (mFile.size() - static_cast<s64>(setData.emitterTblPos)) / static_cast<s64>(sizeof(BinEmitterTblData));
    const u32 numEmitters = std::min(setData.numEmitter, static_cast<u32>(std::max<s64>(maxEmitters, 0)));

    set.emitters.reserve(numEmitters);

    for (u32 idx = 0; idx < numEmitters; ++idx) {
        mFile.seek(setData.emitterTblPos + idx * static_cast<s64>((sizeof(BinEmitterTblData))));

        BinEmitterTblData tblData;
        mStream >> tblData;

        RawEmitterRecord emitter;
        if (tblData.emitterPos <= 0) {
            emitter.isNull = true;
            set.emitters.push_back(std::move(emitter));
            continue;
        }

        mFile.seek(tblData.emitterPos);

        BinCommonEmitterData commonData{};
        mStream >> commonData;

        emitter.name = readName(commonData.namePos);
        emitter.common = commonData;
        emitter.textureIndex = loadTexture(
            commonData.texturePos,
            commonData.textureSize,
            commonData.textureRes.width,
            commonData.textureRes.height,
            commonData.textureRes.format
        );

        if (commonData.type == EmitterType::Complex || commonData.type == EmitterType::Compact) {
            mFile.seek(tblData.emitterPos + static_cast<qint64>(sizeof(BinCommonEmitterData)));
            readComplexData(emitter, commonData);
        }

        set.emitters.push_back(std::move(emitter));
    }
    return set;
}

void PtclBinaryReader::readComplexData(RawEmitterRecord& record, const BinCommonEmitterData& common) {
    BinComplexEmitterData complex{};
    mStream >> complex;

    record.complex = complex;

    // ChildData
    if (complex.childFlag.isSet(ChildFlag::Enabled)) {
        BinChildData childData{};
        mStream >> childData;

        record.child = childData;
        record.childTextureIndex = loadTexture(
            childData.childTexturePos,
            childData.childTextureSize,
            childData.childTextureRes.width,
            childData.childTextureRes.height,
            childData.childTextureRes.format
        );
    }

    // FieldData
    if (complex.fieldFlag.isSet(FieldFlag::Random)) {
        BinFieldRandomData randomData{};
        mStream >> randomData;
        record.fieldRandom = randomData;
    }
    if (complex.fieldFlag.isSet(FieldFlag::Magnet)) {
        BinFieldMagnetData magnetData{};
        mStream >> magnetData;
        record.fieldMagnet = magnetData;
    }
    if (complex.fieldFlag.isSet(FieldFlag::Spin)) {
        BinFieldSpinData spinData{};
        mStream >> spinData;
        record.fieldSpin = spinData;
    }
    if (complex.fieldFlag.isSet(FieldFlag::Collision)) {
        BinFieldCollisionData collisionData{};
        mStream >> collisionData;
        record.fieldCollision = collisionData;
    }
    if (complex.fieldFlag.isSet(FieldFlag::Convergence)) {
        BinFieldConvergenceData convergenceData{};
        mStream >> convergenceData;
        record.fieldConvergence = convergenceData;
    }
    if (complex.fieldFlag.isSet(FieldFlag::PosAdd)) {
        BinFieldPosAddData posAddData{};
        mStream >> posAddData;
        record.fieldPosAdd = posAddData;
    }

    // FluctuationData
    if (complex.fluctuationFlag.isSet(FluctuationFlag::Enabled)) {
        BinFluctuationData fluctuationData{};
        mStream >> fluctuationData;
        record.fluctuation = fluctuationData;
    }

    // StripeData
    const auto billboardType = common.billboardType;
    if (billboardType == BillboardType::Stripe || billboardType == BillboardType::ComplexStripe) {
        BinStripeData stripeData{};
        mStream >> stripeData;
        record.stripe = stripeData;
    }
}

std::vector<RawTextureData> PtclBinaryReader::takeTextures() {
    return std::move(mTextures);
}

QString PtclBinaryReader::readName(u32 namePos) {
    if (mNameTbl.isEmpty() || namePos >= static_cast<u32>(mNameTbl.size())) {
        return {};
    }

    return StringUtil::shiftJISToQString(mNameTbl.constData() + namePos);
}


// ========================================================================== //


PtclBinaryWriter::PtclBinaryWriter(const QString& filePath) :
    mFile(filePath), mStream(&mFile) {
    if (!mFile.open(QIODevice::WriteOnly)) {
        throw std::runtime_error("PtclBinaryWriter - Failed to open file.");
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
    const u32 emitterDataBasePos = emitterTblDataBasePos + (res.totalEmitterCount() * sizeof(BinEmitterTblData));

    mEmitterSetsCurOffset = emitterSetsBasePos;
    mEmitterTblCurOffset = emitterTblDataBasePos;
    mEmitterDataCurOffset = emitterDataBasePos;
    mNameTblCurOffset = 0;
    mTextureTblCurOffset = 0;

     mHeader = {
        .magic          = {'S', 'P', 'B', 'D'},
        .version        = 11,
        .numEmitterSet  = static_cast<u32>(res.emitterSetCount()),
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

    BinChildData childData(emitter);
    emitterData.childDataOffset = emitterDataSize;

    if (hasChild) {
        emitterDataSize += sizeof(BinChildData);
        childData.childTexturePos = appendTexture(emitter.childTextureHandle(), childData.childTextureSize);
    }

    // Main Texture
    emitterData.texturePos = appendTexture(emitter.textureHandle(), emitterData.textureSize);

    // Field
    emitterData.fieldDataOffset = emitterDataSize;

    const auto& fieldFlags = emitter.fieldFlags();

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
    const bool hasFluctuation = emitter.fluctuationFlags().isSet(FluctuationFlag::Enabled);

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
    if (hasRandom)      { mEmitterData.emplace_back(BinFieldRandomData{emitter}); }
    if (hasMagnet)      { mEmitterData.emplace_back(BinFieldMagnetData{emitter}); }
    if (hasSpin)        { mEmitterData.emplace_back(BinFieldSpinData{emitter}); }
    if (hasCollision)   { mEmitterData.emplace_back(BinFieldCollisionData{emitter}); }
    if (hasConvergence) { mEmitterData.emplace_back(BinFieldConvergenceData{emitter}); }
    if (hasPosAdd)      { mEmitterData.emplace_back(BinFieldPosAddData{emitter}); }
    if (hasFluctuation) { mEmitterData.emplace_back(BinFluctuationData{emitter}); }
    if (hasStripe)      { mEmitterData.emplace_back(BinStripeData{emitter}); }
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
    mSanitizeReport = PtclSanitizeReport{};

    try {
        if (filePath.endsWith(".ptclproj")) {
            mName.clear();
            mEmitterSets.clear();
            mTextures.clear();
            return PtclJson::importProject(filePath, *this, mSanitizeReport);
        }

        PtclBinaryReader reader(filePath);
        PtclReadResult result = reader.readAll();
        const QString name = std::move(result.name);

        PtclValidator validator;
        PtclValidationResult validated = validator.validate(std::move(result));

        mName = name;
        mEmitterSets = std::move(validated.emitterSets);
        mTextures = std::move(validated.textures);
        mSanitizeReport = std::move(validated.report);
        return true;
    } catch (const std::exception& ex) {
        qWarning() << "Failed to load PTCL file" << filePath << ":" << ex.what();
    } catch (...) {
        qWarning() << "Failed to load PTCL file" << filePath << ": unknown error.";
    }

    return false;
}

bool PtclRes::save(const QString& filePath) {
    PtclBinaryWriter writer(filePath);
    writer.write(*this);
    return true;
}

bool PtclRes::exportProject(const QString& dirPath) {
    return PtclJson::exportProject(*this, dirPath);
}

const PtclSanitizeReport& PtclRes::sanitizeReport() const {
    return mSanitizeReport;
}

s32 PtclRes::emitterSetCount() const {
    return static_cast<s32>(mEmitterSets.size());
}

s32 PtclRes::emitterCount(s32 setIndex) const {
    if (setIndex < 0 || setIndex >= static_cast<s32>(mEmitterSets.size())) {
        return 0;
    }

    return mEmitterSets[setIndex]->emitterCount();
}

u32 PtclRes::totalEmitterCount() const {
    u32 count = 0;

    for (auto& emitterSet : mEmitterSets) {
        count += emitterSet->emitterCount();
    }
    return count;
}

s32 PtclRes::textureCount() const {
    return static_cast<s32>(mTextures.size());
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

const EmitterSetList& PtclRes::getEmitterSets() const {
    return mEmitterSets;
}

EmitterSet* PtclRes::emitterSet(s32 index) {
    if (index < 0 || index >= static_cast<s32>(mEmitterSets.size())) {
        return nullptr;
    }

    return mEmitterSets[index].get();
}

const EmitterSet* PtclRes::emitterSet(s32 index) const {
    if (index < 0 || index >= static_cast<s32>(mEmitterSets.size())) {
        return nullptr;
    }

    return mEmitterSets[index].get();
}

Emitter* PtclRes::emitter(s32 setIndex, s32 emitterIndex) {
    if (setIndex < 0 || setIndex >= static_cast<s32>(mEmitterSets.size())) {
        return nullptr;
    }

    auto& set = mEmitterSets[setIndex];
    if (emitterIndex < 0 || emitterIndex >= static_cast<s32>(set->emitters().size())) {
        return nullptr;
    }

    return set->emitters()[emitterIndex].get();
}

const Emitter* PtclRes::emitter(s32 setIndex, s32 emitterIndex) const {
    if (setIndex < 0 || setIndex >= static_cast<s32>(mEmitterSets.size())) {
        return nullptr;
    }

    auto& set = mEmitterSets[setIndex];
    if (emitterIndex < 0 || emitterIndex >= static_cast<s32>(set->emitters().size())) {
        return nullptr;
    }

    return set->emitters()[emitterIndex].get();
}

std::vector<TextureUsage> PtclRes::textureUsages(const Texture* texture) const {
    if (!texture || texture->userCount() == 0) {
        return {};
    }

    std::vector<TextureUsage> usages{};
    usages.reserve(texture->userCount());

    for (s32 setIndex = 0; setIndex < static_cast<s32>(mEmitterSets.size()); ++setIndex) {
        const auto& emitters = mEmitterSets[setIndex]->emitters();

        for (s32 emitterIndex = 0; emitterIndex < static_cast<s32>(emitters.size()); ++emitterIndex) {
            const auto& emitter = *emitters[emitterIndex];

            if (emitter.texture() == texture) {
                usages.push_back({setIndex, emitterIndex, false});
            }
            if (emitter.childTexture() == texture) {
                usages.push_back({setIndex, emitterIndex, true});
            }
        }
    }

    return usages;
}

const TextureList& PtclRes::textures() const {
    return mTextures;
}

TextureList& PtclRes::textures() {
    return mTextures;
}

void PtclRes::insertEmitterSet(s32 setIndex, std::unique_ptr<EmitterSet> emitterSet) {
    mEmitterSets.insert(mEmitterSets.begin() + setIndex, std::move(emitterSet));
}

std::unique_ptr<EmitterSet> PtclRes::removeEmitterSet(s32 setIndex) {
    auto it = mEmitterSets.begin() + setIndex;

    std::unique_ptr<EmitterSet> removed = std::move(*it);
    mEmitterSets.erase(it);
    return removed;
}

void PtclRes::insertTexture(s32 index, std::unique_ptr<Texture> texture) {
    mTextures.insert(mTextures.begin() + index, std::move(texture));
}

void PtclRes::swapTexture(s32 index, std::unique_ptr<Texture>& texture) {
    mTextures[index]->swapTexture(*texture);
}

std::unique_ptr<Texture> PtclRes::removeTexture(s32 index) {
    auto it = mTextures.begin() + index;

    std::unique_ptr<Texture> removed = std::move(*it);
    mTextures.erase(it);
    return removed;
}


// ========================================================================== //


} // namespace Ptcl
