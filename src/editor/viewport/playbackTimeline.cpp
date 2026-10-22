#include "editor/viewport/playbackTimeline.h"

#include "editor/viewport/previewViewport.h"

#include <QColor>
#include <QFont>
#include <QFontMetricsF>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>

#include <algorithm>
#include <array>
#include <cmath>


namespace PtclTool {


// ========================================================================== //


namespace {

constexpr s32 cFramesPerSecond = PreviewViewport::cFramesPerSecond;
constexpr s32 cWidgetHeight = 20;
constexpr s32 cTrackMarginX = 12;
constexpr qreal cTrackHeight = 5.0;
constexpr qreal cTimeGap = 6.0;
constexpr qreal cTimeLabelPointSize = 8.0;
constexpr qreal cMinimumTickSpacing = 44.0;

constexpr std::array<s32, 14> cTickStepsSeconds{
    1, 2, 5, 10, 15, 30, 60, 120, 300, 600, 900, 1800, 3600, 7200
};

QFont timeFont(QFont font) {
    font.setPointSizeF(cTimeLabelPointSize);
    return font;
}

} // namespace


// ========================================================================== //


PlaybackTimeline::PlaybackTimeline(PreviewViewport* viewport, QWidget* parent) :
    QWidget{parent} {
    setObjectName("PlaybackTimeline");
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(cWidgetHeight);
    setMouseTracking(true);

    mSeekTimer.setSingleShot(true);
    mSeekTimer.setInterval(16);
    connect(&mSeekTimer, &QTimer::timeout, this, &PlaybackTimeline::flushPendingSeek);

    connect(viewport, &PreviewViewport::previewRangeChanged, this, &PlaybackTimeline::setRange);
    connect(viewport, &PreviewViewport::frameChanged, this, &PlaybackTimeline::setCurrentFrame);
    connect(viewport, &PreviewViewport::playbackStateChanged, this, &PlaybackTimeline::setPlaying);
}

QSize PlaybackTimeline::sizeHint() const {
    return { 320, cWidgetHeight };
}

void PlaybackTimeline::setRange(s32 endFrame, bool infinite) {
    endFrame = std::max<s32>(0, endFrame);
    if (mEndFrame == endFrame && mInfinite == infinite) {
        return;
    }

    mEndFrame = endFrame;
    mInfinite = infinite;
    mCurrentFrame = std::clamp(mCurrentFrame, 0, mEndFrame);
    mHoverX = -1;
    mPendingFrame = -1;
    mLastEmittedFrame = -1;
    mSeekTimer.stop();

    if (mEndFrame > 0) {
        if (mInfinite) {
            const s32 previewSeconds = std::max<s32>(1, (mEndFrame + cFramesPerSecond - 1) / cFramesPerSecond);
            setToolTip(QStringLiteral(
                "Infinite or very long effect — showing a %1 second preview "
                "window. Click or drag to scrub."
            ).arg(previewSeconds));
        } else {
            setToolTip("Click or drag to scrub through the effect.");
        }
    } else {
        setToolTip(QString());
    }

    update();
}

void PlaybackTimeline::setCurrentFrame(s32 frame) {
    frame = std::clamp(frame, 0, mEndFrame);
    if (mCurrentFrame == frame) {
        return;
    }

    mCurrentFrame = frame;
    update();
}

void PlaybackTimeline::setPlaying(bool playing) {
    if (mPlaying == playing) {
        return;
    }

    mPlaying = playing;
    update();
}

QRectF PlaybackTimeline::trackRect() const {
    const QFontMetricsF metrics(timeFont(font()));

    const QString leftLabel = formatTime(0);
    const QString rightLabel = endLabel();

    const qreal left = cTrackMarginX + metrics.horizontalAdvance(leftLabel) + cTimeGap;
    const qreal right = static_cast<qreal>(width()) - cTrackMarginX - metrics.horizontalAdvance(rightLabel) - cTimeGap;

    return {
        left,
        (height() - cTrackHeight) * 0.5,
        std::max<qreal>(1.0, right - left),
        cTrackHeight
    };
}

qreal PlaybackTimeline::xForFrame(s32 frame) const {
    const QRectF track = trackRect();
    if (mEndFrame <= 0) {
        return track.left();
    }

    const f64 t = static_cast<f64>(std::clamp(frame, 0, mEndFrame)) / static_cast<f64>(mEndFrame);
    return track.left() + track.width() * t;
}

s32 PlaybackTimeline::frameAtX(s32 x) const {
    const QRectF track = trackRect();
    if (mEndFrame <= 0 || track.width() <= 0.0) {
        return 0;
    }

    const f64 t = std::clamp((static_cast<f64>(x) - track.left()) / track.width(), 0.0, 1.0);
    return static_cast<s32>(std::lround(t * static_cast<f64>(mEndFrame)));
}

void PlaybackTimeline::updateHover(s32 x) {
    if (mHoverX == x) {
        return;
    }

    mHoverX = x;
    update();
}

void PlaybackTimeline::queueSeek(s32 x) {
    mPendingFrame = frameAtX(x);
    if (mPendingFrame == mLastEmittedFrame) {
        return;
    }

    if (!mSeekTimer.isActive()) {
        mSeekTimer.start();
    }
}

void PlaybackTimeline::flushPendingSeek() {
    if (mPendingFrame < 0 || mPendingFrame == mLastEmittedFrame) {
        return;
    }

    const s32 frame = mPendingFrame;
    mLastEmittedFrame = frame;
    emit seekRequested(frame);
}

QString PlaybackTimeline::endLabel() const {
    const QString time = formatTime(mEndFrame, true);
    return mInfinite ? QStringLiteral("∞ %1").arg(time) : time;
}

QString PlaybackTimeline::formatTime(s32 frame, bool roundUp) const {
    const s32 totalSeconds = (std::max<s32>(0, frame) + (roundUp ? cFramesPerSecond - 1 : 0)) / cFramesPerSecond;
    const s32 minutes = totalSeconds / 60;
    const s32 seconds = totalSeconds % 60;

    return QStringLiteral("%1:%2")
        .arg(minutes)
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

void PlaybackTimeline::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    painter.setPen(palette().color(QPalette::Mid));
    painter.drawLine(0, 0, width(), 0);

    if (mEndFrame <= 0) {
        painter.setPen(palette().color(QPalette::Disabled, QPalette::WindowText));
        painter.setFont(timeFont(painter.font()));
        painter.drawText(
            QRectF(cTrackMarginX, 0, width() - cTrackMarginX * 2, height()),
            Qt::AlignVCenter | Qt::AlignLeft,
            QStringLiteral("No preview")
        );
        return;
    }

    const QRectF track = trackRect();
    const QColor trackColor = palette().color(QPalette::Dark);
    const QColor fillColor = palette().color(QPalette::Highlight);
    const QColor dimTextColor = palette().color(QPalette::Disabled, QPalette::WindowText);
    const qreal playheadX = xForFrame(mCurrentFrame);

    QPainterPath trackPath;
    trackPath.addRoundedRect(track, cTrackHeight * 0.5, cTrackHeight * 0.5);
    painter.fillPath(trackPath, trackColor);

    if (playheadX > track.left()) {
        painter.save();
        painter.setClipPath(trackPath);

        painter.fillRect(
            QRectF(
                track.left(),
                track.top(),
                playheadX - track.left(),
                track.height()
            ),
            fillColor
        );

        painter.restore();
    }

    painter.setFont(timeFont(painter.font()));
    const QFontMetricsF metrics(painter.font());

    const QString leftLabel = formatTime(0);
    const QString rightLabel = endLabel();
    const qreal leftLabelWidth = metrics.horizontalAdvance(leftLabel);
    const qreal rightLabelWidth = metrics.horizontalAdvance(rightLabel);

    s32 tickStepFrames = cTickStepsSeconds.back() * cFramesPerSecond;
    for (const s32 stepSeconds : cTickStepsSeconds) {
        const s32 stepFrames = stepSeconds * cFramesPerSecond;
        if (stepFrames >= mEndFrame) {
            tickStepFrames = stepFrames;
            break;
        }

        const f64 spacing = track.width() * static_cast<f64>(stepFrames) / static_cast<f64>(mEndFrame);

        tickStepFrames = stepFrames;
        if (spacing >= cMinimumTickSpacing) {
            break;
        }
    }

    painter.setPen(palette().color(QPalette::Mid));
    for (s32 tick = tickStepFrames; tick < mEndFrame; tick += tickStepFrames) {
        const qreal x = xForFrame(tick);
        painter.drawLine(QPointF(x, track.bottom() + 1.0), QPointF(x, track.bottom() + 4.0));
    }

    painter.setPen(dimTextColor);
    painter.drawText(
        QRectF(cTrackMarginX, 0, leftLabelWidth, height()),
        Qt::AlignVCenter | Qt::AlignLeft,
        leftLabel
    );

    painter.setPen(mInfinite ? fillColor : dimTextColor);
    painter.drawText(
        QRectF(width() - cTrackMarginX - rightLabelWidth, 0, rightLabelWidth, height()),
        Qt::AlignVCenter | Qt::AlignRight,
        rightLabel
    );

    if (mHoverX >= 0 && !mDragging) {
        const qreal hoverX = std::clamp(static_cast<qreal>(mHoverX), track.left(), track.right());

        QColor hoverColor = fillColor;
        hoverColor.setAlpha(100);

        painter.setPen(QPen(hoverColor, 1.0));
        painter.drawLine(
            QPointF(hoverX, track.top() - 2.0),
            QPointF(hoverX, track.bottom() + 3.0)
        );
    }

    const qreal knobRadius = mDragging ? 5.0 : 4.0;
    painter.setPen(QPen(fillColor, 1.5));
    painter.setBrush(mPlaying ? fillColor : palette().color(QPalette::Window));
    painter.drawEllipse(
        QPointF(playheadX, track.center().y()),
        knobRadius,
        knobRadius
    );

    painter.setPen(Qt::NoPen);
    painter.setBrush(fillColor);
    painter.drawEllipse(
        QPointF(playheadX, track.center().y()),
        mPlaying || mDragging ? 1.75 : 1.25,
        mPlaying || mDragging ? 1.75 : 1.25
    );
}

void PlaybackTimeline::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton || mEndFrame <= 0) {
        QWidget::mousePressEvent(event);
        return;
    }

    const s32 x = static_cast<s32>(std::lround(event->position().x()));
    mDragging = true;
    mLastEmittedFrame = -1;
    setCursor(Qt::SizeHorCursor);
    updateHover(x);

    emit scrubStarted();
    mPendingFrame = frameAtX(x);
    flushPendingSeek();
    event->accept();
}

void PlaybackTimeline::mouseMoveEvent(QMouseEvent* event) {
    if (mEndFrame <= 0) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    const s32 x = static_cast<s32>(std::lround(event->position().x()));
    updateHover(x);

    if (mDragging) {
        queueSeek(x);
        event->accept();
        return;
    }

    QWidget::mouseMoveEvent(event);
}

void PlaybackTimeline::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton || !mDragging) {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    const s32 x = static_cast<s32>(std::lround(event->position().x()));
    mSeekTimer.stop();
    mPendingFrame = frameAtX(x);
    flushPendingSeek();
    mDragging = false;
    unsetCursor();
    updateHover(x);

    emit scrubFinished();
    event->accept();
}

void PlaybackTimeline::leaveEvent(QEvent* event) {
    if (!mDragging) {
        mHoverX = -1;
        update();
    }
    QWidget::leaveEvent(event);
}


// ========================================================================== //


} // namespace PtclTool
