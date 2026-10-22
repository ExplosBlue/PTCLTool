#pragma once

#include "typedefs.h"

#include <QWidget>


// ========================================================================== //


class QComboBox;
class QLabel;
class QToolButton;


namespace PtclTool {


// ========================================================================== //


class PlaybackTimeline;
class PreviewViewport;


class PlaybackPanel final : public QWidget {
    Q_OBJECT
public:
    explicit PlaybackPanel(PreviewViewport* viewport, QWidget* parent = nullptr);

private:
    void buildControls();
    void connectSignals();

    void syncFromViewport();
    void updatePlayPauseButton();
    void updateFrameLabel();

private:
    PreviewViewport* mViewport{nullptr};

    PlaybackTimeline* mTimeline{nullptr};
    QToolButton* mPlayPauseButton{nullptr};
    QToolButton* mLoopButton{nullptr};
    QComboBox* mSpeedCombo{nullptr};
    QLabel* mFrameLabel{nullptr};

    s32 mCurrentFrame{0};
    s32 mTotalFrames{0};
    bool mInfiniteRange{false};
};


// ========================================================================== //


} // namespace PtclTool
