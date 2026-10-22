#pragma once

#include "editor/viewport/viewportCamera.h"

#include <QPoint>
#include <QPointF>
#include <QRect>
#include <QSize>
#include <QString>


class QPainter;

namespace PtclTool {


// ========================================================================== //


class ViewportGizmo final {
public:
    struct MoveResult {
        bool dragging{false};
        bool startedDragging{false};
        QPoint delta{};
    };

    struct ReleaseResult {
        bool handled{false};
        bool hasView{false};
        ViewportCamera::ViewDirection view{ViewportCamera::ViewDirection::Front};
    };

    QRect area(const QSize& widgetSize) const;
    s32 hoverAxis() const { return mHoverAxis; }
    void setHoverAxis(s32 axis) { mHoverAxis = axis; }
    bool pressActive() const { return mPressActive; }

    bool press(const QPoint& pos, const ViewportCamera& camera, const QSize& widgetSize);
    MoveResult move(const QPoint& pos);
    ReleaseResult release();

    s32 axisAt(const QPoint& pos, const ViewportCamera& camera, const QSize& widgetSize) const;
    QString toolTip(s32 axis) const;
    void draw(QPainter& painter, const ViewportCamera& camera, const QSize& widgetSize) const;

private:
    QPointF axisEndpoint(s32 axis, const ViewportCamera& camera, const QRect& area) const;

    s32 mHoverAxis{-1};
    s32 mPressAxis{-1};
    QPoint mPressPos{};
    QPoint mLastMouse{};
    bool mPressActive{false};
    bool mDragging{false};
};


// ========================================================================== //


} // namespace PtclTool
