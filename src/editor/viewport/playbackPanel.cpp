#include "editor/viewport/playbackPanel.h"

#include "editor/viewport/playbackTimeline.h"
#include "editor/viewport/previewViewport.h"

#include <QComboBox>
#include <QFontMetrics>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QSignalBlocker>
#include <QSize>
#include <QString>
#include <QStyle>
#include <QToolButton>
#include <QVBoxLayout>

#include <algorithm>


namespace PtclTool {


// ========================================================================== //


namespace {

constexpr s32 cIconSize = 18;


QToolButton* createPlaybackButton(QWidget* parent, const QIcon& icon, const QString& toolTip) {
    auto* button = new QToolButton{parent};
    button->setAutoRaise(true);
    button->setIcon(icon);
    button->setIconSize(QSize{cIconSize, cIconSize});
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    button->setToolTip(toolTip);
    button->setFocusPolicy(Qt::NoFocus);
    return button;
}

void setLabelWidth(QLabel* label, const QString& widestText) {
    const QFontMetrics metrics(label->font());
    label->setFixedWidth(metrics.horizontalAdvance(widestText) + 8);
}

void addSeparator(QHBoxLayout* layout, QWidget* parent) {
    auto* separator = new QFrame(parent);
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setFixedHeight(16);
    separator->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(separator, 0, Qt::AlignVCenter);
}

} // namespace


// ========================================================================== //


PlaybackPanel::PlaybackPanel(PreviewViewport* viewport, QWidget* parent) :
    QWidget{parent},
    mViewport{viewport} {
    setObjectName("PlaybackPanel");
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    buildControls();
    connectSignals();
    syncFromViewport();
}

void PlaybackPanel::buildControls() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    auto* controls = new QWidget(this);
    controls->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    auto* controlsLayout = new QHBoxLayout(controls);
    controlsLayout->setContentsMargins(8, 4, 8, 8);
    controlsLayout->setSpacing(8);

    controlsLayout->addStretch(1);

    mFrameLabel = new QLabel(controls);
    mFrameLabel->setText("— / —");
    mFrameLabel->setToolTip("Current frame / total frames");
    mFrameLabel->setAlignment(Qt::AlignCenter);
    setLabelWidth(mFrameLabel, "9999 / 9999 ∞");
    controlsLayout->addWidget(mFrameLabel);
    addSeparator(controlsLayout, controls);

    mLoopButton = new QToolButton(controls);
    mLoopButton->setAutoRaise(true);
    mLoopButton->setCheckable(true);
    mLoopButton->setText("Loop");
    mLoopButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    mLoopButton->setToolTip("Restart at the end of the effect or preview range");
    mLoopButton->setFocusPolicy(Qt::NoFocus);
    connect(mLoopButton, &QToolButton::toggled, mViewport, &PreviewViewport::setLooping);
    controlsLayout->addWidget(mLoopButton);

    auto* restartButton = createPlaybackButton(
        controls,
        style()->standardIcon(QStyle::SP_BrowserReload),
        "Restart (Shift+Left)"
    );
    connect(restartButton, &QToolButton::clicked, mViewport, &PreviewViewport::restart);
    controlsLayout->addWidget(restartButton);
    addSeparator(controlsLayout, controls);

    auto* stepBackButton = createPlaybackButton(
        controls,
        style()->standardIcon(QStyle::SP_MediaSeekBackward),
        "Step back one frame (Left)"
    );
    connect(stepBackButton, &QToolButton::clicked, mViewport, &PreviewViewport::stepBackward);

    mPlayPauseButton = createPlaybackButton(
        controls,
        style()->standardIcon(QStyle::SP_MediaPlay),
        "Play (Space)"
    );
    connect(mPlayPauseButton, &QToolButton::clicked, mViewport, &PreviewViewport::togglePlayback);

    auto* stepButton = createPlaybackButton(
        controls,
        style()->standardIcon(QStyle::SP_MediaSeekForward),
        "Step one frame (Right)"
    );
    connect(stepButton, &QToolButton::clicked, mViewport, &PreviewViewport::step);

    controlsLayout->addWidget(stepBackButton);
    controlsLayout->addWidget(mPlayPauseButton);
    controlsLayout->addWidget(stepButton);
    addSeparator(controlsLayout, controls);

    mSpeedCombo = new QComboBox(controls);
    mSpeedCombo->setMinimumWidth(88);
    mSpeedCombo->setToolTip("Playback speed");
    mSpeedCombo->addItem("0.25x", 0.25f);
    mSpeedCombo->addItem("0.5x", 0.5f);
    mSpeedCombo->addItem("1x", 1.0f);
    mSpeedCombo->addItem("2x", 2.0f);
    mSpeedCombo->addItem("4x", 4.0f);
    mSpeedCombo->setCurrentIndex(2);

    connect(mSpeedCombo, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        if (index >= 0) {
            mViewport->setPlaybackSpeed(mSpeedCombo->itemData(index).toFloat());
        }
    });
    controlsLayout->addWidget(mSpeedCombo);

    controlsLayout->addStretch(1);

    mTimeline = new PlaybackTimeline(mViewport, this);
    mainLayout->addWidget(mTimeline);
    mainLayout->addWidget(controls);
}

void PlaybackPanel::connectSignals() {
    connect(mTimeline, &PlaybackTimeline::scrubStarted, mViewport, &PreviewViewport::beginScrub);
    connect(mTimeline, &PlaybackTimeline::seekRequested, mViewport, &PreviewViewport::seekToFrame);
    connect(mTimeline, &PlaybackTimeline::scrubFinished, mViewport, &PreviewViewport::endScrub);
    connect(mViewport, &PreviewViewport::playbackStateChanged, this, &PlaybackPanel::updatePlayPauseButton);

    connect(mViewport, &PreviewViewport::loopingChanged, this, [this](bool looping) {
        const QSignalBlocker blocker{mLoopButton};
        mLoopButton->setChecked(looping);
    });

    connect(mViewport, &PreviewViewport::frameChanged, this, [this](s32 frame) {
        mCurrentFrame = std::clamp(frame, 0, mTotalFrames);
        updateFrameLabel();
    });

    connect(mViewport, &PreviewViewport::previewRangeChanged, this, [this](s32 endFrame, bool infinite) {
            mTotalFrames = std::max<s32>(0, endFrame);
            mInfiniteRange = infinite;
            mCurrentFrame = std::min(mCurrentFrame, mTotalFrames);
            updateFrameLabel();
        }
    );
}

void PlaybackPanel::syncFromViewport() {
    updatePlayPauseButton();

    {
        const QSignalBlocker blocker{mLoopButton};
        mLoopButton->setChecked(mViewport->isLooping());
    }

    {
        const QSignalBlocker blocker{mSpeedCombo};
        const s32 speedIndex = mSpeedCombo->findData(mViewport->playbackSpeed());
        if (speedIndex >= 0) {
            mSpeedCombo->setCurrentIndex(speedIndex);
        }
    }
}

void PlaybackPanel::updatePlayPauseButton() {
    const bool playing = mViewport->isPlaying();
    mPlayPauseButton->setIcon(style()->standardIcon(playing ? QStyle::SP_MediaPause : QStyle::SP_MediaPlay));
    mPlayPauseButton->setToolTip(playing ? "Pause (Space)" : "Play (Space)");
}

void PlaybackPanel::updateFrameLabel() {
    if (mTotalFrames <= 0) {
        mFrameLabel->setText("— / —");
        return;
    }

    mFrameLabel->setText(QStringLiteral("%1 / %2%3")
        .arg(mCurrentFrame)
        .arg(mTotalFrames)
        .arg(mInfiniteRange ? QStringLiteral(" ∞") : QString())
    );
}

// ========================================================================== //


} // namespace PtclTool
