#ifndef PTCL_H
#define PTCL_H

#include <vector>

#include <QString>
#include <QImage>

#include "typedefs.h"

#include "ptclEnums.h"

#include "gr.h"

namespace Ptcl {

class BinEmitterSet;

class Emitter
{
public:
    Emitter() = default;

    const QString& name() const { return mName; }
    void setName(const QString& name) { mName = name; }

    const QImage& texture() const { return mTexture; }
    void setTexture(const QImage& texture) { mTexture = texture; }

    const gr::PicaDataTextureFormat textureFormat() const { return mTextureFormat; }
    void setTextureFormat(const gr::PicaDataTextureFormat textureFormat) { mTextureFormat = textureFormat; }

private:
    EmitterType mType;
    u32 mFlag;
    u32 randomSeed;

    QString mName; // namePos, name
    QImage mTexture; // - textureRes, texturePos, textureSize, textureHandle

    gr::PicaDataTextureFormat mTextureFormat; // Temp?

    // TODO... more
};


class EmitterSet
{
public:
    EmitterSet() = default;
    EmitterSet(const BinEmitterSet& binEmitterSet);

    std::vector<Emitter>& emitters() { return mEmitters; }

    const QString& name() const { return mName; }
    void setName(const QString& name) { mName = name; }

    const u32 emitterCount() const { return mEmitters.size(); }

    const u32 userData() const { return mUserData; }
    void setUserData(u32 data) { mUserData = data; }

    const bool isShowDetail() const { return mIsShowDetail; }
    void setisShowDetail(bool isShowDetail) { mIsShowDetail = isShowDetail; }

private:
    QString mName;
    std::vector<Emitter> mEmitters;

    u32 mUserData;
    bool mIsShowDetail; // idk what this is
};


class PtclRes
{
public:
    PtclRes() = default;

    void load(const QString& filePath);
    // void save(const QString& filePath);

    std::vector<EmitterSet>& getEmitterSets() { return mEmitterSets; }

    // void printInfo();

private:
    std::vector<EmitterSet> mEmitterSets;
};

}

#endif // PTCL_H
