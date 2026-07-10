#pragma once

#include "typedefs.h"

#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ViewportScrollBar final : public QWidget {
    Q_OBJECT
public:
    explicit ViewportScrollBar(QWidget* parent = nullptr);

    void setRange(s32 contentWidth, s32 viewportWidth, s32 scrollOffset);

signals:
    void scrollChanged(s32 offset);
    void viewportChanged(s32 viewportWidth, s32 offset);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    enum class Grab {
        None,
        Handle,
        LeftGrip,
        RightGrip
    };

    enum class Hover {
        None,
        Handle,
        LeftGrip,
        RightGrip
    };

    struct HandleGeometry {
        QRect rect;
        QRect leftGrip;
        QRect rightGrip;
    };

private:
    s32 handleLeft() const;
    s32 handleWidth() const;

    void moveHandle(s32 mouseX);
    void resizeHandle(s32 mouseX, bool isLeft);

    HandleGeometry geometry() const;
    Hover hoverAt(const QPoint& pos) const;
    void updateCursor();

private:
    s32 mContentWidth{0};
    s32 mViewportWidth{0};
    s32 mScrollOffset{0};

    Grab mGrab{Grab::None};
    Hover mHover{Hover::None};
    s32 mGrabStartX{0};
    s32 mGrabFixedEdge{0};
    s32 mGrabStartHandleLeft{0};

    static constexpr s32 sMinHandle = 40;
};


// ========================================================================== //


} // namespace PtclEditor
