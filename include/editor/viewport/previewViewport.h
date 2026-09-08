#pragma once

#include "ptcl/engine/emitterInstance.h"
#include "ptcl/engine/emitterSetInstance.h"
#include "ptcl/engine/ptclRenderer.h"
#include "ptcl/engine/ptclSystem.h"

#include <QOpenGLBuffer>
#include <QOpenGLFunctions_2_0>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QTimer>

#include <memory>


// ========================================================================== //


namespace Ptcl {
class EmitterSet;
} // namespace Ptcl


namespace PtclTool {


// ========================================================================== //


class PreviewViewport final : public QOpenGLWidget, protected QOpenGLFunctions_2_0, public Ptcl::Engine::PtclRenderer {
    Q_OBJECT
public:
    explicit PreviewViewport(QWidget* parent = nullptr);
    ~PreviewViewport() final;

    void setEmitterSet(const Ptcl::EmitterSet* emitterSet);
    void clearPreview();

    std::size_t lastDrawCount() const { return mLastDrawCount; }
    std::size_t lastFrameParticleCount() const;
    s32 lastFrameAliveCount() const { return system() ? system()->lastFrameAliveCount() : 0; }
    bool hasFrame() const { return !mFrame.batches.empty(); }

    void beginCalc() override;
    void setEmitterState(const Ptcl::Engine::EmitterState& state) override;
    void emitParticle(const Ptcl::Engine::ParticleState& particle) override;
    void endCalc() override;

public slots:
    void stepFrame();

protected:
    void initializeGL() final;
    void paintGL() final;
    void resizeGL(int w, int h) final;

    void wheelEvent(QWheelEvent* event) final;
    void mousePressEvent(QMouseEvent* event) final;
    void mouseMoveEvent(QMouseEvent* event) final;

private:
    void rebuild(const Ptcl::EmitterSet& emitterSet);
    void ensureGlResources();
    void cleanupGlResources();

    Ptcl::Engine::PtclSystem* system() { return mSystem.get(); }
    const Ptcl::Engine::PtclSystem* system() const { return mSystem.get(); }

    QTimer mTimer{};

    const Ptcl::EmitterSet* mEmitterSet{nullptr};
    std::unique_ptr<Ptcl::Engine::PtclSystem> mSystem{};
    Ptcl::Engine::EmitterSetInstance mSetInstance{};
    std::vector<std::unique_ptr<Ptcl::Engine::EmitterInstance>> mEmitters{};
    Ptcl::Engine::EmitterDrawFrame mFrame{};

    QOpenGLShaderProgram* mProgram{nullptr};
    QOpenGLBuffer mQuadVbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject mQuadVao{};
    bool mIsEs{false};
    bool mGlReady{false};
    std::size_t mLastDrawCount{0};

    float mZoom{60.0f};
    QPointF mPan{};
    bool mDragging{false};
    QPoint mLastMouse{};
};


// ========================================================================== //


} // namespace PtclTool
