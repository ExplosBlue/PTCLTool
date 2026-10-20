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


namespace Ptcl::Render {


// ========================================================================== //


enum class ParticleFollowMode : u8 {
    None,
    Direction,
    Basis,
};

struct EmitterState {
    const Ptcl::TextureHandle* textureHandle{nullptr};
    BillboardType billboardType{BillboardType::Billboard};
    bool isEmitterBillboardMtx{false};
    BlendFuncType blendFunction{BlendFuncType::AlphaBlend};
    DepthFuncType depthFunction{DepthFuncType::TestOnly};
    ColorCombinerFuncType combinerFunction{ColorCombinerFuncType::CombinerConfig0};
    Math::Vector2f rotationBasis{};
    Gfx::Color color{};
    Gfx::Color secondaryColor{};
    TextureWrap textureWrapS{TextureWrap::ClampToEdge};
    TextureWrap textureWrapT{TextureWrap::ClampToEdge};
    TextureFilter textureFilter{TextureFilter::Linear};
    u8 textureLodLevel{0};
    Math::Matrix34f emitterRT{};
};

struct ParticleState {
    Math::Matrix34f matrix{};
    Math::Vector3f positionDiff{};
    Math::Vector2f scale{};
    f32 alpha{0.0f};
    Math::Vector2f uvOffset{};
    Math::Vector2f uvSubOffset{};
    f32 rotation{0.0f};
    Gfx::Color color{};
    ParticleFollowMode followMode{ParticleFollowMode::None};
    Math::Vector3f followDirection{};
    Math::Matrix34f followBasis{};
};

struct ParticleDrawBatch {
    EmitterState state;
    std::vector<ParticleState> particles;
};

struct StripePoint {
    Math::Vector3f position;
    Math::Vector3f outer;
    f32 scale{1.0f};
    bool billboard{true};
    f32 alpha{1.0f};
    f32 t{0.0f};
    Math::Vector2f uv;
    Math::Vector2f uvScale;
};

struct DrawStripe {
    EmitterState state;
    std::vector<StripePoint> points;
};

struct DrawFrame {
    std::vector<ParticleDrawBatch> batches;
    std::vector<ParticleDrawBatch> compactBatches;
    std::vector<DrawStripe> stripes;

    void clear() {
        batches.clear();
        compactBatches.clear();
        stripes.clear();
    }
};


// ========================================================================== //


} // namespace Ptcl::Render
