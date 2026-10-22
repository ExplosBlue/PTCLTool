#include "editor/viewport/previewViewport.h"

#include "gfx/glGeometry.h"
#include "ptcl/ptclDocument.h"
#include "util/settingsUtil.h"

#include <QColor>
#include <QCursor>
#include <QDebug>
#include <QEvent>
#include <QFont>
#include <QFontMetrics>
#include <QKeySequence>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QPainter>
#include <QShortcut>
#include <QSurfaceFormat>
#include <QToolTip>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>
#include <numbers>


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

constexpr qint64 cViewTransitionDurationNs = 240'000'000LL;
constexpr s32 cViewTransitionIntervalMs = 16;


} // namespace


// ========================================================================== //


PreviewViewport::PreviewViewport(QWidget* parent) :
    QOpenGLWidget{parent} {
    QSurfaceFormat fmt = format();
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    fmt.setSamples(4);
    setFormat(fmt);

    mSystem = std::make_unique<Ptcl::Engine::PtclSystem>(Ptcl::Engine::PtclSystemConfig{
        .regularParticleCapacity = 256,
        .compactParticleCapacity = 1024,
    });
    mCamera.setProjectionType(SettingsUtil::viewportOrthographic()
        ? ViewportCamera::ProjectionType::Orthographic
        : ViewportCamera::ProjectionType::Perspective
    );
    mGridVisible = SettingsUtil::viewportGridVisible();
    mPlaybackSpeed = std::clamp(SettingsUtil::viewportPlaybackSpeed(), 0.1f, 4.0f);
    mLooping = SettingsUtil::viewportLooping();

    setMinimumSize(320, 240);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    mElapsedTimer.start();

    mTimer.setTimerType(Qt::PreciseTimer);
    mTimer.setInterval(1);
    connect(&mTimer, &QTimer::timeout, this, &PreviewViewport::onTick);

    mRefreshTimer.setSingleShot(true);
    mRefreshTimer.setInterval(120);
    connect(&mRefreshTimer, &QTimer::timeout, this, &PreviewViewport::refreshPreview);

    mViewTransitionTimer.setTimerType(Qt::PreciseTimer);
    mViewTransitionTimer.setInterval(cViewTransitionIntervalMs);
    connect(&mViewTransitionTimer, &QTimer::timeout, this, &PreviewViewport::onViewTransitionTick);

    mStatsTimer.setInterval(250);
    connect(&mStatsTimer, &QTimer::timeout, this, &PreviewViewport::updateStats);
    mStatsTimer.start();

    const auto addShortcut = [this](const QKeySequence& key, auto slot) {
        auto* shortcut = new QShortcut(key, this);
        shortcut->setContext(Qt::WidgetWithChildrenShortcut);
        connect(shortcut, &QShortcut::activated, this, slot);
    };

    const auto numpad = [](Qt::Key key, Qt::KeyboardModifiers modifiers = Qt::NoModifier) {
        return QKeySequence(QKeyCombination(modifiers | Qt::KeypadModifier, key));
    };

    addShortcut(QKeySequence(Qt::Key_Space), &PreviewViewport::togglePlayback);
    addShortcut(QKeySequence(Qt::SHIFT | Qt::Key_Left), &PreviewViewport::restart);
    addShortcut(QKeySequence(Qt::Key_Right), &PreviewViewport::step);

    addShortcut(numpad(Qt::Key_1), [this] {
        setViewDirection(ViewportCamera::ViewDirection::Front);
    });

    addShortcut(numpad(Qt::Key_1, Qt::ControlModifier), [this] {
        setViewDirection(ViewportCamera::ViewDirection::Back);
    });

    addShortcut(numpad(Qt::Key_3), [this] {
        setViewDirection(ViewportCamera::ViewDirection::Right);
    });

    addShortcut(numpad(Qt::Key_3, Qt::ControlModifier), [this] {
        setViewDirection(ViewportCamera::ViewDirection::Left);
    });

    addShortcut(numpad(Qt::Key_7), [this] {
        setViewDirection(ViewportCamera::ViewDirection::Top);
    });

    addShortcut(numpad(Qt::Key_7, Qt::ControlModifier), [this] {
        setViewDirection(ViewportCamera::ViewDirection::Bottom);
    });

    addShortcut(numpad(Qt::Key_5), &PreviewViewport::toggleProjection);
    addShortcut(QKeySequence(Qt::SHIFT | Qt::ALT | Qt::Key_Z), [this] {
        setGridVisible(!mGridVisible);
    });
}

PreviewViewport::~PreviewViewport() {
    mTimer.stop();
    mRefreshTimer.stop();
    mStatsTimer.stop();
    mViewTransitionTimer.stop();
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

    const bool sameSet = (document == mDocument && setIndex == mSetIndex);
    const bool hadPreview = (mDocument != nullptr);
    const bool wasPlaying = mIsPlaying;

    mDocument = document;
    mSetIndex = setIndex;
    mEmitterIndex = selection.emitterIndex();
    mSelectionType = selection.type();

    if (sameSet) {
        update();
        return;
    }

    mRefreshTimer.stop();

    if (!reloadCurrentSet()) {
        clearPreview();
        return;
    }

    simulateFrame();

    if (!hadPreview || wasPlaying) {
        if (!mIsPlaying) {
            play();
        } else {
            mLastTickNs = mElapsedTimer.nsecsElapsed();
            mSimulationAccumulatorNs = 0;
            mTimer.start();
        }
    } else {
        mTimer.stop();
    }

    update();
    updateStats();
}

void PreviewViewport::clearPreview() {
    mTimer.stop();
    mRefreshTimer.stop();
    mViewTransitionTimer.stop();
    mViewTransitionActive = false;
    mIsPlaying = false;
    mDocument = nullptr;
    mSetIndex = -1;
    mEmitterIndex = -1;
    mSelectionType = Ptcl::Selection::Type::None;
    mHasEmitters = false;
    mSimulationAccumulatorNs = 0;
    mLastTickNs = 0;
    mSimulationMs = 0.0f;
    mFps = 0.0f;
    resetFpsWindow();

    mSystem->reset();

    emit playbackStateChanged(false);
    emit frameChanged(0);
    updateStats();
    update();
}

void PreviewViewport::refreshPreview() {
    if (!reloadCurrentSet()) {
        clearPreview();
        return;
    }

    const bool wasPlaying = mIsPlaying;
    simulateFrame();

    if (wasPlaying) {
        mLastTickNs = mElapsedTimer.nsecsElapsed();
        mSimulationAccumulatorNs = 0;
        mTimer.start();
    } else {
        mTimer.stop();
    }

    update();
}

void PreviewViewport::scheduleRefresh() {
    if (mDocument != nullptr) {
        mRefreshTimer.start();
    }
}

void PreviewViewport::play() {
    if (mIsPlaying || mDocument == nullptr) {
        return;
    }

    mIsPlaying = true;
    mLastTickNs = mElapsedTimer.nsecsElapsed();
    mSimulationAccumulatorNs = 0;
    resetFpsWindow();
    mTimer.start();

    emit playbackStateChanged(true);
}

void PreviewViewport::pause() {
    if (!mIsPlaying) {
        return;
    }

    mIsPlaying = false;
    mTimer.stop();
    resetFpsWindow();

    emit playbackStateChanged(false);
}

void PreviewViewport::togglePlayback() {
    if (mIsPlaying) {
        pause();
    } else {
        play();
    }
}

void PreviewViewport::restart() {
    mRefreshTimer.stop();
    refreshPreview();
}

void PreviewViewport::step() {
    if (mDocument == nullptr) {
        return;
    }

    if (mIsPlaying) {
        pause();
    }

    simulateFrame();
}

void PreviewViewport::setPlaybackSpeed(f32 speed) {
    mPlaybackSpeed = std::clamp(speed, 0.1f, 4.0f);
    SettingsUtil::setViewportPlaybackSpeed(mPlaybackSpeed);
    mSimulationAccumulatorNs = 0;
    mLastTickNs = mElapsedTimer.nsecsElapsed();
}

void PreviewViewport::setLooping(bool looping) {
    if (mLooping == looping) {
        return;
    }

    mLooping = looping;
    SettingsUtil::setViewportLooping(looping);
    emit loopingChanged(looping);
}

void PreviewViewport::setProjection(ViewportCamera::ProjectionType type) {
    if (mCamera.projectionType() == type) {
        if (mTemporaryAxisOrthographic
            && type == ViewportCamera::ProjectionType::Orthographic) {
            mTemporaryAxisOrthographic = false;
            SettingsUtil::setViewportOrthographic(true);
        }
        return;
    }

    mTemporaryAxisOrthographic = false;
    mCamera.setProjectionType(type);
    SettingsUtil::setViewportOrthographic(type == ViewportCamera::ProjectionType::Orthographic);

    emit projectionChanged(type == ViewportCamera::ProjectionType::Orthographic);
    update();
}

void PreviewViewport::toggleProjection() {
    const auto type = mCamera.projectionType();
    setProjection(type == ViewportCamera::ProjectionType::Orthographic
        ? ViewportCamera::ProjectionType::Perspective
        : ViewportCamera::ProjectionType::Orthographic);
}

void PreviewViewport::startViewTransition(ViewportCamera::ViewDirection direction) {
    cancelViewTransition();

    const ViewportCamera::Orientation start = mCamera.orientation();
    ViewportCamera::Orientation target = ViewportCamera::orientationFor(direction);

    constexpr f32 twoPi = std::numbers::pi_v<f32> * 2.0f;
    target.yaw = start.yaw + std::remainder(target.yaw - start.yaw, twoPi);

    mViewTransitionStart = start;
    mViewTransitionTarget = target;
    mViewTransitionElapsed.restart();
    mViewTransitionActive = true;
    mViewTransitionTimer.start();

    if (!mTemporaryAxisOrthographic
        && mCamera.projectionType() == ViewportCamera::ProjectionType::Perspective) {
        mProjectionBeforeAxisView = mCamera.projectionType();
        mTemporaryAxisOrthographic = true;
        mCamera.setProjectionType(ViewportCamera::ProjectionType::Orthographic);
        emit projectionChanged(true);
    }

    update();
}

void PreviewViewport::onViewTransitionTick() {
    if (!mViewTransitionActive) {
        mViewTransitionTimer.stop();
        return;
    }

    const f32 progress = std::clamp(
        static_cast<f32>(mViewTransitionElapsed.nsecsElapsed())
            / static_cast<f32>(cViewTransitionDurationNs),
        0.0f,
        1.0f
    );
    const f32 eased = progress * progress * (3.0f - 2.0f * progress);

    mCamera.setOrientation({
        mViewTransitionStart.yaw
            + (mViewTransitionTarget.yaw - mViewTransitionStart.yaw) * eased,
        mViewTransitionStart.pitch
            + (mViewTransitionTarget.pitch - mViewTransitionStart.pitch) * eased
    });

    update();

    if (progress >= 1.0f) {
        mViewTransitionActive = false;
        mViewTransitionTimer.stop();
    }
}

void PreviewViewport::cancelViewTransition() {
    mViewTransitionTimer.stop();
    mViewTransitionActive = false;
}

void PreviewViewport::leaveTemporaryAxisProjection() {
    if (!mTemporaryAxisOrthographic) {
        return;
    }

    mTemporaryAxisOrthographic = false;
    if (mCamera.projectionType() != mProjectionBeforeAxisView) {
        mCamera.setProjectionType(mProjectionBeforeAxisView);
        emit projectionChanged(
            mProjectionBeforeAxisView == ViewportCamera::ProjectionType::Orthographic
        );
        update();
    }
}

void PreviewViewport::setViewDirection(ViewportCamera::ViewDirection direction) {
    updateGizmoHover(-1);
    startViewTransition(direction);
}

void PreviewViewport::resetCamera() {
    cancelViewTransition();
    mCamera.reset();
    update();
}

void PreviewViewport::setGridVisible(bool visible) {
    if (mGridVisible == visible) {
        return;
    }

    mGridVisible = visible;
    SettingsUtil::setViewportGridVisible(visible);
    emit gridVisibleChanged(visible);
    update();
}

void PreviewViewport::onTick() {
    if (!mIsPlaying) {
        return;
    }

    const qint64 now = mElapsedTimer.nsecsElapsed();
    qint64 delta = now - mLastTickNs;
    mLastTickNs = now;

    if (delta < 0) {
        delta = 0;
    }

    constexpr qint64 cMaxDeltaNs = cFrameIntervalNs * 5;
    delta = std::min(delta, cMaxDeltaNs);

    const f32 speed = std::max(mPlaybackSpeed, 0.1f);
    const qint64 frameInterval = std::max<qint64>(
        1,
        static_cast<qint64>(static_cast<f64>(cFrameIntervalNs) / speed)
    );

    mSimulationAccumulatorNs += delta;

    s32 steps = 0;
    constexpr s32 cMaxSteps = 8;
    while (mSimulationAccumulatorNs >= frameInterval && steps < cMaxSteps) {
        simulateFrame();
        mSimulationAccumulatorNs -= frameInterval;
        ++steps;
    }

    if (steps == cMaxSteps) {
        mSimulationAccumulatorNs = 0;
    }
}

void PreviewViewport::simulateFrame() {
    if (mDocument == nullptr) {
        return;
    }

    mSimulationTimer.restart();
    mSystem->calc();
    mSimulationMs = static_cast<f32>(mSimulationTimer.nsecsElapsed()) / 1'000'000.0f;

    if (mLooping && mHasEmitters && !mSystem->isActive()) {
        if (!reloadCurrentSet()) {
            clearPreview();
            return;
        }

        mSimulationTimer.restart();
        mSystem->calc();
        mSimulationMs = static_cast<f32>(mSimulationTimer.nsecsElapsed()) / 1'000'000.0f;
    }

    emit frameChanged(mSystem->frameIndex());
    update();
}

bool PreviewViewport::reloadCurrentSet() {
    const Ptcl::EmitterSet* set = currentEmitterSet();
    if (set == nullptr) {
        mHasEmitters = false;
        return false;
    }

    mSystem->setEmitterSet(*set);
    mHasEmitters = set->emitterCount() > 0;
    mSimulationAccumulatorNs = 0;
    mLastTickNs = mElapsedTimer.nsecsElapsed();
    mSimulationMs = 0.0f;

    return true;
}

void PreviewViewport::updateStats() {
    ViewportStats stats{};
    stats.active = (mDocument != nullptr);
    stats.fps = mFps;
    stats.particles = mSystem->liveParticleCount();
    stats.particleCapacity = mSystem->particleCapacity();
    stats.simulationMs = mSimulationMs;

    emit statsChanged(stats);
}

void PreviewViewport::resetFpsWindow() {
    mLastPaintNs = 0;
    mFpsWindowNs = 0;
    mFpsFrameCount = 0;
}

const Ptcl::EmitterSet* PreviewViewport::currentEmitterSet() const {
    if (mDocument == nullptr || mSetIndex < 0) {
        return nullptr;
    }

    return mDocument->emitterSet(mSetIndex);
}

QString PreviewViewport::viewLabel() const {
    constexpr f32 cAxisEpsilon = 1.0e-3f;
    const Math::Vector3f& forward = mCamera.viewBasis().forward;

    QString view = QStringLiteral("User");
    if (forward.getX() > 1.0f - cAxisEpsilon) {
        view = QStringLiteral("Left");
    } else if (forward.getX() < -1.0f + cAxisEpsilon) {
        view = QStringLiteral("Right");
    } else if (forward.getY() < -1.0f + cAxisEpsilon) {
        view = QStringLiteral("Top");
    } else if (forward.getY() > 1.0f - cAxisEpsilon) {
        view = QStringLiteral("Bottom");
    } else if (forward.getZ() > 1.0f - cAxisEpsilon) {
        view = QStringLiteral("Front");
    } else if (forward.getZ() < -1.0f + cAxisEpsilon) {
        view = QStringLiteral("Back");
    }

    const bool orthographic =
        mCamera.projectionType() == ViewportCamera::ProjectionType::Orthographic;
    return QStringLiteral("%1 %2").arg(
        view,
        orthographic ? QStringLiteral("Orthographic") : QStringLiteral("Perspective")
    );
}

QString PreviewViewport::contextLabel() const {
    const Ptcl::EmitterSet* set = currentEmitterSet();
    if (set == nullptr) {
        return QStringLiteral("No preview");
    }

    QString setName = set->name().trimmed();
    if (setName.isEmpty()) {
        setName = QStringLiteral("Emitter Set");
    }

    QString label = QStringLiteral("(%1) %2").arg(mSetIndex).arg(setName);
    if (mSelectionType == Ptcl::Selection::Type::EmitterSet
        || mSelectionType == Ptcl::Selection::Type::None) {
        return label;
    }

    const Ptcl::Emitter* emitter = (mEmitterIndex >= 0)
        ? mDocument->emitter(mSetIndex, mEmitterIndex)
        : nullptr;
    if (emitter == nullptr) {
        return label;
    }

    QString emitterName = emitter->name().trimmed();
    if (emitterName.isEmpty()) {
        emitterName = QStringLiteral("Emitter");
    }
    label += QStringLiteral(" / Emitter %1: %2").arg(mEmitterIndex).arg(emitterName);

    switch (mSelectionType) {
    case Ptcl::Selection::Type::EmitterChild:
        label += QStringLiteral(" / ChildData");
        break;
    case Ptcl::Selection::Type::EmitterFlux:
        label += QStringLiteral(" / Fluctuation");
        break;
    case Ptcl::Selection::Type::EmitterField:
        label += QStringLiteral(" / Field");
        break;
    default:
        break;
    }

    return label;
}

void PreviewViewport::updateGizmoHover(s32 axis) {
    const s32 previousAxis = mGizmo.hoverAxis();
    mGizmo.setHoverAxis(axis);

    if (axis >= 0) {
        if (axis != previousAxis) {
            setCursor(Qt::PointingHandCursor);
            QToolTip::showText(QCursor::pos(), mGizmo.toolTip(axis), this);
            update();
        }
    } else {
        unsetCursor();
        QToolTip::hideText();
        if (previousAxis >= 0) {
            update();
        }
    }
}

void PreviewViewport::drawViewContext(QPainter& painter) {
    if (width() <= 0 || height() <= 0) {
        return;
    }

    QFont font = painter.font();
    font.setPointSizeF(10.0);
    font.setBold(false);
    font.setStyleStrategy(QFont::PreferAntialias);
    painter.setFont(font);

    const QFontMetrics metrics(font);
    constexpr int cLeft = 14;
    int baseline = 12 + metrics.ascent();

    const auto drawShadowedText = [&](const QString& text, s32 x, s32 y, const QColor& color) {
        painter.setPen(QColor(0, 0, 0, 160));
        painter.drawText(QPointF(x + 1, y + 1), text);
        painter.setPen(color);
        painter.drawText(QPointF(x, y), text);
    };

    drawShadowedText(viewLabel(), cLeft, baseline, QColor(240, 240, 240, 235));
    baseline += metrics.height() + 1;

    const QString context = contextLabel();
    if (!context.isEmpty()) {
        const s32 gizmoLeft = mGizmo.area(size()).left();
        const s32 availableWidth = std::max(0, gizmoLeft - cLeft - 12);
        const QString elidedContext = metrics.elidedText(context, Qt::ElideRight, availableWidth);
        drawShadowedText(elidedContext, cLeft, baseline, QColor(205, 205, 210, 220));
    }
}

void PreviewViewport::drawViewportOverlay(QPainter& painter) {
    if (width() <= 0 || height() <= 0) {
        return;
    }

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    drawViewContext(painter);
    mGizmo.draw(painter, mCamera, size());
    painter.restore();
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
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.beginNativePainting();

    const QSize viewportSize = size() * devicePixelRatioF();
    glViewport(0, 0, viewportSize.width(), viewportSize.height());
    glClearColor(0.094f, 0.094f, 0.388f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mGlReady) {
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

        if (mGridVisible) {
            mGrid.draw(*this, mCamera, static_cast<float>(devicePixelRatioF()));
        }

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

    painter.endNativePainting();
    drawViewportOverlay(painter);

    const qint64 now = mElapsedTimer.nsecsElapsed();
    if (mLastPaintNs > 0) {
        mFpsWindowNs += now - mLastPaintNs;
        ++mFpsFrameCount;

        if (mFpsWindowNs >= 500'000'000LL) {
            mFps = static_cast<f32>(mFpsFrameCount) / (static_cast<f32>(mFpsWindowNs) / 1'000'000'000.0f);
            mFpsWindowNs = 0;
            mFpsFrameCount = 0;
        }
    }
    mLastPaintNs = now;
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
    setFocus(Qt::MouseFocusReason);

    if (event->button() == Qt::LeftButton) {
        cancelViewTransition();

        if (mGizmo.press(event->pos(), mCamera, size())) {
            setCursor(Qt::ClosedHandCursor);
            QToolTip::hideText();
            update();
            event->accept();
            return;
        }

        if (mGizmo.hoverAxis() >= 0) {
            updateGizmoHover(-1);
        }
        mOrbitDragging = true;
        mLastMouse = event->pos();
        event->accept();
        return;
    }
    if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) {
        cancelViewTransition();

        if (mGizmo.hoverAxis() >= 0) {
            updateGizmoHover(-1);
        }
        mPanDragging = true;
        mLastMouse = event->pos();
        event->accept();
        return;
    }
    QOpenGLWidget::mousePressEvent(event);
}

void PreviewViewport::mouseMoveEvent(QMouseEvent* event) {
    if (mGizmo.pressActive()) {
        const ViewportGizmo::MoveResult result = mGizmo.move(event->pos());
        if (result.startedDragging) {
            QToolTip::hideText();
            update();
        }

        if (result.dragging) {
            leaveTemporaryAxisProjection();
            mCamera.orbit(result.delta, event->modifiers().testFlag(Qt::ShiftModifier));
            update();
        }

        event->accept();
        return;
    }
    if (mOrbitDragging) {
        const QPoint delta = event->pos() - mLastMouse;
        leaveTemporaryAxisProjection();
        mCamera.orbit(delta, event->modifiers().testFlag(Qt::ShiftModifier));
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

    updateGizmoHover(mGizmo.axisAt(event->pos(), mCamera, size()));
    QOpenGLWidget::mouseMoveEvent(event);
}

void PreviewViewport::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && mGizmo.pressActive()) {
        const ViewportGizmo::ReleaseResult result = mGizmo.release();
        if (result.handled && result.hasView) {
            setViewDirection(result.view);
            update();
            event->accept();
            return;
        }

        updateGizmoHover(mGizmo.axisAt(event->pos(), mCamera, size()));
        update();
        event->accept();
        return;
    }

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

void PreviewViewport::leaveEvent(QEvent* event) {
    updateGizmoHover(-1);
    QOpenGLWidget::leaveEvent(event);
}


// ========================================================================== //


} // namespace PtclTool
