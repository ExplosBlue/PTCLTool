#include "editor/viewport/previewViewport.h"

#include "gfx/glGeometry.h"
#include "ptcl/ptclDocument.h"

#include <QDebug>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QWheelEvent>


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
    if (QOpenGLContext* glContext = context(); glContext != nullptr && glContext->isValid()) {
        makeCurrent();
        cleanupGlResources();
        doneCurrent();
    }
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

void PreviewViewport::drawOpaqueOverlay() {
    if (!mScreenProgram) {
        return;
    }

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
    mScreenProgram->bind();
    mQuadVao.bind();

    glDrawArrays(GL_TRIANGLES, 0, Gfx::unitQuadVertexCount);

    mQuadVao.release();
    mScreenProgram->release();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
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
    mRenderer.cleanup(*this);
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

    mRenderer.initialize(*this);
    mScreenProgram = new QOpenGLShaderProgram(this);
    mScreenProgram->bindAttributeLocation("aPos", 0);
    if (!mScreenProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, cScreenVertShader) ||
        !mScreenProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, cScreenFragShader) ||
        !mScreenProgram->link()) {
        qWarning() << "[viewport] screen shader compile/link failed:" << mScreenProgram->log();
        delete mScreenProgram;
        mScreenProgram = nullptr;
    }

    Gfx::createUnitQuad(*this, mQuadVbo, mQuadVao);
    mGrid.initialize(*this);

    mGlReady = true;
}

void PreviewViewport::paintGL() {
    const QSize viewportSize = size() * devicePixelRatioF();
    glViewport(0, 0, viewportSize.width(), viewportSize.height());
    glClearColor(0.094f, 0.094f, 0.388f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!mGlReady) {
        return;
    }

    const Math::Matrix44f& viewProj = mCamera.viewProjection();

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

    mGrid.draw(*this, viewProj);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_FALSE);

    mRenderer.render(*this, mSystem->lastFrame(), mCamera);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    drawOpaqueOverlay();
}

void PreviewViewport::resizeGL(int w, int h) {
    mCamera.setViewportSize(w, h);
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
        mCamera.pan(delta);
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
