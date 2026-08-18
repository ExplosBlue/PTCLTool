#pragma once

#include "ptcl/ptclBinary.h"
#include "ptcl/ptclEmitterSet.h"
#include "ptcl/ptclSanitizeReport.h"
#include "ptcl/ptclTexture.h"
#include "typedefs.h"

#include <QFile>
#include <QString>

#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>


namespace Ptcl {


// ========================================================================== //


using TextureList = std::vector<std::unique_ptr<Texture>>;
using EmitterSetList = std::vector<std::unique_ptr<EmitterSet>>;


struct TextureUsage {
    s32 setIndex{-1};
    s32 emitterIndex{-1};
    bool isChild{false};
};

struct RawTextureData {
    u32 pos{0};
    u32 size{0};
    u32 width{0};
    u32 height{0};
    TextureFormat format{TextureFormat::RGBA8888};
    std::vector<u8> bytes{};
};

struct RawEmitterRecord {
    bool isNull{false};
    QString name{};
    BinCommonEmitterData common{};
    s32 textureIndex{-1};
    std::optional<BinComplexEmitterData> complex{};
    s32 childTextureIndex{-1};
    std::optional<BinChildData> child{};
    std::optional<BinFieldRandomData> fieldRandom{};
    std::optional<BinFieldMagnetData> fieldMagnet{};
    std::optional<BinFieldSpinData> fieldSpin{};
    std::optional<BinFieldCollisionData> fieldCollision{};
    std::optional<BinFieldConvergenceData> fieldConvergence{};
    std::optional<BinFieldPosAddData> fieldPosAdd{};
    std::optional<BinFluctuationData> fluctuation{};
    std::optional<BinStripeData> stripe{};
};

struct RawEmitterSetRecord {
    QString name{};
    BinEmitterSetData data{};
    std::vector<RawEmitterRecord> emitters{};
};

struct PtclReadResult {
    QString name;
    std::vector<RawEmitterSetRecord> emitterSets;
    std::vector<RawTextureData> textures;
};

// ========================================================================== //


class PtclBinaryReader {
public:
    explicit PtclBinaryReader(const QString& filePath);

    PtclReadResult readAll();

private:
    s32 loadTexture(u32 texturePos, u32 size, u32 width, u32 height, TextureFormat format);

    RawEmitterSetRecord readEmitterSet(s32 index);
    void readComplexData(RawEmitterRecord& record, const BinCommonEmitterData& common);
    QString readName(u32 namePos);

    void readHeader();
    std::vector<RawEmitterSetRecord> readEmitterSets();
    std::vector<RawTextureData> takeTextures();

private:
    QFile mFile{};
    QDataStream mStream{};
    QByteArray mNameTbl{};

    s64 mEmitterSetTblPos{0};
    s64 mTextureTblPos{0};
    BinHeaderData mHeaderData{};

    std::vector<RawTextureData> mTextures{};
    std::unordered_map<u32, u32> mTextureOffsetMap{};
};


// ========================================================================== //


class PtclRes;

class PtclBinaryWriter {
public:
    explicit PtclBinaryWriter(const QString& filePath);

    void write(const PtclRes& res);

private:
    void buildHeader(const PtclRes& res);
    void buildEmitters(const PtclRes& res);
    void writeFile();

    u32 appendName(const QString& name);
    u32 appendTexture(const TextureHandle& texture, u32& outSize);
    void alignTextureTable();

    void writeSimpleEmitter(const Emitter& emitter);
    void writeComplexEmitter(const Emitter& emitter);

private:
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

private:
    QFile mFile{};
    QDataStream mStream{};

    BinHeaderData mHeader{};

    std::vector<BinEmitterSetData> mEmitterSets{};
    std::vector<BinEmitterTblData> mEmitterTbls{};
    std::vector<DataBlockVariant> mEmitterData{};

    std::vector<char> mNameTbl;
    std::vector<char> mTextureTbl;

    std::unordered_map<u32, u32> mTextureOffsetMap{};

    u32 mEmitterSetsCurOffset{0};
    u32 mEmitterTblCurOffset{0};
    u32 mEmitterDataCurOffset{0};
    u32 mNameTblCurOffset{0};
    u32 mTextureTblCurOffset{0};
};


// ========================================================================== //


class PtclRes {
public:
    PtclRes() = default;

    ~PtclRes() {
        mEmitterSets.clear();
        mTextures.clear();
    }

    PtclRes(const PtclRes&) = delete;
    PtclRes& operator=(const PtclRes&) = delete;

    bool load(const QString& filePath);
    bool save(const QString& filePath);

    bool exportProject(const QString& dirPath);

    const QString& name() const;
    void setName(const QString& name);

    const PtclSanitizeReport& sanitizeReport() const;

    EmitterSetList& getEmitterSets();
    const EmitterSetList& getEmitterSets() const;

    EmitterSet* emitterSet(s32 index);
    const EmitterSet* emitterSet(s32 index) const;

    Emitter* emitter(s32 setIndex, s32 emitterIndex);
    const Emitter* emitter(s32 setIndex, s32 emitterIndex) const;

    std::vector<TextureUsage> textureUsages(const Texture* texture) const;

    TextureList& textures();
    const TextureList& textures() const;

    void insertEmitterSet(s32 setIndex, std::unique_ptr<EmitterSet> emitterSet);
    std::unique_ptr<EmitterSet> removeEmitterSet(s32 setIndex);

    void insertTexture(s32 index, std::unique_ptr<Texture> texture);
    void swapTexture(s32 index, std::unique_ptr<Texture>& texture);
    std::unique_ptr<Texture> removeTexture(s32 index);

    s32 emitterSetCount() const;
    s32 emitterCount(s32 setIndex) const;
    u32 totalEmitterCount() const;
    s32 textureCount() const;

private:
    QString mName;
    TextureList mTextures;
    EmitterSetList mEmitterSets;
    PtclSanitizeReport mSanitizeReport{};
};


// ========================================================================== //


} // namespace Ptcl
