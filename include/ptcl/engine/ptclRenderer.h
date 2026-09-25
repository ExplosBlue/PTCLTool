#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

#include "gfx/color.h"
#include "ptcl/engine/combiner.h"
#include "math/matrix.h"
#include "math/vector.h"
#include "ptcl/ptclEnum.h"
#include "typedefs.h"


namespace Ptcl {
class Texture;
class TextureHandle;
} // namespace Ptcl

class QOpenGLFunctions_2_0;


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
    EmitterType emitterType{EmitterType::Simple};
    Ptcl::RotType rotationType{Ptcl::RotType::None};
    bool preChildDraw{false};
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
    f32 rotation{0.0f};
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

struct CompactDrawBatch {
    EmitterState state;
    std::vector<ParticleState> particles;
};

struct EmitterDrawFrame {
    std::vector<EmitterDrawBatch> batches;
    std::vector<EmitterDrawStripe> stripes;
    std::vector<CompactDrawBatch> compactBatches;
};

struct RenderCamera {
    Math::Matrix44f viewProjection{};
    Math::Vector3f right{};
    Math::Vector3f up{};
    Math::Vector3f forward{};
};


struct TextureBinding {
    const Ptcl::Texture* texture{nullptr};
    u32 id{0};
    bool hasTexture{false};
    f32 texelWidth{0.0f};
    f32 texelHeight{0.0f};
};


// ========================================================================== //


class PtclRenderer {
public:
    void initialize(QOpenGLFunctions_2_0& gl);
    std::size_t render(QOpenGLFunctions_2_0& gl, const EmitterDrawFrame& frame, const RenderCamera& camera);
    void renderParticles(QOpenGLFunctions_2_0& gl, const EmitterDrawFrame& frame, const RenderCamera& camera, std::size_t& drawCount);
    void renderCompact(QOpenGLFunctions_2_0& gl, const std::vector<CompactDrawBatch>& batches, const RenderCamera& camera, std::size_t& drawCount);
    void renderStripes(QOpenGLFunctions_2_0& gl, const std::vector<EmitterDrawStripe>& stripes, const RenderCamera& camera);
    void cleanup(QOpenGLFunctions_2_0& gl);

    static std::array<s32, 32> buildCombinerCfg(ColorCombinerFuncType function);
    static void setupRenderState(QOpenGLFunctions_2_0& gl, BlendFuncType blendFunction, DepthFuncType depthFunction);

    TextureBinding bindTexture(QOpenGLFunctions_2_0& gl, const EmitterState& state);
    void cleanupTextures(QOpenGLFunctions_2_0& gl);

    void beginCalc();
    void setEmitterState(const EmitterState& state);
    void emitParticle(const ParticleState& particle);
    void emitStripe(const EmitterState& state, const StripeDraw& stripe);
    void emitCompact(const CompactDrawBatch& batch);

    EmitterDrawFrame& frame() { return mFrame; }
    const EmitterDrawFrame& frame() const { return mFrame; }

private:
    std::unique_ptr<QOpenGLShaderProgram> mParticleProgram;
    std::unique_ptr<QOpenGLShaderProgram> mCompactProgram;
    std::unique_ptr<QOpenGLShaderProgram> mStripeProgram;
    QOpenGLBuffer mParticleVbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject mParticleVao{};
    QOpenGLBuffer mStripeVbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject mStripeVao{};
    QOpenGLBuffer mCompactVbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject mCompactVao{};
    std::unordered_map<const Ptcl::Texture*, u32> mTextureCache;
    EmitterDrawFrame mFrame;
};


// ========================================================================== //


} // namespace Ptcl::Engine
