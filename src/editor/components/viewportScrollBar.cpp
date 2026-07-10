#include "editor/components/viewportScrollBar.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QStyleOption>


namespace PtclEditor {


ViewportScrollBar::ViewportScrollBar(QWidget* parent) :
    QWidget{parent} {

    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(14);
}

void ViewportScrollBar::setRange(s32 contentWidth, s32 viewportWidth, s32 scrollOffset) {
    mContentWidth = contentWidth;
    mViewportWidth = viewportWidth;
    mScrollOffset = scrollOffset;
    update();
}

ViewportScrollBar::HandleGeometry ViewportScrollBar::geometry() const {
    const s32 width = handleWidth();
    const s32 left = handleLeft();
    const s32 grip = 10;

    const QRect rect(left, 0, width, height());

    return {
        rect,
        QRect(left, 0, grip, height()),
        QRect(left + width - grip, 0, grip, height()),
    };
}

s32 ViewportScrollBar::handleWidth() const {
    if (mContentWidth <= 0 || mViewportWidth <= 0) {
        return 0;
    }
    const f32 fraction = static_cast<f32>(mViewportWidth) / static_cast<f32>(mContentWidth);
    return std::clamp(static_cast<s32>(fraction * static_cast<f32>(mViewportWidth)), sMinHandle, mViewportWidth);
}

s32 ViewportScrollBar::handleLeft() const {
    const s32 width = handleWidth();
    const s32 range = mViewportWidth - width;

    if (range <= 0 || mContentWidth <= mViewportWidth) {
        return 0;
    }

    const f32 fraction = static_cast<f32>(mScrollOffset) / static_cast<f32>(mContentWidth - mViewportWidth);
    return static_cast<s32>(fraction * static_cast<f32>(range));
}

void ViewportScrollBar::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    const auto pal = palette();

    painter.fillRect(rect(), pal.color(QPalette::Window));
    painter.setPen(pal.color(QPalette::Mid));
    painter.drawLine(0, 0, width(), 0);

    const auto geom = geometry();

    if (!geom.rect.isValid()) {
        return;
    }

    const bool isDragging = mGrab != Grab::None;

    QColor accent = pal.color(QPalette::Highlight);
    if (isDragging) {
        accent = pal.color(QPalette::HighlightedText);
        accent = accent.darker(130);
    } else if (mHover == Hover::Handle || mHover == Hover::LeftGrip || mHover == Hover::RightGrip) {
        accent = accent.lighter(130);
    }

    painter.fillRect(geom.rect.adjusted(0, 1, 0, -1), accent);

    QColor gripColor = accent.lighter(130);
    if (isDragging && (mGrab == Grab::LeftGrip || mGrab == Grab::RightGrip)) {
        gripColor = accent.lighter(160);
    } else if (mHover == Hover::LeftGrip || mHover == Hover::RightGrip) {
        gripColor = accent.lighter(150);
    }

    painter.fillRect(geom.leftGrip, gripColor);
    painter.fillRect(geom.rightGrip, gripColor);
}

void ViewportScrollBar::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {
        return;
    }

    const auto geom = geometry();
    const s32 mouseX = event->pos().x();

    if (geom.leftGrip.contains(event->pos())) {
        mGrab = Grab::LeftGrip;
        mGrabFixedEdge = geom.rect.right();
    } else if (geom.rightGrip.contains(event->pos())) {
        mGrab = Grab::RightGrip;
        mGrabFixedEdge = geom.rect.left();
    } else if (geom.rect.contains(event->pos())) {
        mGrab = Grab::Handle;
    } else {
        if (mContentWidth <= mViewportWidth) {
            return;
        }
        const f32 fraction = static_cast<f32>(mouseX) / static_cast<f32>(mViewportWidth);
        const s32 offset = static_cast<s32>(fraction * static_cast<f32>(mContentWidth) - static_cast<f32>(mViewportWidth) / 2.0f);
        emit scrollChanged(std::clamp(offset, 0, mContentWidth - mViewportWidth));
        return;
    }

    mGrabStartX = mouseX;
    mGrabStartHandleLeft = geom.rect.left();
    updateCursor();
    update();
}

void ViewportScrollBar::moveHandle(s32 mouseX) {
    const s32 width = handleWidth();
    const s32 range = mViewportWidth - width;

    if (range <= 0) {
        return;
    }

    const s32 left = std::clamp(mGrabStartHandleLeft + mouseX - mGrabStartX, 0, range);
    const f32 fraction = static_cast<f32>(left) / static_cast<f32>(range);
    const s32 newOffset = static_cast<s32>(fraction * static_cast<f32>(mContentWidth - mViewportWidth));

    emit scrollChanged(newOffset);
}

void ViewportScrollBar::resizeHandle(s32 mouseX, bool isLeft) {
    const s32 newWidth = isLeft
        ? mGrabFixedEdge - std::clamp(mouseX, 0, mGrabFixedEdge - sMinHandle)
        : std::clamp(mouseX, mGrabFixedEdge + sMinHandle, mViewportWidth) - mGrabFixedEdge;

    const s32 newHandleWidth = std::clamp(newWidth, sMinHandle, mViewportWidth);

    if (newHandleWidth <= 0) {
        return;
    }

    const s32 newContentWidth = static_cast<s32>(
        (static_cast<f32>(mViewportWidth) * static_cast<f32>(mViewportWidth))
        / static_cast<f32>(newHandleWidth));

    const s32 edge = isLeft ? mGrabFixedEdge - newHandleWidth : mGrabFixedEdge;

    const s32 range = mViewportWidth - newHandleWidth;

    const f32 fraction = (range > 0)
        ? static_cast<f32>(edge) / static_cast<f32>(range)
        : 0;

    const s32 maxOffset = std::max(0, newContentWidth - mViewportWidth);
    const s32 offset = static_cast<s32>(fraction * static_cast<f32>(maxOffset));

    emit viewportChanged(newContentWidth, std::clamp(offset, 0, maxOffset));
}

ViewportScrollBar::Hover ViewportScrollBar::hoverAt(const QPoint& pos) const {
    const auto geom = geometry();
    if (!geom.rect.isValid()) {
        return Hover::None;
    }
    if (geom.leftGrip.contains(pos)) {
        return Hover::LeftGrip;
    }
    if (geom.rightGrip.contains(pos)) {
        return Hover::RightGrip;
    }
    if (geom.rect.contains(pos)) {
        return Hover::Handle;
    }
    return Hover::None;
}

void ViewportScrollBar::updateCursor() {
    if (mGrab != Grab::None) {
        setCursor(Qt::ClosedHandCursor);
        return;
    }
    switch (mHover) {
        case Hover::LeftGrip:
        case Hover::RightGrip:
            setCursor(Qt::SizeHorCursor);
            break;
        case Hover::Handle:
            setCursor(Qt::OpenHandCursor);
            break;
        case Hover::None:
            unsetCursor();
            break;
    }
}

void ViewportScrollBar::mouseMoveEvent(QMouseEvent* event) {
    if (mGrab != Grab::None) {
        switch (mGrab) {
            case Grab::Handle:    moveHandle(event->pos().x());          break;
            case Grab::LeftGrip:  resizeHandle(event->pos().x(), true);  break;
            case Grab::RightGrip: resizeHandle(event->pos().x(), false); break;
            case Grab::None: break;
        }
        return;
    }

    const Hover prevHover = mHover;
    mHover = hoverAt(event->pos());
    updateCursor();

    if (mHover != prevHover) {
        update();
    }
}

void ViewportScrollBar::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        mGrab = Grab::None;
        mHover = hoverAt(event->pos());
        updateCursor();
        update();
    }
}

void ViewportScrollBar::leaveEvent(QEvent*) {
    if (mGrab == Grab::None) {
        mHover = Hover::None;
        updateCursor();
        update();
    }
}


} // namespace PtclEditor
