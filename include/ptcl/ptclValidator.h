#pragma once

#include "ptcl/ptcl.h"

#include <vector>


namespace Ptcl {


// ========================================================================== //


struct PtclValidationResult {
    TextureList textures;
    EmitterSetList emitterSets;
    PtclSanitizeReport report;
};


// ========================================================================== //


class PtclValidator {
public:
    PtclValidationResult validate(PtclReadResult&& read);

private:
    static BinCommonEmitterData normalizeCommon(const BinCommonEmitterData& data, PtclSanitizeReport& report);
    static BinChildData normalizeChild(const BinChildData& data, PtclSanitizeReport& report);
    static BinFieldSpinData normalizeFieldSpin(const BinFieldSpinData& data, PtclSanitizeReport& report);
    static BinFieldCollisionData normalizeFieldCollision(const BinFieldCollisionData& data, PtclSanitizeReport& report);
    static BinFieldConvergenceData normalizeFieldConvergence(const BinFieldConvergenceData& data, PtclSanitizeReport& report);
    static BinStripeData normalizeStripe(const BinStripeData& data, PtclSanitizeReport& report);

    Texture* buildTexture(PtclReadResult& read, s32 textureIndex, PtclValidationResult& result);

private:
    std::vector<Texture*> mTextureCache{};
};


// ========================================================================== //


} // namespace Ptcl
