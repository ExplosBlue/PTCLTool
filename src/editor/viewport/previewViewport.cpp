#include "editor/viewport/previewViewport.h"

#include "editor/components/combinerPreviewWidget.h"
#include "ptcl/ptclEmitterSet.h"
#include "ptcl/ptclTexture.h"

#include <QSurfaceFormat>
#include <QWheelEvent>

#include <array>


namespace PtclTool {


namespace {


constexpr const char* cVertShader = R"(#version 330 core
in vec2 aPos;
uniform vec2 uOffset;
uniform vec2 uSize;
uniform vec2 uPan;
uniform vec2 uFactor;
uniform vec2 uUvOffset;
uniform vec2 uUvScale;
out vec2 vUv;
void main() {
    vUv = uUvOffset + (aPos + 0.5) * uUvScale;
    vec2 world = uOffset + aPos * uSize;
    vec2 ndc = (world - uPan) * uFactor;
    gl_Position = vec4(ndc, 0.0, 1.0);
}
)";

constexpr const char* cFragShader = R"(#version 330 core
in vec2 vUv;
uniform sampler2D uTex;
uniform int uHasTex;
uniform vec4 uConstantColor;
uniform vec4 uPrimaryColor;
uniform int uCfg[32];
uniform float uAlpha;
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
        vec2 c = vUv - vec2(0.5);
        float m = step(length(c), 0.5);
        tex = vec4(vec3(1.0), m);
    }
    vec4 prev = vec4(0.0);
    vec3 cOut = evalColorStage(0, uConstantColor, uPrimaryColor, tex, prev);
    prev = vec4(cOut, 0.0);
    cOut = evalColorStage(8, uConstantColor, uPrimaryColor, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, 0.0);
    float aOut = evalAlphaStage(16, uConstantColor, uPrimaryColor, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, aOut);
    aOut = evalAlphaStage(24, uConstantColor, uPrimaryColor, tex, prev);
    fragColor = vec4(cOut, aOut * uAlpha);
}
)";

constexpr const char* cVertShaderEs = R"(#version 300 es
precision highp float;
in vec2 aPos;
uniform vec2 uOffset;
uniform vec2 uSize;
uniform vec2 uPan;
uniform vec2 uFactor;
uniform vec2 uUvOffset;
uniform vec2 uUvScale;
out vec2 vUv;
void main() {
    vUv = uUvOffset + (aPos + 0.5) * uUvScale;
    vec2 world = uOffset + aPos * uSize;
    vec2 ndc = (world - uPan) * uFactor;
    gl_Position = vec4(ndc, 0.0, 1.0);
}
)";

constexpr const char* cFragShaderEs = R"(#version 300 es
precision highp float;
in vec2 vUv;
uniform sampler2D uTex;
uniform int uHasTex;
uniform vec4 uConstantColor;
uniform vec4 uPrimaryColor;
uniform int uCfg[32];
uniform float uAlpha;
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
        vec2 c = vUv - vec2(0.5);
        float m = step(length(c), 0.5);
        tex = vec4(vec3(1.0), m);
    }
    vec4 prev = vec4(0.0);
    vec3 cOut = evalColorStage(0, uConstantColor, uPrimaryColor, tex, prev);
    prev = vec4(cOut, 0.0);
    cOut = evalColorStage(8, uConstantColor, uPrimaryColor, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, 0.0);
    float aOut = evalAlphaStage(16, uConstantColor, uPrimaryColor, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, aOut);
    aOut = evalAlphaStage(24, uConstantColor, uPrimaryColor, tex, prev);
    fragColor = vec4(cOut, aOut * uAlpha);
}
)";


constexpr const char* cScreenVertShader = R"(#version 330 core
in vec2 aPos;
void main() {
    gl_Position = vec4(aPos * 2.0, 0.0, 1.0);
}
)";

constexpr const char* cScreenFragShader = R"(#version 330 core
out vec4 fragColor;
void main() {
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
)";

constexpr const char* cScreenVertShaderEs = R"(#version 300 es
precision highp float;
in vec2 aPos;
void main() {
    gl_Position = vec4(aPos * 2.0, 0.0, 1.0);
}
)";

constexpr const char* cScreenFragShaderEs = R"(#version 300 es
precision highp float;
out vec4 fragColor;
void main() {
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
)";


std::array<GLint, 32> buildCombinerCfg(Ptcl::ColorCombinerFuncType func) {
    std::array<GLint, 32> cfgData{};
    const auto idx = static_cast<std::size_t>(func);
    if (idx >= Combiner::configurations.size()) {
        return cfgData;
    }

    const Combiner::Config& config = Combiner::configurations[idx];
    const Combiner::Stage* stages[4] = {
        &config.colorStage[0], &config.colorStage[1],
        &config.alphaStage[0], &config.alphaStage[1],
    };

    for (s32 s = 0; s < 4; ++s) {
        const Combiner::Stage& stage = *stages[s];
        const s32 base = s * 8;
        cfgData[base + 0] = static_cast<GLint>(stage.func);
        cfgData[base + 1] = static_cast<GLint>(stage.inputs[0]);
        cfgData[base + 2] = static_cast<GLint>(stage.inputs[1]);
        cfgData[base + 3] = static_cast<GLint>(stage.inputs[2]);
        cfgData[base + 4] = static_cast<GLint>(stage.operands[0]);
        cfgData[base + 5] = static_cast<GLint>(stage.operands[1]);
        cfgData[base + 6] = static_cast<GLint>(stage.operands[2]);
        cfgData[base + 7] = static_cast<GLint>(stage.scale);
    }
    return cfgData;
}

GLuint uploadTexture(const Ptcl::Texture& texture) {
    QImage img = texture.textureData();
    if (img.isNull()) {
        return 0;
    }
    img = img.convertToFormat(QImage::Format_RGBA8888);

    GLuint id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img.width(), img.height(), 0, GL_RGBA,
        GL_UNSIGNED_BYTE, img.constBits());
    return id;
}


} // namespace


// ========================================================================== //


PreviewViewport::PreviewViewport(QWidget* parent) :
    QOpenGLWidget{parent} {
    QSurfaceFormat fmt = format();
    fmt.setDepthBufferSize(24);
    setFormat(fmt);

    setMinimumSize(320, 240);
    mTimer.setInterval(16);
    connect(&mTimer, &QTimer::timeout, this, &PreviewViewport::stepFrame);
}

PreviewViewport::~PreviewViewport() {
    mTimer.stop();
    disconnect(context(), nullptr, this, nullptr);
}

void PreviewViewport::setEmitterSet(const Ptcl::EmitterSet* emitterSet) {
    mEmitterSet = emitterSet;
    if (emitterSet == nullptr) {
        clearPreview();
        return;
    }
    rebuild(*emitterSet);
    mTimer.start();
    update();
}

void PreviewViewport::clearPreview() {
    mTimer.stop();
    mEmitterSet = nullptr;
    mEmitters.clear();
    mSetInstance = {};
    mSystem.reset();
    mFrame.batches.clear();
    mLastDrawCount = 0;
    update();
}

void PreviewViewport::rebuild(const Ptcl::EmitterSet& emitterSet) {
    mTimer.stop();
    mEmitters.clear();
    mSetInstance = {};
    mSystem = std::make_unique<Ptcl::Engine::PtclSystem>(8);
    mFrame.batches.clear();

    u32 seed = 0x13579BDFu;
    for (const auto& res : emitterSet.emitters()) {
        std::unique_ptr<Ptcl::Engine::EmitterInstance> instance =
            std::make_unique<Ptcl::Engine::EmitterInstance>(*res, mSetInstance, seed, 3, 21, res->emitRate());
        seed = seed * 1664525u + 1013904223u;

        mSetInstance.addEmitter(*instance);
        mSystem->addEmitter(*instance);
        mEmitters.push_back(std::move(instance));
    }

    stepFrame();
    mTimer.start();
}

void PreviewViewport::beginCalc() {
    mFrame.batches.clear();
}

void PreviewViewport::setEmitterState(const Ptcl::Engine::EmitterState& state) {
    mFrame.batches.push_back({state, {}});
}

void PreviewViewport::emitParticle(const Ptcl::Engine::ParticleState& particle) {
    if (mFrame.batches.empty()) {
        return;
    }
    mFrame.batches.back().particles.push_back(particle);
}

void PreviewViewport::endCalc() {
}

void PreviewViewport::stepFrame() {
    if (mSystem == nullptr) {
        return;
    }
    mSystem->calc(*this);
    update();
}

std::size_t PreviewViewport::lastFrameParticleCount() const {
    std::size_t count = 0;
    for (const auto& batch : mFrame.batches) {
        count += batch.particles.size();
    }
    return count;
}

// ========================================================================== //


void PreviewViewport::initializeGL() {
    initializeOpenGLFunctions();
    mIsEs = QOpenGLContext::currentContext()->isOpenGLES();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this,
        &PreviewViewport::cleanupGlResources);

    std::fprintf(stderr, "[viewport] GL_VERSION=%s GLSL=%s es=%d\n",
        reinterpret_cast<const char*>(glGetString(GL_VERSION)),
        reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)),
        mIsEs ? 1 : 0);

    ensureGlResources();
}

void PreviewViewport::cleanupGlResources() {
    if (!mGlReady) {
        return;
    }
    for (const auto& [texture, id] : mTextureCache) {
        (void)texture;
        if (id != 0) {
            glDeleteTextures(1, &id);
        }
    }
    mTextureCache.clear();
    if (mProgram) {
        mProgram->release();
        delete mProgram;
        mProgram = nullptr;
    }
    if (mScreenProgram) {
        mScreenProgram->release();
        delete mScreenProgram;
        mScreenProgram = nullptr;
    }
    mQuadVao.destroy();
    mQuadVbo.destroy();
    mGlReady = false;
}

void PreviewViewport::ensureGlResources() {
    if (mGlReady) {
        return;
    }

    mProgram = new QOpenGLShaderProgram(this);
    if (!mProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, mIsEs ? cVertShaderEs : cVertShader) ||
        !mProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, mIsEs ? cFragShaderEs : cFragShader) ||
        !mProgram->link()) {
        std::fprintf(stderr, "[viewport] shader compile/link failed: %s\n",
            mProgram->log().toUtf8().constData());
    }

    mScreenProgram = new QOpenGLShaderProgram(this);
    if (!mScreenProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
            mIsEs ? cScreenVertShaderEs : cScreenVertShader) ||
        !mScreenProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
            mIsEs ? cScreenFragShaderEs : cScreenFragShader) ||
        !mScreenProgram->link()) {
        std::fprintf(stderr, "[viewport] screen shader compile/link failed: %s\n",
            mScreenProgram->log().toUtf8().constData());
    }

    static const std::array<GLfloat, 12> cQuad = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f,
    };

    mQuadVao.create();
    mQuadVbo.create();

    mQuadVao.bind();
    mQuadVbo.bind();
    mQuadVbo.allocate(cQuad.data(), static_cast<int>(sizeof(cQuad)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    mQuadVbo.release();
    mQuadVao.release();

    mGlReady = true;
}

void PreviewViewport::paintGL() {
    glViewport(0, 0, width(), height());
    glClearColor(0.09f, 0.09f, 0.11f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!mGlReady || !mProgram) {
        return;
    }

    const float aspect = (height() > 0) ? static_cast<float>(width()) / static_cast<float>(height()) : 1.0f;
    const float factorY = 2.0f / mZoom;
    const float factorX = factorY / aspect;

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    mProgram->bind();

    const int uOffset = mProgram->uniformLocation("uOffset");
    const int uSize = mProgram->uniformLocation("uSize");
    const int uPan = mProgram->uniformLocation("uPan");
    const int uFactor = mProgram->uniformLocation("uFactor");
    const int uCfg = mProgram->uniformLocation("uCfg");
    const int uConstantColor = mProgram->uniformLocation("uConstantColor");
    const int uPrimaryColor = mProgram->uniformLocation("uPrimaryColor");
    const int uHasTex = mProgram->uniformLocation("uHasTex");
    const int uAlpha = mProgram->uniformLocation("uAlpha");
    const int uUvOffset = mProgram->uniformLocation("uUvOffset");
    const int uUvScale = mProgram->uniformLocation("uUvScale");

    mProgram->setUniformValue(uPan, static_cast<GLfloat>(mPan.x()), static_cast<GLfloat>(mPan.y()));
    mProgram->setUniformValue(uFactor, factorX, factorY);

    glActiveTexture(GL_TEXTURE0);
    mQuadVao.bind();

    const auto& batches = mFrame.batches;
    std::size_t drawCount = 0;
    for (const auto& batch : batches) {
        const std::array<GLint, 32> cfgData = buildCombinerCfg(batch.state.combinerFunction);
        mProgram->setUniformValueArray(uCfg, cfgData.data(), static_cast<int>(cfgData.size()));

        switch (batch.state.blendFunction) {
        case Ptcl::BlendFuncType::Additive:
            glBlendEquation(GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE);
            break;
        case Ptcl::BlendFuncType::Subtractive:
            glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE);
            break;
        case Ptcl::BlendFuncType::Transparence:
            glBlendEquation(GL_FUNC_ADD);
            glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
            break;
        default:
            glBlendEquation(GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
            break;
        }

        const Ptcl::Texture* texture = nullptr;
        if (batch.state.textureHandle != nullptr && batch.state.textureHandle->isValid()) {
            texture = batch.state.textureHandle->get();
        }
        const bool hasTexture = (texture != nullptr && !texture->isPlaceholder());

        mProgram->setUniformValue(uHasTex, hasTexture ? 1 : 0);
        if (hasTexture) {
            GLuint texId = 0;
            const auto cacheIt = mTextureCache.find(texture);
            if (cacheIt != mTextureCache.end()) {
                texId = cacheIt->second;
            } else {
                texId = uploadTexture(*texture);
                if (texId != 0) {
                    mTextureCache.emplace(texture, texId);
                }
            }
            if (texId != 0) {
                glBindTexture(GL_TEXTURE_2D, texId);
                const GLint wrapS = (batch.state.textureWrapS == Ptcl::TextureWrap::Repeat) ? GL_REPEAT
                    : (batch.state.textureWrapS == Ptcl::TextureWrap::MirroredRepeat) ? GL_MIRRORED_REPEAT
                    : GL_CLAMP_TO_EDGE;
                const GLint wrapT = (batch.state.textureWrapT == Ptcl::TextureWrap::Repeat) ? GL_REPEAT
                    : (batch.state.textureWrapT == Ptcl::TextureWrap::MirroredRepeat) ? GL_MIRRORED_REPEAT
                    : GL_CLAMP_TO_EDGE;
                const GLint filter = (batch.state.textureFilter == Ptcl::TextureFilter::Nearest)
                    ? GL_NEAREST : GL_LINEAR;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
            } else {
                mProgram->setUniformValue(uHasTex, 0);
            }
        }

        Gfx::Color constant = batch.state.color;
        if (!hasTexture && constant.r() <= 0.0f && constant.g() <= 0.0f && constant.b() <= 0.0f) {
            constant = Gfx::Color{0.9f, 0.9f, 0.9f, 1.0f};
        }
        mProgram->setUniformValue(uConstantColor,
            static_cast<GLfloat>(constant.r()),
            static_cast<GLfloat>(constant.g()),
            static_cast<GLfloat>(constant.b()),
            1.0f);

        for (const auto& particle : batch.particles) {
            const Math::Vector3f& pos = particle.positionDiff;
            const Math::Vector2f& scale = particle.scale;

            mProgram->setUniformValue(uPrimaryColor,
                static_cast<GLfloat>(particle.color.r()),
                static_cast<GLfloat>(particle.color.g()),
                static_cast<GLfloat>(particle.color.b()),
                1.0f);
            mProgram->setUniformValue(uOffset,
                static_cast<GLfloat>(pos.getX()),
                static_cast<GLfloat>(pos.getY()));
            mProgram->setUniformValue(uSize,
                static_cast<GLfloat>(scale.getX()),
                static_cast<GLfloat>(scale.getY()));
            mProgram->setUniformValue(uUvOffset,
                static_cast<GLfloat>(particle.uvOffset.getX()),
                static_cast<GLfloat>(particle.uvOffset.getY()));
            mProgram->setUniformValue(uUvScale,
                static_cast<GLfloat>(particle.uvSubOffset.getX()),
                static_cast<GLfloat>(particle.uvSubOffset.getY()));
            mProgram->setUniformValue(uAlpha, static_cast<GLfloat>(particle.alpha));

            glDrawArrays(GL_TRIANGLES, 0, 6);
            ++drawCount;
        }
    }
    mLastDrawCount = drawCount;

    mQuadVao.release();
    mProgram->release();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);

    if (mScreenProgram) {
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
        mScreenProgram->bind();
        mQuadVao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        mQuadVao.release();
        mScreenProgram->release();
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
}

void PreviewViewport::resizeGL(int, int) {
    update();
}

void PreviewViewport::wheelEvent(QWheelEvent* event) {
    const float factor = event->angleDelta().y() > 0 ? 0.9f : 1.1f;
    mZoom *= factor;
    mZoom = qBound(0.5f, mZoom, 500.0f);
    update();
    event->accept();
}

void PreviewViewport::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        mDragging = true;
        mLastMouse = event->pos();
        event->accept();
        return;
    }
    QOpenGLWidget::mousePressEvent(event);
}

void PreviewViewport::mouseMoveEvent(QMouseEvent* event) {
    if (mDragging) {
        QPointF delta = event->pos() - mLastMouse;
        mPan.rx() -= delta.x() * mZoom / static_cast<float>(width());
        mPan.ry() += delta.y() * mZoom / static_cast<float>(height());
        mLastMouse = event->pos();
        update();
        event->accept();
        return;
    }
    QOpenGLWidget::mouseMoveEvent(event);
}


// ========================================================================== //


} // namespace PtclTool
