#include "ptcl/engine/ptclRenderer.h"

#include "ptcl/ptclTexture.h"

#include <QDebug>
#include <QImage>
#include <QOpenGLFunctions_2_0>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cmath>


namespace {


const void* offsetPointer(std::size_t offset) {
    const void* result = nullptr;
    const auto value = static_cast<std::uintptr_t>(offset);
    std::memcpy(&result, &value, sizeof(result));
    return result;
}


} // namespace


constexpr const char* cVertShader = R"(#version 150 core
in vec2 aPos;
uniform vec3 uCenter;
uniform vec3 uAxisX;
uniform vec3 uAxisY;
uniform mat4 uViewProj;
uniform vec2 uUvOffset;
uniform vec2 uUvScale;
uniform vec2 uTexel;
uniform vec4 uPrimaryColor;
out vec2 vUv;
out vec2 vLocalUv;
out vec4 vColor;
out float vParticleAlpha;
void main() {
    vLocalUv = vec2(aPos.x + 0.5, 0.5 - aPos.y);
    vColor = uPrimaryColor;
    vParticleAlpha = uPrimaryColor.a;
    vec2 uvMin = uUvOffset + uTexel * 0.5;
    vec2 uvMax = uUvOffset + uUvScale - uTexel * 0.5;
    vUv = mix(uvMin, uvMax, vLocalUv);
    vec3 world = uCenter + aPos.x * uAxisX + aPos.y * uAxisY;
    gl_Position = uViewProj * vec4(world, 1.0);
}
)";

constexpr const char* cStripeVertShader = R"(#version 150 core
in vec3 aPosition;
in vec2 aUv;
in float aAlpha;
uniform mat4 uViewProj;
uniform vec4 uPrimaryColor;
out vec2 vUv;
out vec2 vLocalUv;
out vec4 vColor;
out float vParticleAlpha;
void main() {
    vUv = vec2(aUv.x, 1.0 - aUv.y);
    vLocalUv = vec2(0.5);
    vColor = vec4(uPrimaryColor.rgb, aAlpha);
    vParticleAlpha = aAlpha;
    gl_Position = uViewProj * vec4(aPosition, 1.0);
}
)";

constexpr const char* cFragShader = R"(#version 150 core
in vec2 vUv;
in vec2 vLocalUv;
in vec4 vColor;
in float vParticleAlpha;
uniform sampler2D uTex;
uniform int uHasTex;
uniform vec4 uConstantColor;
uniform vec4 uPrimaryColor;
uniform int uCfg[32];
out vec4 fragColor;

vec4 stageSrc(int base, int idx, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    int inIdx = uCfg[base + 1 + idx];
    if (inIdx == 0) return constant;
    else if (inIdx == 1) return primary;
    else if (inIdx == 2) return tex;
    else return prev;
}

vec3 stageColor(int base, int idx, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    vec4 v = stageSrc(base, idx, constant, primary, tex, prev);
    int op = uCfg[base + 4 + idx];
    if (op == 1) return vec3(v.g);
    else if (op == 2) return vec3(1.0) - v.rgb;
    else return v.rgb;
}

float stageAlpha(int base, int idx, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    vec4 v = stageSrc(base, idx, constant, primary, tex, prev);
    int op = uCfg[base + 4 + idx];
    if (op == 1) return v.g;
    else if (op == 2) return 1.0 - v.a;
    else return v.a;
}

vec3 evalColorStage(int base, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    int fn = uCfg[base];
    vec3 a = stageColor(base, 0, constant, primary, tex, prev);
    vec3 b = stageColor(base, 1, constant, primary, tex, prev);
    vec3 c = stageColor(base, 2, constant, primary, tex, prev);
    vec3 outColor;
    if (fn == 0) outColor = a;
    else if (fn == 1) outColor = a * b;
    else if (fn == 2) outColor = max(a - b, vec3(0.0));
    else outColor = clamp(a * b + c, vec3(0.0), vec3(1.0));
    if (uCfg[base + 7] == 1) outColor = min(outColor * 2.0, vec3(1.0));
    return outColor;
}

float evalAlphaStage(int base, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    int fn = uCfg[base];
    float a = stageAlpha(base, 0, constant, primary, tex, prev);
    float b = stageAlpha(base, 1, constant, primary, tex, prev);
    float c = stageAlpha(base, 2, constant, primary, tex, prev);
    float outVal;
    if (fn == 0) outVal = a;
    else if (fn == 1) outVal = a * b;
    else if (fn == 2) outVal = max(a - b, 0.0);
    else outVal = clamp(a * b + c, 0.0, 1.0);
    if (uCfg[base + 7] == 1) outVal = min(outVal * 2.0, 1.0);
    return outVal;
}

void main() {
    vec4 tex;
    if (uHasTex == 1) {
        tex = texture(uTex, vUv);
    } else {
        vec2 c = vLocalUv - vec2(0.5);
        float m = step(length(c), 0.5);
        tex = vec4(vec3(1.0), m);
    }
    vec4 primary = vec4(vColor.rgb, vParticleAlpha);
    vec4 prev = vec4(0.0);
    vec3 cOut = evalColorStage(0, uConstantColor, primary, tex, prev);
    prev = vec4(cOut, 0.0);
    cOut = evalColorStage(8, uConstantColor, primary, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, 0.0);
    float aOut = evalAlphaStage(16, uConstantColor, primary, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, aOut);
    aOut = evalAlphaStage(24, uConstantColor, primary, tex, prev);
    fragColor = vec4(cOut, aOut);
}
)";

constexpr const char* cCompactPointVertShader = R"(#version 150 core
in vec4 aCenterRotation;
in vec4 aScale;
in vec4 aColor;
in vec4 aUv;
uniform mat4 uViewProj;
uniform float uPointScale;
out vec2 vUv;
out vec2 vUvScale;
out vec2 vLocalUv;
out vec4 vColor;
out float vParticleAlpha;
out float vRotation;

void main() {
    vUv = aUv.xy;
    vUvScale = aUv.zw;
    vLocalUv = vec2(0.5);
    vColor = vec4(aColor.rgb, aColor.a);
    vParticleAlpha = aColor.a;
    vRotation = radians(aCenterRotation.w);
    gl_PointSize = max(1.0, max(aScale.x, aScale.y) * 2.0 * uPointScale);
    gl_Position = uViewProj * vec4(aCenterRotation.xyz, 1.0);
}
)";

constexpr const char* cCompactFragShader = R"(#version 150 core
in vec2 vUv;
in vec2 vUvScale;
in vec2 vLocalUv;
in vec4 vColor;
in float vParticleAlpha;
in float vRotation;
uniform sampler2D uTex;
uniform int uHasTex;
uniform int uWrapS;
uniform int uWrapT;
uniform vec2 uTexel;
uniform vec4 uConstantColor;
uniform int uCfg[32];
out vec4 fragColor;

vec4 stageSrc(int base, int idx, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    int inIdx = uCfg[base + 1 + idx];
    if (inIdx == 0) return constant;
    if (inIdx == 1) return primary;
    if (inIdx == 2) return tex;
    return prev;
}
vec3 stageColor(int base, int idx, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    vec4 value = stageSrc(base, idx, constant, primary, tex, prev);
    int op = uCfg[base + 4 + idx];
    if (op == 1) return vec3(value.g);
    if (op == 2) return vec3(1.0) - value.rgb;
    return value.rgb;
}
float stageAlpha(int base, int idx, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    vec4 value = stageSrc(base, idx, constant, primary, tex, prev);
    int op = uCfg[base + 4 + idx];
    if (op == 1) return value.g;
    if (op == 2) return 1.0 - value.a;
    return value.a;
}
vec3 evalColorStage(int base, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    vec3 a = stageColor(base, 0, constant, primary, tex, prev);
    vec3 b = stageColor(base, 1, constant, primary, tex, prev);
    vec3 c = stageColor(base, 2, constant, primary, tex, prev);
    int fn = uCfg[base];
    vec3 result;
    if (fn == 0) result = a;
    else if (fn == 1) result = a * b;
    else if (fn == 2) result = max(a - b, vec3(0.0));
    else result = clamp(a * b + c, vec3(0.0), vec3(1.0));
    if (uCfg[base + 7] == 1) result = min(result * 2.0, vec3(1.0));
    return result;
}
float evalAlphaStage(int base, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    float a = stageAlpha(base, 0, constant, primary, tex, prev);
    float b = stageAlpha(base, 1, constant, primary, tex, prev);
    float c = stageAlpha(base, 2, constant, primary, tex, prev);
    int fn = uCfg[base];
    float result;
    if (fn == 0) result = a;
    else if (fn == 1) result = a * b;
    else if (fn == 2) result = max(a - b, 0.0);
    else result = clamp(a * b + c, 0.0, 1.0);
    if (uCfg[base + 7] == 1) result = min(result * 2.0, 1.0);
    return result;
}
float wrapCoordinate(float value, int mode) {
    if (mode == 1) {
        return fract(value);
    }
    if (mode == 0) {
        return abs(fract(value * 0.5) * 2.0 - 1.0);
    }
    return clamp(value, 0.0, 1.0);
}

vec2 applyTextureWrap(vec2 uv) {
    return vec2(
        wrapCoordinate(uv.x, uWrapS),
        wrapCoordinate(uv.y, uWrapT));
}

void main() {
    vec2 local = gl_PointCoord - vec2(0.5);
    float sinRotation = sin(-vRotation);
    float cosRotation = cos(-vRotation);
    local = vec2(
        local.x * cosRotation - local.y * sinRotation,
        local.x * sinRotation + local.y * cosRotation) + vec2(0.5);
    vec4 tex;
    if (uHasTex == 1) {
        vec2 localUv = vec2(local.x, 1.0 - local.y);
        vec2 pointUv = mix(vUv + uTexel * 0.5, vUv + vUvScale - uTexel * 0.5, localUv);
        pointUv = applyTextureWrap(pointUv);
        tex = texture(uTex, pointUv);
    } else {
        tex = vec4(1.0, 1.0, 1.0, step(length(local), 0.5));
    }
    vec4 primary = vec4(vColor.rgb, vParticleAlpha);
    vec4 prev = vec4(0.0);
    vec3 color = evalColorStage(0, uConstantColor, primary, tex, prev);
    prev = vec4(color, 0.0);
    color = evalColorStage(8, uConstantColor, primary, tex, prev);
    prev = vec4(0.0);
    float alpha = evalAlphaStage(16, uConstantColor, primary, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, alpha);
    alpha = evalAlphaStage(24, uConstantColor, primary, tex, prev);
    fragColor = vec4(color, alpha);
}
)";


namespace Ptcl::Engine {


// ========================================================================== //


struct StripeGLVertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat u;
    GLfloat v;
    GLfloat alpha;
};

void PtclRenderer::initialize(QOpenGLFunctions_2_0& gl) {
    if (mParticleProgram != nullptr) {
        return;
    }

    auto particleProgram = std::make_unique<QOpenGLShaderProgram>();
    if (!particleProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, cVertShader) ||
        !particleProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, cFragShader) ||
        !particleProgram->link()) {
        qWarning() << "[ptcl] Failed to create particle shader:" << particleProgram->log();
        return;
    }

    auto compactProgram = std::make_unique<QOpenGLShaderProgram>();
    compactProgram->bindAttributeLocation("aCenterRotation", 0);
    compactProgram->bindAttributeLocation("aScale", 1);
    compactProgram->bindAttributeLocation("aColor", 2);
    compactProgram->bindAttributeLocation("aUv", 3);
    if (!compactProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, cCompactPointVertShader) ||
        !compactProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, cCompactFragShader) ||
        !compactProgram->link()) {
        qWarning() << "[ptcl] Failed to create compact shader:" << compactProgram->log();
        return;
    }

    auto stripeProgram = std::make_unique<QOpenGLShaderProgram>();
    if (!stripeProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, cStripeVertShader) ||
        !stripeProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, cFragShader) ||
        !stripeProgram->link()) {
        qWarning() << "[ptcl] Failed to create stripe shader:" << stripeProgram->log();
        return;
    }

    static constexpr std::array<GLfloat, 12> quad = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f,
    };

    mParticleVao.create();
    mParticleVbo.create();
    mParticleVao.bind();
    mParticleVbo.bind();
    mParticleVbo.allocate(quad.data(), static_cast<int>(sizeof(quad)));
    gl.glEnableVertexAttribArray(0);
    gl.glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    mParticleVbo.release();
    mParticleVao.release();

    mStripeVao.create();
    mStripeVbo.create();

    mStripeVao.bind();
    mStripeVbo.bind();

    gl.glEnableVertexAttribArray(0);
    gl.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StripeGLVertex), offsetPointer(offsetof(StripeGLVertex, x)));
    gl.glEnableVertexAttribArray(1);
    gl.glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(StripeGLVertex), offsetPointer(offsetof(StripeGLVertex, u)));
    gl.glEnableVertexAttribArray(2);
    gl.glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(StripeGLVertex), offsetPointer(offsetof(StripeGLVertex, alpha)));

    mStripeVbo.release();
    mStripeVao.release();

    mCompactVao.create();
    mCompactVbo.create();
    mCompactVao.bind();
    mCompactVbo.bind();
    constexpr GLsizei cCompactPointStride = 16 * sizeof(GLfloat);
    const auto compactOffset = [](std::size_t offset) {
        return offsetPointer(offset);
    };
    gl.glEnableVertexAttribArray(0);
    gl.glEnableVertexAttribArray(1);
    gl.glEnableVertexAttribArray(2);
    gl.glEnableVertexAttribArray(3);
    gl.glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, cCompactPointStride, compactOffset(0));
    gl.glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, cCompactPointStride, compactOffset(4 * sizeof(GLfloat)));
    gl.glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, cCompactPointStride, compactOffset(8 * sizeof(GLfloat)));
    gl.glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, cCompactPointStride, compactOffset(12 * sizeof(GLfloat)));
    mCompactVbo.release();
    mCompactVao.release();

    mParticleProgram = std::move(particleProgram);
    mCompactProgram = std::move(compactProgram);
    mStripeProgram = std::move(stripeProgram);
}

std::array<s32, 32> PtclRenderer::buildCombinerCfg(
    ColorCombinerFuncType function) {
    std::array<s32, 32> config{};
    const auto index = static_cast<std::size_t>(function);
    if (index >= Combiner::configurations.size()) {
        return config;
    }

    const Combiner::Config& combiner = Combiner::configurations[index];
    const std::array<const Combiner::Stage*, 4> stages = {
        &combiner.colorStage[0], &combiner.colorStage[1],
        &combiner.alphaStage[0], &combiner.alphaStage[1],
    };

    for (s32 stageIndex = 0; stageIndex < 4; ++stageIndex) {
        const Combiner::Stage& stage = *stages[stageIndex];
        const s32 base = stageIndex * 8;
        config[base + 0] = static_cast<s32>(stage.func);
        config[base + 1] = static_cast<s32>(stage.inputs[0]);
        config[base + 2] = static_cast<s32>(stage.inputs[1]);
        config[base + 3] = static_cast<s32>(stage.inputs[2]);
        config[base + 4] = static_cast<s32>(stage.operands[0]);
        config[base + 5] = static_cast<s32>(stage.operands[1]);
        config[base + 6] = static_cast<s32>(stage.operands[2]);
        config[base + 7] = static_cast<s32>(stage.scale);
    }
    return config;
}

void PtclRenderer::setupRenderState(QOpenGLFunctions_2_0& gl,
    BlendFuncType blendFunction, DepthFuncType depthFunction) {
    switch (blendFunction) {
    case BlendFuncType::AlphaBlend:
        gl.glBlendEquation(GL_FUNC_ADD);
        gl.glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
            GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
        break;
    case BlendFuncType::Additive:
        gl.glBlendEquation(GL_FUNC_ADD);
        gl.glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE);
        break;
    case BlendFuncType::Subtractive:
        gl.glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
        gl.glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE);
        break;
    case BlendFuncType::Opaque:
        gl.glBlendEquation(GL_FUNC_ADD);
        gl.glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
        break;
    }

    switch (depthFunction) {
    case DepthFuncType::TestOnly:
        gl.glEnable(GL_DEPTH_TEST);
        gl.glDepthFunc(GL_LESS);
        gl.glDepthMask(GL_FALSE);
        gl.glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        break;
    case DepthFuncType::Disabled:
        gl.glDisable(GL_DEPTH_TEST);
        gl.glDepthMask(GL_FALSE);
        gl.glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        break;
    case DepthFuncType::TestAndWrite:
        gl.glEnable(GL_DEPTH_TEST);
        gl.glDepthFunc(GL_LESS);
        gl.glDepthMask(GL_TRUE);
        gl.glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        break;
    }
}

TextureBinding PtclRenderer::bindTexture(QOpenGLFunctions_2_0& gl, const EmitterState& state) {
    if (state.textureHandle == nullptr || !state.textureHandle->isValid()) {
        return {};
    }

    TextureBinding binding{};

    binding.texture = state.textureHandle->get();
    if (binding.texture == nullptr || binding.texture->isPlaceholder()) {
        return binding;
    }

    const QImage source = binding.texture->textureData();
    if (source.isNull()) {
        return binding;
    }
    binding.hasTexture = true;
    binding.texelWidth = 1.0f / static_cast<f32>(source.width());
    binding.texelHeight = 1.0f / static_cast<f32>(source.height());

    const auto cacheEntry = mTextureCache.find(binding.texture);
    if (cacheEntry != mTextureCache.end()) {
        binding.id = cacheEntry->second;
    } else {
        QImage image = source.convertToFormat(QImage::Format_RGBA8888);
        gl.glGenTextures(1, &binding.id);
        gl.glBindTexture(GL_TEXTURE_2D, binding.id);
        gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
            image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
            image.constBits());
        if (binding.id != 0) {
            mTextureCache.emplace(binding.texture, binding.id);
        } else {
            binding.hasTexture = false;
            return binding;
        }
    }

    gl.glBindTexture(GL_TEXTURE_2D, binding.id);
    const auto wrapS = (state.textureWrapS == TextureWrap::Repeat) ? GL_REPEAT
        : (state.textureWrapS == TextureWrap::MirroredRepeat) ? GL_MIRRORED_REPEAT
        : GL_CLAMP_TO_EDGE;
    const auto wrapT = (state.textureWrapT == TextureWrap::Repeat) ? GL_REPEAT
        : (state.textureWrapT == TextureWrap::MirroredRepeat) ? GL_MIRRORED_REPEAT
        : GL_CLAMP_TO_EDGE;
    const auto filter = (state.textureFilter == TextureFilter::Nearest)
        ? GL_NEAREST : GL_LINEAR;
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    return binding;
}

void PtclRenderer::cleanupTextures(QOpenGLFunctions_2_0& gl) {
    for (const auto& [texture, id] : mTextureCache) {
        (void)texture;
        if (id != 0) {
            gl.glDeleteTextures(1, &id);
        }
    }
    mTextureCache.clear();
}

void computeParticleAxes(const Ptcl::Engine::EmitterState& state, const Ptcl::Engine::ParticleState& particle,
    const RenderCamera& cam, Math::Vector3f& axisX, Math::Vector3f& axisY) {

    const f32 width = particle.scale.getX();
    const f32 height = particle.scale.getY();
    const auto& m = particle.matrix;

    const auto& followBasis = particle.followMode == Ptcl::Engine::ParticleFollowMode::Basis ? particle.followBasis : state.emitterRT;
    const auto localX = Math::Vector3f{m(0, 3), m(1, 3), m(2, 3)};
    const auto localY = Math::Vector3f{m(0, 2), m(1, 2), m(2, 2)};
    const auto worldX = Math::VectorCalc::rotate(followBasis, localX);
    const auto worldY = Math::VectorCalc::rotate(followBasis, localY);

    switch (state.billboardType) {
    case Ptcl::BillboardType::BillboardY: {
        const Math::Vector3f worldUp{0.0f, 1.0f, 0.0f};
        axisX = Math::VectorCalc::add(
            Math::VectorCalc::multScalar(cam.right, localX.getX()),
            Math::VectorCalc::multScalar(cam.up, localX.getY()));
        Math::VectorCalc::normalize(axisX);
        axisX = Math::VectorCalc::multScalar(axisX, width);
        axisY = Math::VectorCalc::multScalar(worldUp, height);
        break;
    }

    case Ptcl::BillboardType::PolygonXY:
        axisX = Math::VectorCalc::multScalar(worldX, width);
        axisY = Math::VectorCalc::multScalar(worldY, height);
        break;

    case Ptcl::BillboardType::PolygonXZ:
        axisX = Math::VectorCalc::multScalar(worldX, width);
        axisY = Math::VectorCalc::multScalar(
            Math::VectorCalc::rotate(followBasis, Math::Vector3f{m(0, 1), m(1, 1), m(2, 1)}), height);
        break;

    case Ptcl::BillboardType::VelLook:
    case Ptcl::BillboardType::VelLookPolygon: {
        Math::Vector3f dir = particle.followDirection;
        if (dir.length() <= 0.0001f) {
            dir = cam.up;
        }
        Math::VectorCalc::normalize(dir);

        const f32 axisSign = state.rotationBasis.getY() < 0.0f ? -1.0f : 1.0f;
        axisY = Math::VectorCalc::multScalar(dir, axisSign * height);
        Math::Vector3f baseRight;
        if (state.billboardType == Ptcl::BillboardType::VelLookPolygon) {
            const Math::Vector3f emitterRight =
                Math::VectorCalc::rotate(followBasis, {1.0f, 0.0f, 0.0f});
            baseRight = Math::VectorCalc::cross(dir, emitterRight);
        } else {
            baseRight = Math::VectorCalc::cross(dir, cam.forward);
        }
        if (baseRight.length() <= 0.0001f) {
            baseRight = cam.right;
        }
        Math::VectorCalc::normalize(baseRight);

        const f32 dirDotReferenceX = dir.getX();
        const Math::Vector3f referenceRight{
            1.0f - dir.getX() * dirDotReferenceX,
            -dir.getY() * dirDotReferenceX,
            -dir.getZ() * dirDotReferenceX,
        };
        const f32 dirDotLocalX = dir.getX() * localX.getX()
            + dir.getY() * localX.getY() + dir.getZ() * localX.getZ();
        const Math::Vector3f rotatedReferenceRight{
            localX.getX() - dir.getX() * dirDotLocalX,
            localX.getY() - dir.getY() * dirDotLocalX,
            localX.getZ() - dir.getZ() * dirDotLocalX,
        };
        Math::Vector3f right = baseRight;
        if (referenceRight.length() > 0.0001f && rotatedReferenceRight.length() > 0.0001f) {
            const Math::Vector3f ref = Math::VectorCalc::multScalar(
                referenceRight, 1.0f / referenceRight.length());
            const Math::Vector3f rotated = Math::VectorCalc::multScalar(
                rotatedReferenceRight, 1.0f / rotatedReferenceRight.length());
            const f32 cosine = ref.getX() * rotated.getX()
                + ref.getY() * rotated.getY() + ref.getZ() * rotated.getZ();
            const Math::Vector3f sineVector = Math::VectorCalc::cross(ref, rotated);
            const f32 sine = sineVector.getX() * dir.getX()
                + sineVector.getY() * dir.getY() + sineVector.getZ() * dir.getZ();
            const Math::Vector3f tangent = Math::VectorCalc::cross(dir, baseRight);
            right = Math::VectorCalc::add(
                Math::VectorCalc::multScalar(baseRight, cosine),
                Math::VectorCalc::multScalar(tangent, sine));
            Math::VectorCalc::normalize(right);
        }
        axisX = Math::VectorCalc::multScalar(right, width);
        break;
    }

    case Ptcl::BillboardType::Stripe:
    case Ptcl::BillboardType::ComplexStripe:
    case Ptcl::BillboardType::Primitive:
    default:
        axisX = Math::VectorCalc::multScalar(worldX, width);
        axisY = Math::VectorCalc::multScalar(worldY, height);
        break;

    case Ptcl::BillboardType::Billboard: {
        if (state.isEmitterBillboardMtx) {
            axisX = Math::VectorCalc::multScalar(worldX, width);
            axisY = Math::VectorCalc::multScalar(worldY, height);
            break;
        }

        if (state.rotationType == Ptcl::RotType::RotY) {
            axisX = Math::VectorCalc::multScalar(worldX, width);
            axisY = Math::VectorCalc::multScalar(worldY, height);
            break;
        }

        const f32 screenX = localX.getX();
        const f32 screenY = state.rotationType == Ptcl::RotType::RotZ
            ? -localX.getY()
            : localX.getY();
        const f32 projectedLength = std::sqrt(screenX * screenX + screenY * screenY);
        if (projectedLength > 0.0001f) {
            const f32 cosine = screenX / projectedLength;
            const f32 sine = screenY / projectedLength;
            axisX = Math::VectorCalc::add(
                Math::VectorCalc::multScalar(cam.right, cosine),
                Math::VectorCalc::multScalar(cam.up, sine));
            axisY = Math::VectorCalc::add(
                Math::VectorCalc::multScalar(cam.right, -sine),
                Math::VectorCalc::multScalar(cam.up, cosine));
        } else {
            axisX = cam.right;
            axisY = cam.up;
        }
        axisX = Math::VectorCalc::multScalar(axisX, -width);
        axisY = Math::VectorCalc::multScalar(axisY, height);
        break;
    }
    }
}


std::size_t PtclRenderer::render(QOpenGLFunctions_2_0& gl,
    const EmitterDrawFrame& frame, const RenderCamera& camera) {
    std::size_t drawCount = 0;
    renderParticles(gl, frame, camera, drawCount);
    renderCompact(gl, frame.compactBatches, camera, drawCount);
    renderStripes(gl, frame.stripes, camera);
    return drawCount;
}

void PtclRenderer::renderParticles(QOpenGLFunctions_2_0& gl,
    const EmitterDrawFrame& frame, const RenderCamera& camera,std::size_t& drawCount) {
    drawCount = 0;
    if (mParticleProgram == nullptr) {
        return;
    }

    mParticleProgram->bind();
    const auto centerLocation = mParticleProgram->uniformLocation("uCenter");
    const auto axisXLocation = mParticleProgram->uniformLocation("uAxisX");
    const auto axisYLocation = mParticleProgram->uniformLocation("uAxisY");
    const auto viewProjectionLocation = mParticleProgram->uniformLocation("uViewProj");
    const auto configLocation = mParticleProgram->uniformLocation("uCfg");
    const auto constantColorLocation = mParticleProgram->uniformLocation("uConstantColor");
    const auto primaryColorLocation = mParticleProgram->uniformLocation("uPrimaryColor");
    const auto hasTextureLocation = mParticleProgram->uniformLocation("uHasTex");
    const auto uvOffsetLocation = mParticleProgram->uniformLocation("uUvOffset");
    const auto uvScaleLocation = mParticleProgram->uniformLocation("uUvScale");
    const auto texelLocation = mParticleProgram->uniformLocation("uTexel");
    gl.glUniformMatrix4fv(viewProjectionLocation, 1, GL_TRUE, camera.viewProjection.data());
    gl.glActiveTexture(GL_TEXTURE0);

    mParticleVao.bind();
    for (const auto& batch : frame.batches) {
        const auto combinerConfig = buildCombinerCfg(batch.state.combinerFunction);
        std::array<GLint, 32> config{};
        for (s32 i = 0; i < 32; ++i) {
            config[static_cast<std::size_t>(i)] = static_cast<GLint>(
                combinerConfig[static_cast<std::size_t>(i)]);
        }
        mParticleProgram->setUniformValueArray(configLocation, config.data(),
            static_cast<int>(config.size()));
        setupRenderState(gl, batch.state.blendFunction, batch.state.depthFunction);

        const TextureBinding textureBinding = bindTexture(gl, batch.state);
        const bool hasTexture = textureBinding.hasTexture;
        mParticleProgram->setUniformValue(texelLocation,
            textureBinding.texelWidth, textureBinding.texelHeight);
        mParticleProgram->setUniformValue(hasTextureLocation, hasTexture ? 1 : 0);

        Gfx::Color constant = batch.state.color;
        if (!hasTexture && constant.r() <= 0.0f && constant.g() <= 0.0f && constant.b() <= 0.0f) {
            constant = Gfx::Color{0.9f, 0.9f, 0.9f, 1.0f};
        }
        mParticleProgram->setUniformValue(constantColorLocation,
            constant.r(), constant.g(), constant.b(), 1.0f);

        for (const auto& particle : batch.particles) {
            Math::Vector3f axisX;
            Math::Vector3f axisY;
            computeParticleAxes(batch.state, particle, camera, axisX, axisY);

            const Math::Vector3f pivotOffset = Math::VectorCalc::add(
                Math::VectorCalc::multScalar(axisX, batch.state.rotationBasis.getX()),
                Math::VectorCalc::multScalar(axisY, batch.state.rotationBasis.getY()));
            const Math::Vector3f center = Math::VectorCalc::add(
                particle.positionDiff, pivotOffset);
            mParticleProgram->setUniformValue(centerLocation,
                center.getX(), center.getY(), center.getZ());
            mParticleProgram->setUniformValue(axisXLocation,
                axisX.getX(), axisX.getY(), axisX.getZ());
            mParticleProgram->setUniformValue(axisYLocation,
                axisY.getX(), axisY.getY(), axisY.getZ());
            mParticleProgram->setUniformValue(primaryColorLocation,
                particle.color.r(), particle.color.g(), particle.color.b(), particle.alpha);
            mParticleProgram->setUniformValue(uvOffsetLocation,
                particle.uvOffset.getX(), particle.uvOffset.getY());
            mParticleProgram->setUniformValue(uvScaleLocation,
                particle.uvSubOffset.getX(), particle.uvSubOffset.getY());

            gl.glDrawArrays(GL_TRIANGLES, 0, 6);
            ++drawCount;
        }
    }
    mParticleVao.release();
    mParticleProgram->release();
}

void PtclRenderer::renderCompact(QOpenGLFunctions_2_0& gl,
    const std::vector<CompactDrawBatch>& batches, const RenderCamera& camera,
    std::size_t& drawCount) {
    if (mCompactProgram == nullptr || batches.empty()) {
        return;
    }

    mCompactProgram->bind();
    const auto viewProjLocation = mCompactProgram->uniformLocation("uViewProj");
    const auto pointScaleLocation = mCompactProgram->uniformLocation("uPointScale");
    const auto configLocation = mCompactProgram->uniformLocation("uCfg");
    const auto constantColorLocation = mCompactProgram->uniformLocation("uConstantColor");
    const auto hasTextureLocation = mCompactProgram->uniformLocation("uHasTex");
    const auto wrapSLocation = mCompactProgram->uniformLocation("uWrapS");
    const auto wrapTLocation = mCompactProgram->uniformLocation("uWrapT");
    const auto texelLocation = mCompactProgram->uniformLocation("uTexel");
    const auto textureLocation = mCompactProgram->uniformLocation("uTex");

    gl.glUniformMatrix4fv(viewProjLocation, 1, GL_TRUE, camera.viewProjection.data());
    mCompactProgram->setUniformValue(pointScaleLocation, 1.2f);
    mCompactProgram->setUniformValue(textureLocation, 0);
    gl.glActiveTexture(GL_TEXTURE0);
    gl.glDisable(GL_CULL_FACE);

    mCompactVao.bind();
    mCompactVbo.bind();
    for (const auto& batch : batches) {
        if (batch.particles.empty()) {
            continue;
        }

        std::vector<std::array<GLfloat, 16>> vertices;
        vertices.reserve(batch.particles.size());
        for (const auto& particle : batch.particles) {
            vertices.push_back({
                particle.positionDiff.getX(),
                particle.positionDiff.getY(),
                particle.positionDiff.getZ(),
                particle.rotation,
                particle.scale.getX(),
                particle.scale.getY(),
                0.0f,
                0.0f,
                particle.color.r(),
                particle.color.g(),
                particle.color.b(),
                particle.alpha,
                particle.uvOffset.getX(),
                particle.uvOffset.getY(),
                particle.uvSubOffset.getX(),
                particle.uvSubOffset.getY(),
            });
        }
        mCompactVbo.allocate(vertices.data(),
            static_cast<int>(vertices.size() * sizeof(vertices[0])));

        setupRenderState(gl, batch.state.blendFunction, batch.state.depthFunction);
        const auto combinerConfig = buildCombinerCfg(batch.state.combinerFunction);
        std::array<GLint, 32> config{};
        for (s32 index = 0; index < 32; ++index) {
            config[static_cast<std::size_t>(index)] = static_cast<GLint>(
                combinerConfig[static_cast<std::size_t>(index)]);
        }
        mCompactProgram->setUniformValueArray(configLocation, config.data(),
            static_cast<int>(config.size()));

        const TextureBinding textureBinding = bindTexture(gl, batch.state);

        mCompactProgram->setUniformValue(hasTextureLocation, textureBinding.hasTexture ? 1 : 0);
        mCompactProgram->setUniformValue(wrapSLocation, static_cast<int>(batch.state.textureWrapS));
        mCompactProgram->setUniformValue(wrapTLocation, static_cast<int>(batch.state.textureWrapT));
        mCompactProgram->setUniformValue(texelLocation, textureBinding.texelWidth, textureBinding.texelHeight);

        Gfx::Color constant = batch.state.color;
        if (!textureBinding.hasTexture && constant.r() <= 0.0f && constant.g() <= 0.0f && constant.b() <= 0.0f) {
            constant = Gfx::Color{0.9f, 0.9f, 0.9f, 1.0f};
        }
        mCompactProgram->setUniformValue(constantColorLocation, constant.r(), constant.g(), constant.b(), 1.0f);

        const auto count = static_cast<GLsizei>(vertices.size());
        gl.glDrawArrays(GL_POINTS, 0, count);
        drawCount += static_cast<std::size_t>(count);
    }
    mCompactVbo.release();
    mCompactVao.release();
    mCompactProgram->release();
}

void PtclRenderer::renderStripes(QOpenGLFunctions_2_0& gl,
    const std::vector<EmitterDrawStripe>& stripes,
    const RenderCamera& camera) {
    if (mStripeProgram == nullptr) {
        return;
    }

    mStripeProgram->bind();
    const auto viewProjectionLocation = mStripeProgram->uniformLocation("uViewProj");
    const auto configLocation = mStripeProgram->uniformLocation("uCfg");
    const auto constantColorLocation = mStripeProgram->uniformLocation("uConstantColor");
    const auto primaryColorLocation = mStripeProgram->uniformLocation("uPrimaryColor");
    const auto hasTextureLocation = mStripeProgram->uniformLocation("uHasTex");
    const auto textureLocation = mStripeProgram->uniformLocation("uTex");
    gl.glUniformMatrix4fv(viewProjectionLocation, 1, GL_TRUE, camera.viewProjection.data());
    gl.glActiveTexture(GL_TEXTURE0);
    mStripeProgram->setUniformValue(textureLocation, 0);

    for (const auto& stripeEntry : stripes) {
        const auto& state = stripeEntry.state;
        const auto& points = stripeEntry.stripe.points;
        if (points.size() < 2) {
            continue;
        }

        const auto combinerConfig = buildCombinerCfg(state.combinerFunction);
        std::array<GLint, 32> config{};
        for (s32 i = 0; i < 32; ++i) {
            config[static_cast<std::size_t>(i)] = static_cast<GLint>(
                combinerConfig[static_cast<std::size_t>(i)]);
        }
        mStripeProgram->setUniformValueArray(configLocation, config.data(),
            static_cast<int>(config.size()));
        setupRenderState(gl, state.blendFunction, state.depthFunction);

        const TextureBinding textureBinding = bindTexture(gl, state);
        const bool hasTexture = textureBinding.hasTexture;
        mStripeProgram->setUniformValue(hasTextureLocation, hasTexture ? 1 : 0);

        Gfx::Color constant = state.secondaryColor;
        if (!hasTexture && constant.r() <= 0.0f && constant.g() <= 0.0f && constant.b() <= 0.0f) {
            constant = Gfx::Color{0.9f, 0.9f, 0.9f, 1.0f};
        }
        mStripeProgram->setUniformValue(constantColorLocation,
            constant.r(), constant.g(), constant.b(), 1.0f);
        mStripeProgram->setUniformValue(primaryColorLocation,
            state.color.r(), state.color.g(), state.color.b(), 1.0f);

        std::vector<StripeGLVertex> vertices;
        vertices.reserve(points.size() * 2);
        for (const auto& point : points) {
            Math::Vector3f edge0;
            Math::Vector3f edge1;
            if (state.billboardType == BillboardType::Stripe) {
                Math::Vector3f direction = point.outer;
                if (direction.length() <= 0.0001f) {
                    direction = camera.up;
                }
                Math::VectorCalc::normalize(direction);
                edge0 = Math::VectorCalc::cross(direction, camera.forward);
                if (edge0.length() <= 0.0001f) {
                    edge0 = camera.right;
                }
                Math::VectorCalc::normalize(edge0);
                edge0 = Math::VectorCalc::multScalar(edge0, point.scale * 0.5f);
                edge1 = Math::VectorCalc::multScalar(edge0, -1.0f);
            } else {
                edge0 = Math::VectorCalc::multScalar(point.outer, 0.5f);
                edge1 = Math::VectorCalc::multScalar(edge0, -1.0f);
            }

            const Math::Vector3f p0 = Math::VectorCalc::add(point.position, edge0);
            const Math::Vector3f p1 = Math::VectorCalc::add(point.position, edge1);
            const f32 u0 = point.uv.getX() + point.uvScale.getX();
            const f32 u1 = point.uv.getX();
            const f32 v = point.uv.getY() + point.t * point.uvScale.getY();
            vertices.push_back({p0.getX(), p0.getY(), p0.getZ(), u0, v, point.alpha});
            vertices.push_back({p1.getX(), p1.getY(), p1.getZ(), u1, v, point.alpha});
        }

        mStripeVbo.bind();
        mStripeVbo.allocate(vertices.data(),
            static_cast<int>(vertices.size() * sizeof(StripeGLVertex)));
        mStripeVbo.release();
        mStripeVao.bind();
        gl.glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(vertices.size()));
        mStripeVao.release();
    }

    mStripeProgram->release();
}

void PtclRenderer::cleanup(QOpenGLFunctions_2_0& gl) {
    cleanupTextures(gl);
    mParticleProgram.reset();
    mCompactProgram.reset();
    mStripeProgram.reset();
    mParticleVao.destroy();
    mParticleVbo.destroy();
    mCompactVao.destroy();
    mCompactVbo.destroy();
    mStripeVao.destroy();
    mStripeVbo.destroy();
}

void PtclRenderer::beginCalc() {
    mFrame.batches.clear();
    mFrame.stripes.clear();
    mFrame.compactBatches.clear();
}

void PtclRenderer::setEmitterState(const EmitterState& state) {
    mFrame.batches.push_back({state, {}});
}

void PtclRenderer::emitParticle(const ParticleState& particle) {
    if (mFrame.batches.empty()) {
        return;
    }
    mFrame.batches.back().particles.push_back(particle);
}

void PtclRenderer::emitStripe(const EmitterState& state, const StripeDraw& stripe) {
    mFrame.stripes.push_back({state, stripe});
}

void PtclRenderer::emitCompact(const CompactDrawBatch& batch) {
    mFrame.compactBatches.push_back(batch);
}


// ========================================================================== //


} // namespace Ptcl::Engine
