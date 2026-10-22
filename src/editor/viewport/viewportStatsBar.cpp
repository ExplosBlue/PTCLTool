#include "editor/viewport/viewportStatsBar.h"

#include "editor/viewport/previewViewport.h"

#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>


namespace PtclTool {


// ========================================================================== //


namespace /* Anonymous */ {

QLabel* createStatLabel(QWidget* parent, const QString& text, const QString& toolTip) {
    auto* label = new QLabel(text, parent);
    label->setToolTip(toolTip);
    return label;
}

void setStatWidth(QLabel* label, const QString& widestText) {
    const QFontMetrics metrics(label->font());
    label->setFixedWidth(metrics.horizontalAdvance(widestText) + 8);
}

void addSeparator(QHBoxLayout* layout, QWidget* parent) {
    auto* separator = new QFrame(parent);
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    layout->addWidget(separator);
}

} // namespace Anonymous


// ========================================================================== //


ViewportStatsBar::ViewportStatsBar(PreviewViewport* viewport, QWidget* parent) :
    QFrame{parent},
    mViewport{viewport} {
    setObjectName("ViewportStatsBar");
    setFrameShape(QFrame::NoFrame);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 3, 8, 3);
    layout->setSpacing(6);

    mPlaybackLabel = createStatLabel(this, "No preview", "Playback state");
    mPlaybackLabel->setAlignment(Qt::AlignCenter);
    mFrameLabel = createStatLabel(this, "Frame —", "Current simulation frame");
    mFpsLabel = createStatLabel(this, "— fps", "Rendered frames per second");
    mParticlesLabel = createStatLabel(this, "— particles", "Live particles / total pool capacity (regular + compact)");
    mSimulationLabel = createStatLabel(this, "— ms", "Simulation time per frame");

    setStatWidth(mPlaybackLabel, "No preview");
    setStatWidth(mFrameLabel, "Frame 9999999");
    setStatWidth(mFpsLabel, "9999 fps");
    setStatWidth(mParticlesLabel, "9999/9999 particles");
    setStatWidth(mSimulationLabel, "9999.9 ms");

    layout->addWidget(mPlaybackLabel);
    addSeparator(layout, this);
    layout->addWidget(mFrameLabel);

    layout->addStretch(1);

    layout->addWidget(mParticlesLabel);
    addSeparator(layout, this);
    layout->addWidget(mFpsLabel);
    addSeparator(layout, this);
    layout->addWidget(mSimulationLabel);

    connect(mViewport, &PreviewViewport::statsChanged, this, &ViewportStatsBar::updateStats);
    connect(mViewport, &PreviewViewport::frameChanged, this, [this](s32 frame) {
        mFrameLabel->setText(QStringLiteral("Frame %1").arg(frame));
    });
    connect(mViewport, &PreviewViewport::playbackStateChanged, this, &ViewportStatsBar::updatePlaybackState);
}

void ViewportStatsBar::updateStats(const ViewportStats& stats) {
    mHasPreview = stats.active;

    if (!stats.active) {
        mPlaybackLabel->setText("No preview");
        mFrameLabel->setText("Frame —");
        mFpsLabel->setText("— fps");
        mParticlesLabel->setText("— particles");
        mSimulationLabel->setText("— ms");
        return;
    }

    updatePlaybackState(mViewport->isPlaying());

    mFpsLabel->setText(QStringLiteral("%1 fps").arg(stats.fps, 0, 'f', 0));
    mParticlesLabel->setText(QStringLiteral("%1/%2 particles")
        .arg(stats.particles)
        .arg(stats.particleCapacity));
    mSimulationLabel->setText(QStringLiteral("%1 ms").arg(stats.simulationMs, 0, 'f', 1));
}

void ViewportStatsBar::updatePlaybackState(bool playing) {
    if (!mHasPreview) {
        return;
    }

    mPlaybackLabel->setText(playing ? "Playing" : "Paused");
}

void ViewportStatsBar::paintEvent(QPaintEvent* event) {
    QFrame::paintEvent(event);

    QPainter painter(this);
    painter.setPen(palette().color(QPalette::Mid));
    painter.drawLine(0, 0, width(), 0);
}


// ========================================================================== //


} // namespace PtclTool
