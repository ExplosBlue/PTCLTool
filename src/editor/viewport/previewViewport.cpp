#include "editor/viewport/previewViewport.h"

#include "ptcl/ptclDocument.h"

#include <QDebug>
#include <QSurfaceFormat>
#include <QWheelEvent>

#include <array>
#include <cstddef>


namespace PtclTool {


namespace {


constexpr const char* cScreenVertShader = R"(#version 150 core
in vec2 aPos;
void main() {
    gl_Position = vec4(aPos * 2.0, 0.0, 1.0);
}
)";

constexpr const char* cScreenFragShader = R"(#version 150 core
out vec4 fragColor;
void main() {
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
)";


} // namespace


// ========================================================================== //


PreviewViewport::PreviewViewport(QWidget* parent) :
    QOpenGLWidget{parent} {
    QSurfaceFormat fmt = format();
    fmt.setDepthBufferSize(24);
    setFormat(fmt);

    mSystem = std::make_unique<Ptcl::Engine::PtclSystem>(Ptcl::Engine::PtclSystemConfig{
        .regularParticleCapacity = 256,
        .compactParticleCapacity = 1024,
    });
    mCamera.setProjectionType(ViewportCamera::ProjectionType::Orthographic);

    setMinimumSize(320, 240);
    mTimer.setTimerType(Qt::PreciseTimer);
    mTimer.setInterval(1);
    connect(&mTimer, &QTimer::timeout, this, &PreviewViewport::stepFrame);
}

PreviewViewport::~PreviewViewport() {
    mTimer.stop();
    disconnect(context(), nullptr, this, nullptr);
}

void PreviewViewport::setPreview(const Ptcl::Document* document, const Ptcl::Selection& selection) {
    if (document == nullptr || selection.type() == Ptcl::Selection::Type::None) {
        clearPreview();
        return;
    }

    const s32 setIndex = selection.emitterSetIndex();
    const Ptcl::EmitterSet* set = (setIndex >= 0) ? document->emitterSet(setIndex) : nullptr;
    if (set == nullptr) {
        clearPreview();
        return;
    }

    mSystem->setEmitterSet(*set);
    mElapsedTimer.restart();
    mLastFrameNs = -cFrameIntervalNs;
    stepFrame();
    mTimer.start();
    update();
}

void PreviewViewport::clearPreview() {
    mTimer.stop();
    mSystem->reset();
    update();
}

void PreviewViewport::stepFrame() {
    const qint64 now = mElapsedTimer.nsecsElapsed();
    if (now - mLastFrameNs < cFrameIntervalNs) {
        return;
    }
    mLastFrameNs = now;
    mSystem->calc();
    update();
}

std::size_t PreviewViewport::lastFrameParticleCount() const {
    const Ptcl::Engine::EmitterDrawFrame& frame = mSystem->lastFrame();
    std::size_t count = 0;
    for (const auto& batch : frame.batches) {
        count += batch.particles.size();
    }
    for (const auto& batch : frame.compactBatches) {
        count += batch.particles.size();
    }
    return count;
}

void PreviewViewport::initializeGL() {
    initializeOpenGLFunctions();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this,
        &PreviewViewport::cleanupGlResources);

    ensureGlResources();
}

void PreviewViewport::cleanupGlResources() {
    if (!mGlReady) {
        return;
    }
    mSystem->cleanupRenderer(*this);
    if (mScreenProgram) {
        mScreenProgram->release();
        delete mScreenProgram;
        mScreenProgram = nullptr;
    }
    mGrid.cleanup();
    mQuadVao.destroy();
    mQuadVbo.destroy();
    mGlReady = false;
}

void PreviewViewport::ensureGlResources() {
    if (mGlReady) {
        return;
    }

    mSystem->initializeRenderer(*this);
    mScreenProgram = new QOpenGLShaderProgram(this);
    if (!mScreenProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, cScreenVertShader) ||
        !mScreenProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, cScreenFragShader) ||
        !mScreenProgram->link()) {
        qWarning() << "[viewport] screen shader compile/link failed:" << mScreenProgram->log();
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

    mGrid.initialize(*this);

    mGlReady = true;
}

void PreviewViewport::paintGL() {
    glViewport(0, 0, width(), height());
    glClearColor(0.09f, 0.09f, 0.11f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!mGlReady) {
        return;
    }

    const auto viewProj = mCamera.viewProjection(width(), height());
    const auto cameraBasis = mCamera.basis();

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

    mGrid.draw(*this, viewProj);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_FALSE);

    mSystem->render(
        *this, viewProj,
        cameraBasis.right, cameraBasis.up, cameraBasis.forward);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDisable(GL_DEPTH_TEST);
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
    const bool zoomIn = event->angleDelta().y() > 0;
    mCamera.zoom(zoomIn ? 0.1f : -0.1f);
    update();
    event->accept();
}

void PreviewViewport::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        mOrbitDragging = true;
        mLastMouse = event->pos();
        event->accept();
        return;
    }
    if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) {
        mPanDragging = true;
        mLastMouse = event->pos();
        event->accept();
        return;
    }
    QOpenGLWidget::mousePressEvent(event);
}

void PreviewViewport::mouseMoveEvent(QMouseEvent* event) {
    if (mOrbitDragging) {
        const QPoint delta = event->pos() - mLastMouse;
        mCamera.orbit(delta);
        mLastMouse = event->pos();
        update();
        event->accept();
        return;
    }
    if (mPanDragging) {
        const QPoint delta = event->pos() - mLastMouse;
        mCamera.pan(delta, height());
        mLastMouse = event->pos();
        update();
        event->accept();
        return;
    }
    QOpenGLWidget::mouseMoveEvent(event);
}

void PreviewViewport::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        mOrbitDragging = false;
        event->accept();
        return;
    }
    if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) {
        mPanDragging = false;
        event->accept();
        return;
    }
    QOpenGLWidget::mouseReleaseEvent(event);
}


// ========================================================================== //


} // namespace PtclTool
