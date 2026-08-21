#pragma once

#include "ptcl/ptcl.h"

#include <memory>
#include <optional>


namespace Ptcl::PtclJson {


// ========================================================================== //


bool exportProject(const PtclRes& res, const QString& dirPath);

bool exportEmitter(const Emitter& emitter, const QString& filePath);

bool exportEmitterSet(const EmitterSet& emitterSet, const QString& filePath);

struct ImportEmitterResult {
    std::unique_ptr<Emitter> emitter;
    TextureList textures;
};

std::optional<ImportEmitterResult> importEmitter(const QString& filePath, const QString& projectDir = {}, s32 textureIdOffset = 0);

bool importProject(const QString& projPath, PtclRes& res, PtclSanitizeReport& report);


// ========================================================================== //


} // namespace Ptcl::PtclJson
