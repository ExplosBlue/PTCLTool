#pragma once

#include "typedefs.h"

#include <QRectF>
#include <QSize>
#include <QString>
#include <QTimer>
#include <QWidget>


// ========================================================================== //


class QMouseEvent;
class QPaintEvent;


namespace PtclTool {


class PreviewViewport;


class PlaybackTimeline final : public QWidget {
    Q_OBJECT
public:
    explicit PlaybackTimeline(PreviewViewport* viewport, QWidget* parent = nullptr);

    QSize sizeHint() const final;

public slots:
    void setRange(s32 endFrame, bool infinite);
    void setCurrentFrame(s32 frame);
    void setPlaying(bool playing);

signals:
    void scrubStarted();
    void scrubFinished();
    void seekRequested(s32 frame);

protected:
    void paintEvent(QPaintEvent* event) final;

    void mousePressEvent(QMouseEvent* event) final;
    void mouseMoveEvent(QMouseEvent* event) final;
    void mouseReleaseEvent(QMouseEvent* event) final;

    void leaveEvent(QEvent* event) final;

private:
    QRectF trackRect() const;

    qreal xForFrame(s32 frame) const;
    s32 frameAtX(s32 x) const;

    void updateHover(s32 x);
    void queueSeek(s32 x);
    void flushPendingSeek();

    QString endLabel() const;
    QString formatTime(s32 frame, bool roundUp = false) const;

private:
    QTimer mSeekTimer{};

    s32 mEndFrame{0};
    s32 mCurrentFrame{0};
    s32 mPendingFrame{-1};
    s32 mLastEmittedFrame{-1};

    s32 mHoverX{-1};

    bool mPlaying{false};
    bool mDragging{false};
    bool mInfinite{false};
};


// ========================================================================== //


} // namespace PtclTool
