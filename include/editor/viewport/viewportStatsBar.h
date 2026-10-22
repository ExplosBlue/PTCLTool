#pragma once

#include <QFrame>


// ========================================================================== //


class QLabel;


namespace PtclTool {


// ========================================================================== //


class PreviewViewport;
struct ViewportStats;


class ViewportStatsBar final : public QFrame {
    Q_OBJECT
public:
    explicit ViewportStatsBar(PreviewViewport* viewport, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) final;

private:
    void updateStats(const ViewportStats& stats);
    void updatePlaybackState(bool playing);

    PreviewViewport* mViewport{nullptr};
    bool mHasPreview{false};

    QLabel* mPlaybackLabel{nullptr};
    QLabel* mFrameLabel{nullptr};
    QLabel* mFpsLabel{nullptr};
    QLabel* mParticlesLabel{nullptr};
    QLabel* mSimulationLabel{nullptr};
};


// ========================================================================== //


} // namespace PtclTool
