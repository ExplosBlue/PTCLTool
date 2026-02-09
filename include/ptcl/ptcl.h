#pragma once

#include "ptcl/ptclEmitterSet.h"
#include "ptcl/ptclTexture.h"
#include "typedefs.h"

#include <QFile>
#include <QString>


namespace Ptcl {


// ========================================================================== //


using TextureList = std::vector<std::shared_ptr<Texture>>;
using EmitterSetList = std::vector<std::unique_ptr<EmitterSet>>;


// ========================================================================== //


class PtclBinaryReader {
public:
    explicit PtclBinaryReader(const QString& filePath);

    void readHeader();
    QString readProjectName();
    EmitterSetList readEmitterSets();
    TextureList takeTextures();

private:
    std::shared_ptr<Texture> loadTexture(u32 texturePos, u32 size, u32 width, u32 height, TextureFormat format);

    std::unique_ptr<EmitterSet> readEmitterSet(s32 index);
    void readComplexData(Emitter& emitter, const BinCommonEmitterData& common);
    QString readName(u32 namePos);

private:
    QFile mFile{};
    QDataStream mStream{};
    QByteArray mNameTbl{};

    s64 mEmitterSetTblPos{0};
    s64 mTextureTblPos{0};
    BinHeaderData mHeaderData{};

    TextureList mTextures{};
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

    PtclRes(const PtclRes&) = delete;
    PtclRes& operator=(const PtclRes&) = delete;

    bool load(const QString& filePath);
    bool save(const QString& filePath);

    const QString& name() const;
    void setName(const QString& name);

    const EmitterSetList& getEmitterSets() const;
    EmitterSetList& getEmitterSets();

    const TextureList& textures() const;
    TextureList& textures();

    void addNewEmitterSet();
    void removeEmitterSet(s32 setIndex);

    u32 emitterSetCount() const;
    u32 emitterCount() const;

    const std::unique_ptr<EmitterSet>& appendEmitterSet(std::unique_ptr<EmitterSet>& newSet);

private:
    QString mName;
    EmitterSetList mEmitterSets;
    TextureList mTextures;
};


// ========================================================================== //


} // namespace Ptcl
