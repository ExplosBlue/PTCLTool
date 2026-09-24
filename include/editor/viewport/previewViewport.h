#pragma once

#include "ptcl/engine/emitterInstance.h"
#include "ptcl/engine/emitterSetInstance.h"
#include "ptcl/engine/ptclRenderer.h"
#include "ptcl/engine/ptclSystem.h"
#include "editor/viewport/viewportCamera.h"
#include "editor/viewport/viewportGrid.h"

#include <QOpenGLBuffer>
#include <QOpenGLFunctions_2_0>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTimer>

#include <memory>
#include <unordered_map>


// ========================================================================== //


namespace Ptcl {
class Document;
class EmitterSet;
class Selection;
class Texture;
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

    std::size_t lastDrawCount() const { return mLastDrawCount; }
    std::size_t lastFrameParticleCount() const;
    s32 lastFrameAliveCount() const { return system() ? system()->lastFrameAliveCount() : 0; }
    bool hasFrame() const { return system() && !system()->lastFrame().batches.empty(); }

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
    void createEmitterSet(const Ptcl::EmitterSet& emitterSet);
    void ensureGlResources();
    void cleanupGlResources();

    Ptcl::Engine::PtclSystem* system() { return mSystem.get(); }
    const Ptcl::Engine::PtclSystem* system() const { return mSystem.get(); }

    QTimer mTimer{};

    const Ptcl::EmitterSet* mEmitterSet{nullptr};
    std::unique_ptr<Ptcl::Engine::PtclSystem> mSystem{};
    Ptcl::Engine::EmitterSetInstance mSetInstance{};
    std::vector<std::unique_ptr<Ptcl::Engine::EmitterInstance>> mEmitters{};
    static constexpr qint64 cFrameIntervalNs = 1000000000LL / 60;

    QElapsedTimer mElapsedTimer{};
    qint64 mLastFrameNs{0};

    QOpenGLShaderProgram* mProgram{nullptr};
    QOpenGLShaderProgram* mScreenProgram{nullptr};
    QOpenGLShaderProgram* mStripeProgram{nullptr};
    QOpenGLBuffer mQuadVbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject mQuadVao{};
    QOpenGLBuffer mStripeVbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject mStripeVao{};
    ViewportGrid mGrid{};
    std::unordered_map<const Ptcl::Texture*, GLuint> mTextureCache{};
    bool mGlReady{false};
    std::size_t mLastDrawCount{0};

    ViewportCamera mCamera{};
    bool mOrbitDragging{false};
    bool mPanDragging{false};
    QPoint mLastMouse{};
};


// ========================================================================== //


} // namespace PtclTool
