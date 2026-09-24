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


enum class ParticleFollowMode : u8 {
    None,
    Direction,
    Basis,
};


// ========================================================================== //


struct EmitterState {
    const Ptcl::TextureHandle* textureHandle{nullptr};
    BillboardType billboardType{BillboardType::Billboard};
    bool isEmitterBillboardMtx{false};
    BlendFuncType blendFunction{BlendFuncType::AlphaBlend};
    DepthFuncType depthFunction{DepthFuncType::TestOnly};
    ColorCombinerFuncType combinerFunction{ColorCombinerFuncType::CombinerConfig0};
    ColorCalcType colorCalcType{ColorCalcType::None};
    bool isFogEnabled{false};
    Math::Vector2f rotationBasis{};
    Gfx::Color color{};
    Gfx::Color secondaryColor{};
    TextureWrap textureWrapS{TextureWrap::ClampToEdge};
    TextureWrap textureWrapT{TextureWrap::ClampToEdge};
    TextureFilter textureFilter{TextureFilter::Linear};
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
    ParticleFollowMode followMode{ParticleFollowMode::None};
    bool isChild{false};
    Math::Vector3f followDirection{};
    Math::Matrix34f followBasis{};
};


// ========================================================================== //


struct EmitterDrawBatch {
    EmitterState state;
    std::vector<ParticleState> particles;
};


struct StripePoint {
    Math::Vector3f position;
    Math::Vector3f outer;
    Math::Vector3f emitterY;
    f32 scale{1.0f};
    f32 alpha{1.0f};
    f32 t{0.0f};
    Math::Vector2f uv;
    Math::Vector2f uvScale;
};

struct StripeDraw {
    std::vector<StripePoint> points;
};

struct EmitterDrawStripe {
    EmitterState state;
    StripeDraw stripe;
};

struct EmitterDrawFrame {
    std::vector<EmitterDrawBatch> batches;
    std::vector<EmitterDrawStripe> stripes;
};


// ========================================================================== //


class PtclRenderer {
public:
    void beginCalc();
    void setEmitterState(const EmitterState& state);
    void emitParticle(const ParticleState& particle);
    void emitStripe(const EmitterState& state, const StripeDraw& stripe);
    void endCalc();

    EmitterDrawFrame& frame() { return mFrame; }
    const EmitterDrawFrame& frame() const { return mFrame; }

private:
    EmitterDrawFrame mFrame;
};


// ========================================================================== //


} // namespace Ptcl::Engine
