#ifndef COLORGRADIENTEDITOR_H
#define COLORGRADIENTEDITOR_H

#include <QWidget>

#include "typedefs.h"

class ColorGradientEditor final : public QWidget {
    Q_OBJECT
public:
    enum class HandleType {
        InCompletedHandle   = 0,
        PeakHandle          = 1,
        OutStartHandle      = 2
    };

public:
    explicit ColorGradientEditor(QWidget* parent = nullptr);

    void setTimings(s32 inCompleted, s32 peak, s32 outStart);
    void setColors(const QColor& initial, const QColor& peak, const QColor& end);
    void setRepetitionCount(int repetitions);

    s32 inCompletedTiming() const;
    s32 peakTiming() const;
    s32 outStartTiming() const;

    void setInitialColor(const QColor& color);
    void setPeakColor(const QColor& color);
    void setEndColor(const QColor& color);

signals:
    void handleMoved(HandleType handle);

protected:
    void paintEvent(QPaintEvent* event) final;
    void mousePressEvent(QMouseEvent* event) final;
    void mouseMoveEvent(QMouseEvent* event) final;
    void mouseReleaseEvent(QMouseEvent* event) final;
    void leaveEvent(QEvent* event) final;

    QSize sizeHint() const final;
    QSize minimumSizeHint() const final;

private:
    void drawGradientBar(QPainter& painter, const QRectF& rect, int repetitions = 0);

    f32 calcGradientBarHeight() const;
    f32 calcHandleHeight() const;
    f32 calcHandleWidth() const;
    f32 calcTextHeight() const;

private:
    QColor mInitialColor;
    QColor mPeakColor;
    QColor mEndColor;

    f32 mInCompleted;  // Handle 0
    f32 mPeak;         // Handle 1
    f32 mOutStart;     // Handle 2

    int mRepetitions;

    int mActiveHandle = -1;

    static constexpr float sMargin = 10.0f;
};

#endif // COLORGRADIENTEDITOR_H
