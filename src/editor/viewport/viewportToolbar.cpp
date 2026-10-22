#include "editor/viewport/viewportToolbar.h"

#include "editor/viewport/previewViewport.h"

#include <QAction>
#include <QComboBox>
#include <QMenu>
#include <QSignalBlocker>
#include <QSize>
#include <QStyle>
#include <QToolButton>


namespace PtclTool {


// ========================================================================== //


ViewportToolbar::ViewportToolbar(PreviewViewport* viewport, QWidget* parent) :
    QToolBar{parent},
    mViewport{viewport} {
    setMovable(false);
    setFloatable(false);
    setIconSize(QSize(18, 18));
    setToolButtonStyle(Qt::ToolButtonIconOnly);

    buildActions();
    connectSignals();
    syncFromViewport();
}

void ViewportToolbar::buildActions() {
    mPlayPauseAction = addAction(style()->standardIcon(QStyle::SP_MediaPlay), "Play");
    mPlayPauseAction->setToolTip("Play (Space)");
    connect(mPlayPauseAction, &QAction::triggered, mViewport, &PreviewViewport::togglePlayback);

    QAction* restartAction = addAction(style()->standardIcon(QStyle::SP_BrowserReload), "Restart");
    restartAction->setToolTip("Restart (Shift+Left)");
    connect(restartAction, &QAction::triggered, mViewport, &PreviewViewport::restart);

    QAction* stepAction = addAction(style()->standardIcon(QStyle::SP_MediaSeekForward), "Step");
    stepAction->setToolTip("Step one frame (Right)");
    connect(stepAction, &QAction::triggered, mViewport, &PreviewViewport::step);

    addSeparator();

    mLoopButton = new QToolButton(this);
    mLoopButton->setText("Loop");
    mLoopButton->setCheckable(true);
    mLoopButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    mLoopButton->setToolTip("Restart when the effect finishes");
    connect(mLoopButton, &QToolButton::toggled, mViewport, &PreviewViewport::setLooping);
    addWidget(mLoopButton);

    mSpeedCombo = new QComboBox(this);
    mSpeedCombo->setMinimumWidth(72);
    mSpeedCombo->addItem("0.25x", 0.25f);
    mSpeedCombo->addItem("0.5x", 0.5f);
    mSpeedCombo->addItem("1x", 1.0f);
    mSpeedCombo->addItem("2x", 2.0f);
    mSpeedCombo->addItem("4x", 4.0f);
    mSpeedCombo->setCurrentIndex(2);
    connect(mSpeedCombo, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        mViewport->setPlaybackSpeed(mSpeedCombo->itemData(index).toFloat());
    });
    addWidget(mSpeedCombo);

    addSeparator();

    auto* viewButton = new QToolButton(this);
    viewButton->setText("View");
    viewButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    viewButton->setPopupMode(QToolButton::InstantPopup);
    viewButton->setToolTip("Camera views (Numpad 1/3/7, Ctrl for opposite)");

    auto* viewMenu = new QMenu(viewButton);
    const auto addViewAction = [this, viewMenu](const QString& text, ViewportCamera::ViewDirection direction) {
        QAction* action = viewMenu->addAction(text);
        connect(action, &QAction::triggered, this, [this, direction] {
            mViewport->setViewDirection(direction);
        });
    };

    addViewAction("Front",  ViewportCamera::ViewDirection::Front);
    addViewAction("Back",   ViewportCamera::ViewDirection::Back);
    addViewAction("Left",   ViewportCamera::ViewDirection::Left);
    addViewAction("Right",  ViewportCamera::ViewDirection::Right);
    addViewAction("Top",    ViewportCamera::ViewDirection::Top);
    addViewAction("Bottom", ViewportCamera::ViewDirection::Bottom);

    viewMenu->addSeparator();

    QAction* resetViewAction = viewMenu->addAction("Reset View");
    connect(resetViewAction, &QAction::triggered, mViewport, &PreviewViewport::resetCamera);

    viewMenu->addSeparator();

    mGridAction = viewMenu->addAction("Grid (Shift+Alt+Z)");
    mGridAction->setCheckable(true);
    connect(mGridAction, &QAction::toggled, mViewport, &PreviewViewport::setGridVisible);

    viewButton->setMenu(viewMenu);
    addWidget(viewButton);

    mProjectionButton = new QToolButton(this);
    mProjectionButton->setText("Orthographic");
    mProjectionButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    mProjectionButton->setToolTip("Toggle orthographic/perspective (Numpad 5)");
    connect(mProjectionButton, &QToolButton::clicked, mViewport, &PreviewViewport::toggleProjection);
    addWidget(mProjectionButton);
}

void ViewportToolbar::connectSignals() {
    connect(mViewport, &PreviewViewport::playbackStateChanged, this, [this](bool playing) {
        mPlayPauseAction->setIcon(style()->standardIcon(
            playing ? QStyle::SP_MediaPause : QStyle::SP_MediaPlay
        ));
        mPlayPauseAction->setToolTip(playing ? "Pause (Space)" : "Play (Space)");
    });

    connect(mViewport, &PreviewViewport::projectionChanged, this, [this](bool orthographic) {
        mProjectionButton->setText(orthographic ? "Orthographic" : "Perspective");
    });

    connect(mViewport, &PreviewViewport::gridVisibleChanged, this, [this](bool visible) {
        const QSignalBlocker blocker(mGridAction);
        mGridAction->setChecked(visible);
    });

    connect(mViewport, &PreviewViewport::loopingChanged, this, [this](bool looping) {
        const QSignalBlocker blocker(mLoopButton);
        mLoopButton->setChecked(looping);
    });
}

void ViewportToolbar::syncFromViewport() {
    const bool playing = mViewport->isPlaying();
    mPlayPauseAction->setIcon(style()->standardIcon(
        playing ? QStyle::SP_MediaPause : QStyle::SP_MediaPlay
    ));
    mPlayPauseAction->setToolTip(playing ? "Pause (Space)" : "Play (Space)");

    mLoopButton->setChecked(mViewport->isLooping());
    mGridAction->setChecked(mViewport->isGridVisible());
    mProjectionButton->setText(
        mViewport->projection() == ViewportCamera::ProjectionType::Orthographic ? "Orthographic" : "Perspective"
    );

    const QSignalBlocker blocker(mSpeedCombo);
    const s32 speedIndex = mSpeedCombo->findData(mViewport->playbackSpeed());
    if (speedIndex >= 0) {
        mSpeedCombo->setCurrentIndex(speedIndex);
    }
}


// ========================================================================== //


} // namespace PtclTool
