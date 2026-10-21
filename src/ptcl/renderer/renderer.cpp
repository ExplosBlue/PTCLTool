#include "ptcl/renderer/renderer.h"

#include <array>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <QDebug>
#include <QOpenGLFunctions_2_0>
#include <QOpenGLShader>
#include <QString>

#include "gfx/glGeometry.h"
#include "ptcl/renderer/drawGeometry.h"
#include "ptcl/renderer/renderState.h"
#include "ptcl/renderer/shaders.h"


namespace Ptcl::Render {


// ========================================================================== //


namespace {


constexpr GLint cTextureUnit = 0;
constexpr f32 cCompactPointScale = 1.2f;

const Gfx::Color cUntexturedFallbackColor{0.9f, 0.9f, 0.9f, 1.0f};

std::array<GLint, cCombinerConfigSize> toGlConfig(const CombinerConfig& config) {
    std::array<GLint, cCombinerConfigSize> glConfig{};
    for (std::size_t i = 0; i < config.size(); ++i) {
        glConfig[i] = static_cast<GLint>(config[i]);
    }
    return glConfig;
}

bool isUntexturedBlack(const Gfx::Color& color) {
    return color.r() <= 0.0f && color.g() <= 0.0f && color.b() <= 0.0f;
}

Gfx::Color combinerConstant(const Gfx::Color& color, bool hasTexture) {
    return (!hasTexture && isUntexturedBlack(color)) ? cUntexturedFallbackColor : color;
}


} // namespace


// ========================================================================== //
// BatchProgram
// ========================================================================== //


bool BatchProgram::create(std::string_view name, const std::string& vertexSource, const std::string& fragmentSource, std::span<const AttributeBinding> attributeBindings) {
    auto program = std::make_unique<QOpenGLShaderProgram>();

    for (const AttributeBinding& binding : attributeBindings) {
        program->bindAttributeLocation(std::string{binding.name}.c_str(), binding.index);
    }

    if (!program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSource.c_str())     ||
        !program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentSource.c_str()) ||
        !program->link()) {
        qWarning() << "[ptcl] Failed to create" << QString::fromLatin1(name.data(), name.size()) << "shader:" << program->log();
        return false;
    }

    mViewProjection = program->uniformLocation("uViewProj");
    mTextureSampler = program->uniformLocation("uTex");
    mCombinerConfig = program->uniformLocation("uCfg");
    mHasTexture     = program->uniformLocation("uHasTex");
    mTexel          = program->uniformLocation("uTexel");
    mConstantColor  = program->uniformLocation("uConstantColor");
    mAlphaRef       = program->uniformLocation("uAlphaRef");

    mProgram = std::move(program);
    return true;
}

void BatchProgram::destroy() {
    mProgram.reset();
}

void BatchProgram::bind(QOpenGLFunctions_2_0& gl, const Math::Matrix44f& viewProjection) const {
    mProgram->bind();
    mProgram->setUniformValue(mTextureSampler, cTextureUnit);
    gl.glUniformMatrix4fv(mViewProjection, 1, GL_TRUE, viewProjection.data());
}

void BatchProgram::release() const {
    mProgram->release();
}

void BatchProgram::setCombinerConfig(ColorCombinerFuncType combiner) const {
    const std::array<GLint, cCombinerConfigSize> config = toGlConfig(packCombinerConfig(combiner));
    mProgram->setUniformValueArray(mCombinerConfig, config.data(), static_cast<int>(config.size()));
}

void BatchProgram::setTextureSample(const TextureBinding& texture) const {
    mProgram->setUniformValue(mHasTexture, texture.hasTexture ? 1 : 0);
    mProgram->setUniformValue(mTexel, texture.texelWidth, texture.texelHeight);
}

void BatchProgram::setConstantColor(const Gfx::Color& color) const {
    mProgram->setUniformValue(mConstantColor, color.r(), color.g(), color.b(), 1.0f);
}

void BatchProgram::setAlphaRef(GLfloat value) const {
    mProgram->setUniformValue(mAlphaRef, value);
}

void BatchProgram::setInt(GLint location, GLint value) const {
    mProgram->setUniformValue(location, value);
}

void BatchProgram::setFloat(GLint location, GLfloat value) const {
    mProgram->setUniformValue(location, value);
}

void BatchProgram::setVec2(GLint location, GLfloat x, GLfloat y) const {
    mProgram->setUniformValue(location, x, y);
}

void BatchProgram::setVec3(GLint location, GLfloat x, GLfloat y, GLfloat z) const {
    mProgram->setUniformValue(location, x, y, z);
}

void BatchProgram::setVec4(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const {
    mProgram->setUniformValue(location, x, y, z, w);
}

GLint BatchProgram::uniformLocation(std::string_view name) const {
    return mProgram->uniformLocation(std::string{name}.c_str());
}


// ========================================================================== //
// Buffers
// ========================================================================== //


void Renderer::createStripeBuffer(QOpenGLFunctions_2_0& gl) {
    static_assert(sizeof(StripeVertex) == 6 * sizeof(f32));
    static_assert(offsetof(StripeVertex, x) == 0);
    static_assert(offsetof(StripeVertex, u) == 3 * sizeof(f32));
    static_assert(offsetof(StripeVertex, alpha) == 5 * sizeof(f32));

    constexpr std::array<Gfx::VertexAttribute, 3> layout{{
        {0, 3, offsetof(StripeVertex, x)},
        {1, 2, offsetof(StripeVertex, u)},
        {2, 1, offsetof(StripeVertex, alpha)},
    }};

    mStripeVao.create();
    mStripeVbo.create();

    mStripeVao.bind();
    mStripeVbo.bind();
    Gfx::setVertexAttributes(gl, layout, sizeof(StripeVertex));
    mStripeVbo.release();
    mStripeVao.release();
}

void Renderer::createCompactBuffer(QOpenGLFunctions_2_0& gl) {
    static_assert(sizeof(CompactPointVertex) == 16 * sizeof(f32));
    static_assert(offsetof(CompactPointVertex, centerRotation) == 0);
    static_assert(offsetof(CompactPointVertex, scale) == 4 * sizeof(f32));
    static_assert(offsetof(CompactPointVertex, color) == 8 * sizeof(f32));
    static_assert(offsetof(CompactPointVertex, uv) == 12 * sizeof(f32));

    constexpr std::array<Gfx::VertexAttribute, 4> layout{{
        {0, 4, offsetof(CompactPointVertex, centerRotation)},
        {1, 4, offsetof(CompactPointVertex, scale)},
        {2, 4, offsetof(CompactPointVertex, color)},
        {3, 4, offsetof(CompactPointVertex, uv)},
    }};

    mCompactVao.create();
    mCompactVbo.create();

    mCompactVao.bind();
    mCompactVbo.bind();
    Gfx::setVertexAttributes(gl, layout, sizeof(CompactPointVertex));
    mCompactVbo.release();
    mCompactVao.release();
}


// ========================================================================== //
// Lifetime
// ========================================================================== //


void Renderer::initialize(QOpenGLFunctions_2_0& gl) {
    if (mParticleProgram.isValid()) {
        return;
    }

    static constexpr std::array<AttributeBinding, 1> cParticleBindings{{
        {"aPos", 0},
    }};

    static constexpr std::array<AttributeBinding, 4> cCompactBindings{{
        {"aCenterRotation", 0},
        {"aScale",          1},
        {"aColor",          2},
        {"aUv",             3},
    }};

    static constexpr std::array<AttributeBinding, 3> cStripeBindings{{
        {"aPosition", 0},
        {"aUv",       1},
        {"aAlpha",    2},
    }};

    const bool created =
        mParticleProgram.create("particle", Shaders::particleVertex(), Shaders::particleFragment(), cParticleBindings) &&
        mCompactProgram.create("compact",   Shaders::compactVertex(),  Shaders::compactFragment(),  cCompactBindings)  &&
        mStripeProgram.create("stripe",     Shaders::stripeVertex(),   Shaders::particleFragment(), cStripeBindings);

    if (!created) {
        mParticleProgram.destroy();
        mCompactProgram.destroy();
        mStripeProgram.destroy();
        return;
    }

    mParticleUniforms = {
        .center       = mParticleProgram.uniformLocation("uCenter"),
        .axisX        = mParticleProgram.uniformLocation("uAxisX"),
        .axisY        = mParticleProgram.uniformLocation("uAxisY"),
        .primaryColor = mParticleProgram.uniformLocation("uPrimaryColor"),
        .uvOffset     = mParticleProgram.uniformLocation("uUvOffset"),
        .uvScale      = mParticleProgram.uniformLocation("uUvScale"),
    };

    mCompactUniforms = {
        .pointScale = mCompactProgram.uniformLocation("uPointScale"),
        .wrapS      = mCompactProgram.uniformLocation("uWrapS"),
        .wrapT      = mCompactProgram.uniformLocation("uWrapT"),
    };

    mStripeUniforms = {
        .primaryColor = mStripeProgram.uniformLocation("uPrimaryColor"),
    };

    Gfx::createUnitQuad(gl, mParticleVbo, mParticleVao);
    createStripeBuffer(gl);
    createCompactBuffer(gl);
}

void Renderer::render(QOpenGLFunctions_2_0& gl, const DrawFrame& frame, const Gfx::Camera& camera) {
    gl.glDisable(GL_CULL_FACE);
    gl.glActiveTexture(GL_TEXTURE0 + cTextureUnit);

    renderBatches(gl, camera, frame.batches);
    renderCompactBatches(gl, camera, frame.compactBatches);
    renderStripes(gl, camera, frame.stripes);
}

void Renderer::cleanup(QOpenGLFunctions_2_0& gl) {
    mTextures.clear(gl);

    mParticleProgram.destroy();
    mCompactProgram.destroy();
    mStripeProgram.destroy();

    mParticleVao.destroy();
    mParticleVbo.destroy();
    mStripeVao.destroy();
    mStripeVbo.destroy();
    mCompactVao.destroy();
    mCompactVbo.destroy();
}


// ========================================================================== //
// Drawing
// ========================================================================== //


void Renderer::applyBatchState(QOpenGLFunctions_2_0& gl, const BatchProgram& program,
    const EmitterState& state, const Gfx::Color& constantSource) {
    const TextureBinding texture = mTextures.acquire(gl, state);

    program.setCombinerConfig(state.combinerFunction);
    program.setTextureSample(texture);
    program.setConstantColor(combinerConstant(constantSource, texture.hasTexture));

    const f32 alphaRef = (state.depthFunction == DepthFuncType::TestAndWrite)
        ? 128.0f / 255.0f
        : 0.0f;

    program.setAlphaRef(alphaRef);
}

void Renderer::drawParticle(QOpenGLFunctions_2_0& gl, const EmitterState& state, const ParticleState& particle, const Gfx::Camera& camera) {
    const ParticleAxes axes = computeParticleAxes(state, particle, camera);
    const Math::Vector3f center = axes.center(particle.positionDiff, state.rotationBasis);

    mParticleProgram.setVec3(mParticleUniforms.center, center.getX(), center.getY(), center.getZ());
    mParticleProgram.setVec3(mParticleUniforms.axisX,  axes.x.getX(), axes.x.getY(), axes.x.getZ());
    mParticleProgram.setVec3(mParticleUniforms.axisY,  axes.y.getX(), axes.y.getY(), axes.y.getZ());

    mParticleProgram.setVec4(mParticleUniforms.primaryColor, particle.color.r(), particle.color.g(), particle.color.b(), particle.alpha);

    mParticleProgram.setVec2(mParticleUniforms.uvOffset, particle.uvOffset.getX(),    particle.uvOffset.getY());
    mParticleProgram.setVec2(mParticleUniforms.uvScale,  particle.uvSubOffset.getX(), particle.uvSubOffset.getY());

    gl.glDrawArrays(GL_TRIANGLES, 0, Gfx::unitQuadVertexCount);
}

void Renderer::renderBatches(QOpenGLFunctions_2_0& gl, const Gfx::Camera& camera,
    const std::vector<ParticleDrawBatch>& batches) {
    if (!mParticleProgram.isValid() || batches.empty()) {
        return;
    }

    mParticleProgram.bind(gl, camera.viewProjection());
    mParticleVao.bind();

    for (const ParticleDrawBatch& batch : batches) {
        setupRenderState(gl, batch.state.blendFunction, batch.state.depthFunction);
        applyBatchState(gl, mParticleProgram, batch.state, batch.state.color);

        for (const ParticleState& particle : batch.particles) {
            drawParticle(gl, batch.state, particle, camera);
        }
    }
    mParticleVao.release();
    mParticleProgram.release();
}

void Renderer::appendStripeVertices(std::vector<StripeVertex>& vertices,
    const StripePoint& point, const Gfx::Camera& camera) const {
    const StripeEdges edges = computeStripeEdges(point, camera);
    const f32 v = point.uv.getY() + point.t * point.uvScale.getY();

    const f32 uPlus = point.billboard
        ? point.uv.getX() + point.uvScale.getX()
        : point.uv.getX();

    const f32 uMinus = point.billboard
        ? point.uv.getX()
        : point.uv.getX() + point.uvScale.getX();

    vertices.push_back({
        edges.left.getX(), edges.left.getY(), edges.left.getZ(),
        uPlus, v, point.alpha,
    });

    vertices.push_back({
        edges.right.getX(), edges.right.getY(), edges.right.getZ(),
        uMinus, v, point.alpha,
    });
}

void Renderer::renderStripes(QOpenGLFunctions_2_0& gl, const Gfx::Camera& camera, const std::vector<DrawStripe>& stripes) {
    if (!mStripeProgram.isValid() || stripes.empty()) {
        return;
    }

    mStripeProgram.bind(gl, camera.viewProjection());

    std::vector<StripeVertex> vertices;
    for (const DrawStripe& stripe : stripes) {
        if (stripe.points.size() < 2) {
            continue;
        }

        setupRenderState(gl, stripe.state.blendFunction, stripe.state.depthFunction);
        applyBatchState(gl, mStripeProgram, stripe.state, stripe.state.secondaryColor);

        const Gfx::Color& color = stripe.state.color;
        mStripeProgram.setVec4(mStripeUniforms.primaryColor,
            color.r(), color.g(), color.b(), 1.0f);

        vertices.clear();
        vertices.reserve(stripe.points.size() * 2);
        for (const StripePoint& point : stripe.points) {
            appendStripeVertices(vertices, point, camera);
        }

        mStripeVbo.bind();
        mStripeVbo.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(vertices[0])));
        mStripeVbo.release();

        mStripeVao.bind();
        gl.glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(vertices.size()));
        mStripeVao.release();
    }

    mStripeProgram.release();
}

Renderer::CompactPointVertex Renderer::toCompactVertex(const ParticleState& particle) {
    return {
        .centerRotation = {
            particle.positionDiff.getX(),
            particle.positionDiff.getY(),
            particle.positionDiff.getZ(),
            particle.rotation,
        },
        .scale = {
            particle.scale.getX(),
            particle.scale.getY(),
            0.0f,
            0.0f,
        },
        .color = {
            particle.color.r(),
            particle.color.g(),
            particle.color.b(),
            particle.alpha,
        },
        .uv = {
            particle.uvOffset.getX(),
            particle.uvOffset.getY(),
            particle.uvSubOffset.getX(),
            particle.uvSubOffset.getY(),
        },
    };
}

void Renderer::renderCompactBatches(QOpenGLFunctions_2_0& gl, const Gfx::Camera& camera,
    const std::vector<ParticleDrawBatch>& batches) {
    if (!mCompactProgram.isValid() || batches.empty()) {
        return;
    }

    mCompactProgram.bind(gl, camera.viewProjection());
    mCompactProgram.setFloat(mCompactUniforms.pointScale, cCompactPointScale);

    gl.glEnable(GL_PROGRAM_POINT_SIZE);

    std::vector<CompactPointVertex> vertices;

    mCompactVao.bind();
    mCompactVbo.bind();

    for (const ParticleDrawBatch& batch : batches) {
        if (batch.particles.empty()) {
            continue;
        }

        vertices.clear();
        vertices.reserve(batch.particles.size());
        for (const ParticleState& particle : batch.particles) {
            vertices.push_back(toCompactVertex(particle));
        }

        mCompactVbo.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(vertices[0])));

        setupRenderState(gl, batch.state.blendFunction, batch.state.depthFunction);
        applyBatchState(gl, mCompactProgram, batch.state, batch.state.color);

        mCompactProgram.setInt(mCompactUniforms.wrapS, static_cast<GLint>(batch.state.textureWrapS));
        mCompactProgram.setInt(mCompactUniforms.wrapT, static_cast<GLint>(batch.state.textureWrapT));

        gl.glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vertices.size()));
    }
    gl.glDisable(GL_PROGRAM_POINT_SIZE);
    mCompactVbo.release();
    mCompactVao.release();
    mCompactProgram.release();
}


// ========================================================================== //


} // namespace Ptcl::Render
