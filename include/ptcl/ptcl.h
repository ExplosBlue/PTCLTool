#pragma once

#include "ptcl/ptclEmitterSet.h"
#include "ptcl/ptclTexture.h"
#include "typedefs.h"

#include <QString>

#include <vector>


namespace Ptcl {


// ========================================================================== //


using TextureList = std::vector<std::shared_ptr<Texture>>;
using EmitterSetList = std::vector<std::unique_ptr<EmitterSet>>;

class PtclRes
{
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
