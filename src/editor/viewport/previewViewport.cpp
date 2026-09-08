#include "editor/viewport/previewViewport.h"

#include "ptcl/ptclEmitterSet.h"

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
out vec2 vUv;
void main() {
    vUv = aPos + 0.5;
    vec2 world = uOffset + aPos * uSize;
    vec2 ndc = (world - uPan) * uFactor;
    gl_Position = vec4(ndc, 0.0, 1.0);
}
)";

constexpr const char* cFragShader = R"(#version 330 core
in vec2 vUv;
uniform vec4 uColor;
out vec4 fragColor;
void main() {
    vec2 c = vUv - 0.5;
    float m = step(length(c), 0.5);
    fragColor = vec4(uColor.rgb, uColor.a * m);
}
)";

constexpr const char* cVertShaderEs = R"(#version 300 es
precision highp float;
in vec2 aPos;
uniform vec2 uOffset;
uniform vec2 uSize;
uniform vec2 uPan;
uniform vec2 uFactor;
out vec2 vUv;
void main() {
    vUv = aPos + 0.5;
    vec2 world = uOffset + aPos * uSize;
    vec2 ndc = (world - uPan) * uFactor;
    gl_Position = vec4(ndc, 0.0, 1.0);
}
)";

constexpr const char* cFragShaderEs = R"(#version 300 es
precision highp float;
in vec2 vUv;
uniform vec4 uColor;
out vec4 fragColor;
void main() {
    vec2 c = vUv - 0.5;
    float m = step(length(c), 0.5);
    fragColor = vec4(uColor.rgb, uColor.a * m);
}
)";


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
    if (mProgram) {
        mProgram->release();
        delete mProgram;
        mProgram = nullptr;
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

    if (mFrame.batches.empty()) {
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
    const int uColor = mProgram->uniformLocation("uColor");

    mProgram->setUniformValue(uPan, static_cast<GLfloat>(mPan.x()), static_cast<GLfloat>(mPan.y()));
    mProgram->setUniformValue(uFactor, factorX, factorY);

    mQuadVao.bind();

    const auto& batches = mFrame.batches;
    std::size_t drawCount = 0;
    for (const auto& batch : batches) {
        const Gfx::Color& tint = batch.state.color;
        Gfx::Color useColor = (tint.r() <= 0.0f && tint.g() <= 0.0f && tint.b() <= 0.0f)
            ? Gfx::Color{0.9f, 0.9f, 0.9f, 1.0f}
            : tint;

        switch (batch.state.blendFunction) {
        case Ptcl::BlendFuncType::Additive:
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        case Ptcl::BlendFuncType::Subtractive:
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        default:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        }

        for (const auto& particle : batch.particles) {
            const Math::Vector3f& pos = particle.positionDiff;
            const Math::Vector2f& scale = particle.scale;

            mProgram->setUniformValue(uOffset,
                static_cast<GLfloat>(pos.getX()),
                static_cast<GLfloat>(pos.getY()));
            mProgram->setUniformValue(uSize,
                static_cast<GLfloat>(scale.getX()),
                static_cast<GLfloat>(scale.getY()));
            mProgram->setUniformValue(uColor,
                static_cast<GLfloat>(useColor.r()),
                static_cast<GLfloat>(useColor.g()),
                static_cast<GLfloat>(useColor.b()),
                static_cast<GLfloat>(particle.alpha));

            glDrawArrays(GL_TRIANGLES, 0, 6);
            ++drawCount;
        }
    }
    mLastDrawCount = drawCount;

    mQuadVao.release();
    mProgram->release();

    glDisable(GL_BLEND);
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
