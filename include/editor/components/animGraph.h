#pragma once

#include "typedefs.h"

#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QWidget>


// ========================================================================== //


enum class GraphHandleType {
    Locked,
    HoldStart,
    HoldEnd
};


// ========================================================================== //


class AnimGraph final : public QWidget {
    Q_OBJECT

public:
    using HandleType = ::GraphHandleType;

    struct GraphPoint {
        f32 position;   // X Axis (value from 0% - 100%)
        f32 value;      // Y Axis
        HandleType handleType;
    };

    using PointList = std::vector<GraphPoint>;

private:
    struct GraphRange {
        f32 min;
        f32 max;
        f32 tickStep;
    };

    struct GraphRect {
        s32 x;
        s32 y;
        s32 w;
        s32 h;

        inline bool contains(const QPoint& pos) const {
            return
            pos.x() >= x && pos.x() < x + w &&
            pos.y() >= y && pos.y() < y + h;
        }
    };

    struct DrawContext {
        GraphRange range{};
        GraphRect contentRect{};

        QColor gridBg{};
        QColor gridMajor{};
        QColor gridMinor{};

        QColor handleNormal{};
        QColor handleSelected{};
        QColor handleFill{};

        QColor axisColor{};

        QFont titleFont{};
        QFontMetrics titleMetrics;
    };

public:
    explicit AnimGraph(QWidget* parent = nullptr);

    void setControlPoints(const PointList& points);
    void zoomToFit();
    QPointF handleWidgetPos(s32 handleIndex) const;
    QPointF handleVisualPos(s32 handleIndex) const;
    void setLineColor(const QColor& color);
    const PointList& getPoints() const;
    void setValueRange(f32 min, f32 max);
    void setValueBounds(f32 min, f32 max);
    void setTickStepSize(f32 stepSize);
    void setVerticalAxisLabel(const QString& label);
    void setFixedLeftPadding(s32 pad);
    void setPosLabel(const QString& text);
    void setValLabel(const QString& text);

signals:
    void pointEdited(s32 index, const AnimGraph::GraphPoint& point);

protected:
    void paintEvent(QPaintEvent* event) final;
    void mousePressEvent(QMouseEvent* event) final;
    void mouseMoveEvent(QMouseEvent* event) final;
    void mouseReleaseEvent(QMouseEvent* event) final;
    void wheelEvent(QWheelEvent* event) final;

private:
    static void drawAxisLabels(QPainter& painter, const DrawContext& ctx);
    static void drawGrid(QPainter& painter, const DrawContext& ctx);
    void drawHandle(QPainter& painter, const QPoint& pos, HandleType type, const QColor& color, const QColor& fillColor) const;

    static QPoint mapToScreen(const GraphPoint& point, s32 contentW, s32 contentH, const GraphRange& range);

    GraphRange computeGraphRange() const;
    GraphRange currentGraphRange() const;

    f32 chooseTickStep(f32 range) const;

    QList<s32> hitTestAll(const QPoint& mousePos) const;
    QRect getHitRect(const QPoint& center, HandleType type) const;

    void moveHandle(s32 handleIndex, f32 newPos, f32 newValue);
    void enforceOrdering(s32 draggedIndex);
    void ensureHandleVisible(s32 handleIndex);
    void updateCursor(const QPoint& pos);

    void updateControls();
    void zoom(f32 factor, f32 focusValue);
    void snapZoom();
    void clampZoom();

    GraphRect computeGraphRect() const;

    DrawContext buildDrawContext();

    QVector<QPoint> buildPolyline(const DrawContext& ctx);
    void drawGraphLine(QPainter& painter, const DrawContext& ctx, const QVector<QPoint>& poly);
    void drawHandles(QPainter& painter, const DrawContext& ctx, const QVector<QPoint>& poly);
    void drawEdgeIndicators(QPainter& painter, const DrawContext& ctx, const QVector<QPoint>& poly);
    void drawAxisTitles(QPainter& painter, const DrawContext& ctx);


private:
    struct EdgeIndicator {
        QPoint pos;
        s32 handleIndex;
    };

    PointList mPoints{};
    QLabel* mPosLabel = nullptr;
    QLabel* mValLabel = nullptr;
    QDoubleSpinBox* mPositionSpin = nullptr;
    QDoubleSpinBox* mValueSpin = nullptr;
    QWidget* mControlPanel = nullptr;
    QPushButton* mZoomInBtn = nullptr;
    QPushButton* mZoomOutBtn = nullptr;
    QPushButton* mZoomFitBtn = nullptr;
    s32 mSelectedIdx = -1;
    QString mVerticalAxisLabel;
    s32 mPaddingLeft = 40;
    s32 mPaddingBottom = 28;
    s32 mFixedLeftPad = 0;

    bool mIsDragging = false;
    bool mIsPanning = false;
    bool mDragOccured = false;
    bool mPendingClick = false;
    bool mHasFrozenRange = false;
    bool mHasCustomRange = false;
    mutable bool mHasZoom = false;

    f32 mTickStepSize = 1.0f;

    // Font metric cache (rebuilt when widget font changes)
    QFont mCachedWidgetFont{};
    s32 mCachedMaxYTickW = 0;

    QPoint mPressPos{};
    GraphRange mPanAnchorRange{};
    GraphRange mFrozenRange{};
    GraphRange mCustomRange{};
    mutable f32 mZoomMin = 0.0f;
    mutable f32 mZoomMax = 1.0f;

    bool mHasValueBounds = false;
    f32 mValueBoundsMin = 0.0f;
    f32 mValueBoundsMax = 1.0f;

    QColor mLineColor = Qt::white;
    QVector<EdgeIndicator> mEdgeIndicators{};

    // Ui Style
    static constexpr s32 sPaddingLeft = 40;
    static constexpr s32 sPaddingRight = 16;
    static constexpr s32 sPaddingTop = 16;
    static constexpr s32 sPaddingBottom = 28;

    static constexpr s32 sHandleRadius = 6;
    static constexpr s32 sHandleHitRadius = 12;
};

// ========================================================================== //

