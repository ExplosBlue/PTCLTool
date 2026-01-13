#pragma once

#include "typedefs.h"

#include <QDoubleSpinBox>
#include <QWidget>


// ========================================================================== //


struct GraphPoint {
    f32 position;   // X Axis (value from 0% - 100%)
    f32 value;      // Y Axis
    // TODO: Handle type [Locked Position, Hold Start, Hold End]
};


// ========================================================================== //


class GraphHandleEditor final : public QWidget {
    Q_OBJECT

public:
    explicit GraphHandleEditor(QWidget* parent = nullptr);

    void setValues(f32 position, f32 value);
    void setValueRange(f32 min, f32 max);

protected:
    void paintEvent(QPaintEvent* event) final;

signals:
    void valuesEdited(f32 position, f32 value);

private:
    QDoubleSpinBox* mPosition{};
    QDoubleSpinBox* mValue{};

    static constexpr s32 sArrowHeight = 8;
    static constexpr s32 sArrowWidth = 12;
};


// ========================================================================== //


class AnimGraph final : public QWidget {
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

signals:
    void pointEdited(s32 index, const GraphPoint& point);

protected:
    void paintEvent(QPaintEvent* event) final;
    void mousePressEvent(QMouseEvent* event) final;
    void mouseMoveEvent(QMouseEvent* event) final;
    void mouseReleaseEvent(QMouseEvent* event) final;

private:
    static void drawAxisLabels(QPainter& painter, s32 width, s32 height, s32 xDivs, const GraphRange& range);
    static void drawGrid(QPainter& painter, s32 width, s32 height, const GraphRange& range);

    static QPoint mapToScreen(const GraphPoint& point, s32 contentW, s32 contentH, const GraphRange& range);

    GraphRange computeGraphRange() const;
    static f32 chooseTickStep(f32 range);

    s32 hitTestPoint(const QPoint& mousePos) const;

    void moveHandle(s32 handleIndex, f32 newPos, f32 newValue);
    void enforceOrdering();

    GraphRange currentGraphRange() const;

    void showHandleEditor(s32 index);
    void hideHandleEditor();

private:
    PointList mPoints;
    QColor mLineColor = Qt::white;

    s32 mSelectedIdx = -1;
    bool mIsDragging = false;
    bool mDragOccured = false;

    QPoint mPressPos;
    bool mPendingClick = false;

    bool mHasFrozenRange = false;
    GraphRange mFrozenRange;

    GraphHandleEditor* mHandleEditor = nullptr;

    static constexpr s32 sPaddingLeft = 40;
    static constexpr s32 sPaddingRight = 16;
    static constexpr s32 sPaddingTop = 16;
    static constexpr s32 sPaddingBottom = 28;

    static constexpr s32 sHandleRadius = 5;

    // Ui Style
    static constexpr QColor sColorGridBg = { 48, 48, 48 };
    static constexpr QColor sColorGridlineMajor = { 26, 26, 26 };
    static constexpr QColor sColorGridlineMinor = { 42, 42, 42 };

    static constexpr QColor sColorHandle = { 0, 0, 0 };
    static constexpr QColor sColorHandleActive = { 253, 133, 2 };
};

// ========================================================================== //

