#pragma once

#include "typedefs.h"

#include <QWidget>


// ========================================================================== //


struct GraphPoint {
    f32 position;   // X Axis (value from 0% - 100%)
    f32 value;      // Y Axis
    // TODO: Handle type [Locked Position, Hold Start, Hold End]
};


// ========================================================================== //


class AnimGraph : public QWidget {
    Q_OBJECT

public:
    using PointList = std::vector<GraphPoint>;

private:
    struct GraphRange {
        f32 min;
        f32 max;
        f32 tickStep;
    };

public:
    explicit AnimGraph(QWidget* parent = nullptr);

    void setControlPoints(const PointList& points);

    void setLineColor(const QColor& color);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    static void drawAxisLabels(QPainter& painter, s32 width, s32 height, s32 xDivs, const GraphRange& range);
    static void drawGrid(QPainter& painter, s32 width, s32 height, const GraphRange& range);

    static QPoint mapToScreen(const GraphPoint& point, s32 contentW, s32 contentH, const GraphRange& range);

    GraphRange computeGraphRange(s32 maxTicks) const;
    static f32 chooseTickStep(f32 range, s32 maxTicks);

    s32 hitTestPoint(const QPoint& mousePos) const;

    void moveHandle(s32 handleIndex, f32 newPos, f32 newValue);
    void enforceOrdering();

private:
    PointList mPoints;
    QColor mLineColor = Qt::white;

    s32 mSelectedIdx = -1;
    bool mIsDragging = false;

    static constexpr s32 sHandleRadius = 5;

    // Ui Style
    static constexpr QColor sColorGridBg = { 48, 48, 48 };
    static constexpr QColor sColorGridlineMajor = { 26, 26, 26 };
    static constexpr QColor sColorGridlineMinor = { 42, 42, 42 };

    static constexpr QColor sColorHandle = { 0, 0, 0 };
    static constexpr QColor sColorHandleActive = { 253, 133, 2 };
};

// ========================================================================== //

