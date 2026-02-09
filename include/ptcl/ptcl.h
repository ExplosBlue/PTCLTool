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


class PtclRes;

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


class PtclRes {
public:
    PtclRes() = default;

    PtclRes(const PtclRes&) = delete;
    PtclRes& operator=(const PtclRes&) = delete;

    bool load(const QString& filePath);
    bool save(const QString& filePath);

    const QString& name() const;
    void setName(const QString& name);

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
