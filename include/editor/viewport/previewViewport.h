#pragma once

#include "editor/viewport/viewportCamera.h"
#include "editor/viewport/viewportGizmo.h"
#include "editor/viewport/viewportGrid.h"
#include "ptcl/engine/ptclSystem.h"
#include "ptcl/ptclDocument.h"
#include "ptcl/renderer/renderer.h"

#include <QOpenGLBuffer>
#include <QOpenGLFunctions_2_0>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QString>
#include <QTimer>

#include <memory>


// ========================================================================== //


class QPainter;


namespace Ptcl {
class Document;
class EmitterSet;
class Selection;
} // namespace Ptcl


namespace PtclTool {


// ========================================================================== //


struct ViewportStats {
    bool active{false};
    f32 fps{0.0f};
    s32 particles{0};
    s32 particleCapacity{0};
    f32 simulationMs{0.0f};
};


// ========================================================================== //


class PreviewViewport final : public QOpenGLWidget, protected QOpenGLFunctions_2_0 {
    Q_OBJECT
public:
    explicit PreviewViewport(QWidget* parent = nullptr);
    ~PreviewViewport() final;

    void setPreview(const Ptcl::Document* document, const Ptcl::Selection& selection);
    void clearPreview();
    void scheduleRefresh();

    bool isPlaying() const { return mIsPlaying; }
    bool isLooping() const { return mLooping; }
    f32 playbackSpeed() const { return mPlaybackSpeed; }
    ViewportCamera::ProjectionType projection() const { return mCamera.projectionType(); }
    bool isGridVisible() const { return mGridVisible; }

public slots:
    void play();
    void pause();
    void togglePlayback();
    void restart();
    void step();
    void setPlaybackSpeed(f32 speed);
    void setLooping(bool looping);
    void setProjection(ViewportCamera::ProjectionType type);
    void toggleProjection();
    void setViewDirection(ViewportCamera::ViewDirection direction);
    void resetCamera();
    void setGridVisible(bool visible);

signals:
    void playbackStateChanged(bool playing);
    void frameChanged(s32 frame);
    void statsChanged(const ViewportStats& stats);
    void projectionChanged(bool orthographic);
    void gridVisibleChanged(bool visible);
    void loopingChanged(bool looping);

protected:
    void initializeGL() final;
    void paintGL() final;
    void resizeGL(int w, int h) final;

    void wheelEvent(QWheelEvent* event) final;
    void mousePressEvent(QMouseEvent* event) final;
    void mouseMoveEvent(QMouseEvent* event) final;
    void mouseReleaseEvent(QMouseEvent* event) final;
    void leaveEvent(QEvent* event) final;

private:
    void ensureGlResources();
    void cleanupGlResources();
    void drawOpaqueOverlay();

    void drawViewportOverlay(QPainter& painter);
    void drawViewContext(QPainter& painter);
    QString viewLabel() const;
    QString contextLabel() const;
    void updateGizmoHover(s32 axis);

    void startViewTransition(ViewportCamera::ViewDirection direction);
    void onViewTransitionTick();
    void cancelViewTransition();
    void leaveTemporaryAxisProjection();

    void onTick();
    void simulateFrame();
    void refreshPreview();
    bool reloadCurrentSet();
    void updateStats();
    void resetFpsWindow();

    const Ptcl::EmitterSet* currentEmitterSet() const;

    QTimer mTimer{};
    QTimer mRefreshTimer{};
    QTimer mStatsTimer{};
    QTimer mViewTransitionTimer{};
    QElapsedTimer mViewTransitionElapsed{};
    ViewportCamera::Orientation mViewTransitionStart{};
    ViewportCamera::Orientation mViewTransitionTarget{};
    bool mViewTransitionActive{false};

    std::unique_ptr<Ptcl::Engine::PtclSystem> mSystem{};
    Ptcl::Render::Renderer mRenderer{};
    static constexpr qint64 cFrameIntervalNs = 1000000000LL / 60;

    QElapsedTimer mElapsedTimer{};
    QElapsedTimer mSimulationTimer{};
    qint64 mLastTickNs{0};
    qint64 mSimulationAccumulatorNs{0};
    f32 mSimulationMs{0.0f};

    bool mIsPlaying{false};
    bool mLooping{true};
    f32 mPlaybackSpeed{1.0f};
    bool mHasEmitters{false};

    const Ptcl::Document* mDocument{nullptr};
    s32 mSetIndex{-1};
    s32 mEmitterIndex{-1};
    Ptcl::Selection::Type mSelectionType{Ptcl::Selection::Type::None};

    qint64 mLastPaintNs{0};
    qint64 mFpsWindowNs{0};
    s32 mFpsFrameCount{0};
    f32 mFps{0.0f};

    QOpenGLShaderProgram* mScreenProgram{nullptr};
    QOpenGLBuffer mQuadVbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject mQuadVao{};
    ViewportGrid mGrid{};
    bool mGridVisible{true};
    bool mGlReady{false};

    ViewportCamera mCamera{};
    bool mTemporaryAxisOrthographic{false};
    ViewportCamera::ProjectionType mProjectionBeforeAxisView{
        ViewportCamera::ProjectionType::Perspective
    };
    ViewportGizmo mGizmo{};
    bool mOrbitDragging{false};
    bool mPanDragging{false};
    QPoint mLastMouse{};
};


// ========================================================================== //


} // namespace PtclTool
