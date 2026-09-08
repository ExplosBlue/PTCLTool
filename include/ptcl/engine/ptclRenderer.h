#pragma once

#include <vector>

#include "gfx/color.h"
#include "math/matrix.h"
#include "math/vector.h"
#include "ptcl/ptclEnum.h"
#include "typedefs.h"


namespace Ptcl {
class TextureHandle;
} // namespace Ptcl


namespace Ptcl::Engine {

class EmitterInstance;


// ========================================================================== //


struct EmitterState {
    const Ptcl::TextureHandle* textureHandle{nullptr};
    BillboardType billboardType{BillboardType::Billboard};
    bool isEmitterBillboardMtx{false};
    BlendFuncType blendFunction{BlendFuncType::Translucent};
    DepthFuncType depthFunction{DepthFuncType::Unk0};
    ColorCombinerFuncType combinerFunction{ColorCombinerFuncType::CombinerConfig0};
    ColorCalcType colorCalcType{ColorCalcType::None};
    bool isFogEnabled{false};
    Math::Vector2f rotationBasis{};
    Gfx::Color color{};
    Math::Matrix34f emitterRT{};
};


// ========================================================================== //


struct ParticleState {
    Math::Matrix34f matrix{};
    Math::Vector3f positionDiff{};
    Math::Vector2f scale{};
    f32 alpha{0.0f};
    Math::Vector2f uvOffset{};
    Math::Vector2f uvSubOffset{};
    Gfx::Color color{};
    u32 followCount{0};
    Math::Vector3f followDirection{};
    Math::Matrix34f followBasis{};
};


// ========================================================================== //


struct EmitterDrawBatch {
    EmitterState state;
    std::vector<ParticleState> particles;
};


struct EmitterDrawFrame {
    std::vector<EmitterDrawBatch> batches;
};


// ========================================================================== //


class PtclRenderer {
public:
    virtual ~PtclRenderer() = default;

    virtual void beginCalc() = 0;
    virtual void setEmitterState(const EmitterState& state) = 0;
    virtual void emitParticle(const ParticleState& particle) = 0;
    virtual void endCalc() = 0;
};


// ========================================================================== //


} // namespace Ptcl::Engine
