#include "editor/components/cropWidget.h"
#include "util/imageUtil.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include <algorithm>


// ============================================================================= //


void clampResizeRect(QRect& rect, QSize sourceSize) {
    s32 width = sourceSize.width();
    s32 height = sourceSize.height();
    rect.setLeft(std::clamp(rect.left(), 0, width - 1));
    rect.setTop(std::clamp(rect.top(), 0, height - 1));
    rect.setRight(std::clamp(rect.right(), rect.left(), width - 1));
    rect.setBottom(std::clamp(rect.bottom(), rect.top(), height - 1));
}

void snapResizeRect(QRect& rect, ResizeEdge edge) {
    bool moveLeft   = hasFlag(edge, ResizeEdge::Left);
    bool moveRight  = hasFlag(edge, ResizeEdge::Right);
    bool moveTop    = hasFlag(edge, ResizeEdge::Top);
    bool moveBottom = hasFlag(edge, ResizeEdge::Bottom);

    s32 snappedW = ImageUtil::nearestValidTextureSize(rect.width());
    if (moveLeft)  { rect.setLeft(rect.right() + 1 - snappedW); rect.setWidth(snappedW); }
    if (moveRight) { rect.setWidth(snappedW); }

    s32 snappedH = ImageUtil::nearestValidTextureSize(rect.height());
    if (moveTop)    { rect.setTop(rect.bottom() + 1 - snappedH); rect.setHeight(snappedH); }
    if (moveBottom) { rect.setHeight(snappedH); }
}

bool nearPoint(QPoint pos, QPoint target, s32 radius) {
    return std::abs(pos.x() - target.x()) <= radius &&
           std::abs(pos.y() - target.y()) <= radius;
}

bool nearHorizontalEdge(QPoint pos, s32 y, s32 left, s32 right, s32 radius) {
    return std::abs(pos.y() - y) <= radius &&
           pos.x() >= left + radius &&
           pos.x() <= right - radius;
}

bool nearVerticalEdge(QPoint pos, s32 x, s32 top, s32 bottom, s32 radius) {
    return std::abs(pos.x() - x) <= radius &&
           pos.y() >= top + radius &&
           pos.y() <= bottom - radius;
}

static Qt::CursorShape cursorForEdge(ResizeEdge edge) {
    switch (edge) {
    case ResizeEdge::TopLeft:
    case ResizeEdge::BottomRight:
        return Qt::SizeFDiagCursor;
    case ResizeEdge::TopRight:
    case ResizeEdge::BottomLeft:
        return Qt::SizeBDiagCursor;
    case ResizeEdge::Top:
    case ResizeEdge::Bottom:
        return Qt::SizeVerCursor;
    case ResizeEdge::Left:
    case ResizeEdge::Right:
        return Qt::SizeHorCursor;
    default:
        return Qt::ArrowCursor;
    }
}


// ============================================================================= //


CropWidget::CropWidget(QWidget* parent) :
    ThumbnailWidget{parent} {
    setMouseTracking(true);
}

void CropWidget::setImage(const QImage& image) {
    mSourceImage = image;
    setPixmap(QPixmap::fromImage(image));
    resetView();
    setCropRect(QRect());
}

void CropWidget::setCropEnabled(bool enabled) {
    mCropEnabled = enabled;
    setCursor(Qt::ArrowCursor);
    if (!enabled) {
        setCropRect(QRect());
    }
}

void CropWidget::clearCrop() {
    mCropRect = QRect();
    update();
    emit cropChanged(mCropRect);
}

void CropWidget::setCropRect(s32 x, s32 y, s32 w, s32 h) {
    if (mSourceImage.isNull()) {
        clearCrop();
        return;
    }

    w = std::max(1, w);
    h = std::max(1, h);

    s32 maxX = std::max(0, mSourceImage.width() - w);
    s32 maxY = std::max(0, mSourceImage.height() - h);
    x = std::clamp(x, 0, maxX);
    y = std::clamp(y, 0, maxY);
    w = std::min(w, mSourceImage.width() - x);
    h = std::min(h, mSourceImage.height() - y);

    QRect newRect(x, y, w, h);
    if (newRect == mCropRect) {
        return;
    }

    mCropRect = newRect;
    update();
    emit cropChanged(mCropRect);
}

void CropWidget::setCropRect(const QRect& rect) {
    if (rect.isValid()) {
        setCropRect(rect.x(), rect.y(), rect.width(), rect.height());
    } else if (mCropRect.isValid()) {
        clearCrop();
    }
}

QRect CropWidget::cropRect() const {
    return mCropRect;
}

bool CropWidget::canCrop() const {
    return mCropEnabled && !mSourceImage.isNull();
}

bool CropWidget::hasValidCrop() const {
    return canCrop() && mCropRect.isValid();
}

QTransform CropWidget::imageToWidgetTransform() const {
    if (mPixmap.isNull() || mTextureRect.isNull()) {
        return {};
    }
    f32 scaleX = static_cast<f32>(mTextureRect.width()) / static_cast<f32>(mPixmap.width());
    f32 scaleY = static_cast<f32>(mTextureRect.height()) / static_cast<f32>(mPixmap.height());
    QTransform transform;
    transform.translate(mTextureRect.x(), mTextureRect.y());
    transform.scale(scaleX, scaleY);
    return transform;
}

QRect CropWidget::imageToWidget(const QRect& imageRect) const {
    if (mPixmap.isNull() || mTextureRect.isNull()) {
        return {};
    }
    QTransform transform = imageToWidgetTransform();
    QPointF topLeft = transform.map(QPointF(imageRect.topLeft()));
    QPointF bottomRight = transform.map(QPointF(imageRect.bottomRight() + QPoint(1, 1)));
    return {topLeft.toPoint(), bottomRight.toPoint()};
}

QPointF CropWidget::widgetToImage(QPoint widgetPos) const {
    if (mPixmap.isNull() || mTextureRect.isNull()) {
        return {};
    }
    return imageToWidgetTransform().inverted().map(QPointF(widgetPos));
}

ResizeEdge CropWidget::edgeAt(QPoint widgetPos) const {
    if (!hasValidCrop()) {
        return ResizeEdge::None;
    }

    s32 radius = sResizeHandleSize;
    QRect rect = imageToWidget(mCropRect);

    if (nearPoint(widgetPos, rect.topLeft(),     radius)) { return ResizeEdge::TopLeft;     }
    if (nearPoint(widgetPos, rect.topRight(),    radius)) { return ResizeEdge::TopRight;    }
    if (nearPoint(widgetPos, rect.bottomLeft(),  radius)) { return ResizeEdge::BottomLeft;  }
    if (nearPoint(widgetPos, rect.bottomRight(), radius)) { return ResizeEdge::BottomRight; }

    if (nearHorizontalEdge(widgetPos, rect.top(),    rect.left(), rect.right(),  radius)) { return ResizeEdge::Top;    }
    if (nearHorizontalEdge(widgetPos, rect.bottom(), rect.left(), rect.right(),  radius)) { return ResizeEdge::Bottom; }
    if (nearVerticalEdge(widgetPos,   rect.left(),   rect.top(),  rect.bottom(), radius)) { return ResizeEdge::Left;   }
    if (nearVerticalEdge(widgetPos,   rect.right(),  rect.top(),  rect.bottom(), radius)) { return ResizeEdge::Right;  }

    return ResizeEdge::None;
}

void CropWidget::drawHandle(QPainter& painter, QPoint center) {
    s32 halfSize = sResizeHandleSize / 2;
    QColor handleColor = palette().color(QPalette::Highlight);
    handleColor.setAlpha(180);
    painter.fillRect(center.x() - halfSize, center.y() - halfSize, halfSize * 2, halfSize * 2, handleColor);
    painter.setPen(QPen(palette().color(QPalette::Highlight), 1));
    painter.drawRect(center.x() - halfSize, center.y() - halfSize, halfSize * 2, halfSize * 2);
}

void CropWidget::paintEvent(QPaintEvent* event) {
    ThumbnailWidget::paintEvent(event);

    if (!hasValidCrop()) {
        return;
    }

    QPainter painter(this);

    const QRect widgetCropRect = imageToWidget(mCropRect);
    const QPen borderPen(palette().color(QPalette::Highlight), 2);

    QRegion outside(mTextureRect);
    outside -= widgetCropRect;

    painter.save();
    painter.setClipRegion(outside);
    painter.fillRect(mTextureRect, {0, 0, 0, 90});
    painter.restore();

    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(widgetCropRect);

    int handleSize = qMin(widgetCropRect.width(), widgetCropRect.height());
    if (handleSize < sResizeHandleSize) {
        return;
    }

    drawHandle(painter, widgetCropRect.topLeft());
    drawHandle(painter, widgetCropRect.topRight());
    drawHandle(painter, widgetCropRect.bottomLeft());
    drawHandle(painter, widgetCropRect.bottomRight());

    QPoint center = widgetCropRect.center();
    drawHandle(painter, QPoint(center.x(), widgetCropRect.top()));
    drawHandle(painter, QPoint(center.x(), widgetCropRect.bottom()));
    drawHandle(painter, QPoint(widgetCropRect.left(), center.y()));
    drawHandle(painter, QPoint(widgetCropRect.right(), center.y()));
}

QRect CropWidget::resizeRect(ResizeEdge edge, QPoint imagePos, const QRect& original, bool snapToValid) const {
    bool moveLeft   = hasFlag(edge, ResizeEdge::Left);
    bool moveRight  = hasFlag(edge, ResizeEdge::Right);
    bool moveTop    = hasFlag(edge, ResizeEdge::Top);
    bool moveBottom = hasFlag(edge, ResizeEdge::Bottom);

    s32 imageX = imagePos.x();
    s32 imageY = imagePos.y();
    s32 width = mSourceImage.width();
    s32 height = mSourceImage.height();

    QRect rect = original;

    if (moveLeft)   { rect.setLeft(std::clamp(imageX, 0, rect.right())); }
    if (moveRight)  { rect.setRight(std::clamp(imageX - 1, rect.left(), width - 1)); }
    if (moveTop)    { rect.setTop(std::clamp(imageY, 0, rect.bottom())); }
    if (moveBottom) { rect.setBottom(std::clamp(imageY - 1, rect.top(), height - 1)); }

    clampResizeRect(rect, mSourceImage.size());

    if (snapToValid) {
        snapResizeRect(rect, edge);
    }

    clampResizeRect(rect, mSourceImage.size());

    return rect;
}

bool CropWidget::handleResize(QMouseEvent* event) {
    if (mResizeEdge == ResizeEdge::None || mSourceImage.isNull()) {
        return false;
    }

    QPointF mouseImagePos = widgetToImage(event->pos());
    QPoint imagePos(qRound(mouseImagePos.x()), qRound(mouseImagePos.y()));

    bool snapToValid = event->modifiers() & Qt::ShiftModifier;
    QRect newRect = resizeRect(mResizeEdge, imagePos, mResizeOrigRect, snapToValid);
    setCropRect(newRect.x(), newRect.y(), newRect.width(), newRect.height());
    return true;
}

bool CropWidget::handleDrag(QMouseEvent* event) {
    if (!mIsDragging || !mCropRect.isValid()) {
        return false;
    }

    QPointF mouseDelta = widgetToImage(event->pos()) - widgetToImage(mDragStartWidget);
    s32 newX = mDragOrigX + qRound(mouseDelta.x());
    s32 newY = mDragOrigY + qRound(mouseDelta.y());
    setCropRect(newX, newY, mCropRect.width(), mCropRect.height());
    return true;
}

void CropWidget::updateHoverCursor(QMouseEvent* event) {
    if (mCropRect.isValid()) {
        ResizeEdge edge = edgeAt(event->pos());
        if (edge != ResizeEdge::None) {
            setCursor(cursorForEdge(edge));
            return;
        }
    }

    setCursor(Qt::ArrowCursor);
}

void CropWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {
        ThumbnailWidget::mousePressEvent(event);
        return;
    }

    if (!hasValidCrop()) {
        ThumbnailWidget::mousePressEvent(event);
        return;
    }

    QRect widgetCropRect = imageToWidget(mCropRect);

    ResizeEdge edge = edgeAt(event->pos());
    if (edge != ResizeEdge::None) {
        mResizeEdge = edge;
        mResizeOrigRect = mCropRect;
        setCursor(cursorForEdge(edge));
        grabMouse();
        return;
    }

    if (widgetCropRect.contains(event->pos())) {
        mIsDragging = true;
        mDragOrigX = mCropRect.x();
        mDragOrigY = mCropRect.y();
        mDragStartWidget = event->pos();
        setCursor(Qt::ClosedHandCursor);
        grabMouse();
        return;
    }

    ThumbnailWidget::mousePressEvent(event);
}

void CropWidget::mouseMoveEvent(QMouseEvent* event) {
    if (!canCrop()) {
        ThumbnailWidget::mouseMoveEvent(event);
        return;
    }

    if (handleDrag(event)) {
        return;
    }

    if (handleResize(event)) {
        return;
    }

    updateHoverCursor(event);
}

void CropWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (mIsDragging || mResizeEdge != ResizeEdge::None) {
        mIsDragging = false;
        mResizeEdge = ResizeEdge::None;
        releaseMouse();
        setCursor(Qt::ArrowCursor);
        return;
    }
    ThumbnailWidget::mouseReleaseEvent(event);
}

void CropWidget::resizeEvent(QResizeEvent* event) {
    ThumbnailWidget::resizeEvent(event);
}


// ============================================================================= //
