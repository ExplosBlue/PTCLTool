#pragma once

#include "editor/viewport/viewportCamera.h"
#include "editor/viewport/viewportGrid.h"
#include "ptcl/engine/ptclSystem.h"

#include <QOpenGLBuffer>
#include <QOpenGLFunctions_2_0>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTimer>

#include <memory>


// ========================================================================== //


namespace Ptcl {
class Document;
class EmitterSet;
class Selection;
} // namespace Ptcl


namespace PtclTool {


// ========================================================================== //


class PreviewViewport final : public QOpenGLWidget, protected QOpenGLFunctions_2_0 {
    Q_OBJECT
public:
    explicit PreviewViewport(QWidget* parent = nullptr);
    ~PreviewViewport() final;

    void setPreview(const Ptcl::Document* document, const Ptcl::Selection& selection);
    void clearPreview();

    std::size_t lastDrawCount() const { return mSystem ? mSystem->lastDrawCount() : 0; }
    std::size_t lastFrameParticleCount() const;
    s32 lastFrameAliveCount() const { return mSystem ? mSystem->lastFrameAliveCount() : 0; }
    bool hasFrame() const { return mSystem && !mSystem->lastFrame().batches.empty(); }

public slots:
    void stepFrame();

protected:
    void initializeGL() final;
    void paintGL() final;
    void resizeGL(int w, int h) final;

    void wheelEvent(QWheelEvent* event) final;
    void mousePressEvent(QMouseEvent* event) final;
    void mouseMoveEvent(QMouseEvent* event) final;
    void mouseReleaseEvent(QMouseEvent* event) final;

private:
    void ensureGlResources();
    void cleanupGlResources();

    QTimer mTimer{};

    std::unique_ptr<Ptcl::Engine::PtclSystem> mSystem{};
    static constexpr qint64 cFrameIntervalNs = 1000000000LL / 60;

    QElapsedTimer mElapsedTimer{};
    qint64 mLastFrameNs{0};

    QOpenGLShaderProgram* mScreenProgram{nullptr};
    QOpenGLBuffer mQuadVbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject mQuadVao{};
    ViewportGrid mGrid{};
    bool mGlReady{false};

    ViewportCamera mCamera{};
    bool mOrbitDragging{false};
    bool mPanDragging{false};
    QPoint mLastMouse{};
};


// ========================================================================== //


} // namespace PtclTool
