#include "editor/components/thumbnailWidget.h"
#include "util/paintUtil.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>


// ========================================================================== //


ThumbnailWidget::ThumbnailWidget(QWidget* parent) :
    QWidget{parent} {
    setMinimumSize(mThumbnailSize);

    mHScrollBar = new QScrollBar(Qt::Horizontal, this);
    mVScrollBar = new QScrollBar(Qt::Vertical, this);

    mHScrollBar->hide();
    mVScrollBar->hide();

    connect(mHScrollBar, &QScrollBar::valueChanged, this, [this](int value) {
        s32 maxScroll = std::max(0, mTextureRect.width() - width());
        mScrollOffset.rx() = maxScroll / 2 - value;
        calcTextureRect();
        update();
    });

    connect(mVScrollBar, &QScrollBar::valueChanged, this, [this](int value) {
        s32 maxScroll = std::max(0, mTextureRect.height() - height());
        mScrollOffset.ry() = maxScroll / 2 - value;
        calcTextureRect();
        update();
    });
}

void ThumbnailWidget::setPixmap(const QPixmap& pixmap) {
    mPixmap = pixmap;

    calcTextureRect();
    update();
}

void ThumbnailWidget::setThumbnailSize(QSize size) {
    mThumbnailSize = size;

    calcTextureRect();
    updateGeometry();
}

void ThumbnailWidget::clear() {
    mPixmap = {};
    mZoomEnabled = false;
    resetView();
}

void ThumbnailWidget::resetView() {
    mZoom = 1.0f;
    mScrollOffset = {0, 0};

    calcTextureRect();
    updateScrollBars();
    update();
}

void ThumbnailWidget::setZoomEnabled(bool enabled) {
    mZoomEnabled = enabled;
    if (!enabled) {
        resetView();
    } else {
        updateScrollBars();
    }
}

void ThumbnailWidget::zoomIn() {
    if (!mZoomEnabled || mPixmap.isNull()) {
        return;
    }
    mZoom = qBound(0.1f, mZoom * 1.25f, 20.0f);
    calcTextureRect();
    updateScrollBars();
    update();
}

void ThumbnailWidget::zoomOut() {
    if (!mZoomEnabled || mPixmap.isNull()) {
        return;
    }
    mZoom = qBound(0.1f, mZoom / 1.25f, 20.0f);
    calcTextureRect();
    updateScrollBars();
    update();
}

QSize ThumbnailWidget::sizeHint() const {
    return mThumbnailSize;
}

void ThumbnailWidget::calcTextureRect() {
    if (mPixmap.isNull()) {
        mTextureRect = {};
        return;
    }

    QSize baseSize = mPixmap.size();
    baseSize.scale(mThumbnailSize, Qt::KeepAspectRatio);

    QSize zoomedSize(
        static_cast<s32>(static_cast<f32>(baseSize.width()) * mZoom),
        static_cast<s32>(static_cast<f32>(baseSize.height()) * mZoom)
    );

    mTextureRect = {
        (width() - zoomedSize.width()) / 2 + mScrollOffset.x(),
        (height() - zoomedSize.height()) / 2 + mScrollOffset.y(),
        zoomedSize.width(),
        zoomedSize.height()
    };
}

void ThumbnailWidget::updateScrollBars() {
    if (!mZoomEnabled || mPixmap.isNull()) {
        mHScrollBar->hide();
        mVScrollBar->hide();
        return;
    }

    s32 sb = style()->pixelMetric(QStyle::PM_ScrollBarExtent);

    s32 cw = width() - (mVScrollBar->isVisible() ? sb : 0);
    s32 ch = height() - (mHScrollBar->isVisible() ? sb : 0);

    s32 zw = mTextureRect.width();
    s32 zh = mTextureRect.height();
    s32 maxScrollH = std::max(0, zw - cw);
    s32 maxScrollV = std::max(0, zh - ch);

    bool needH = zw > cw;
    bool needV = zh > ch;

    if (needH) {
        mHScrollBar->setRange(0, maxScrollH);
        mHScrollBar->setValue(maxScrollH / 2 - mScrollOffset.x());
        mHScrollBar->setPageStep(cw);
        mHScrollBar->show();
    } else {
        mHScrollBar->hide();
    }

    if (needV) {
        mVScrollBar->setRange(0, maxScrollV);
        mVScrollBar->setValue(maxScrollV / 2 - mScrollOffset.y());
        mVScrollBar->setPageStep(ch);
        mVScrollBar->show();
    } else {
        mVScrollBar->hide();
    }

    s32 hsbHeight = mHScrollBar->isVisible() ? sb : 0;
    s32 vsbWidth = mVScrollBar->isVisible() ? sb : 0;

    mHScrollBar->setGeometry(0, height() - sb, width() - vsbWidth, sb);
    mVScrollBar->setGeometry(width() - sb, 0, sb, height() - hsbHeight);

    if (needH || needV) {
        setMouseTracking(true);
    } else {
        setMouseTracking(false);
    }
}

void ThumbnailWidget::drawBackground(QPainter& painter) const {
    painter.fillRect(rect(), palette().window().color());

    QColor dim(0, 0, 0, 100);

    QRegion outside(rect());
    outside -= mTextureRect;

    for (const QRect& r : outside) {
        painter.fillRect(r, dim);
    }
}

void ThumbnailWidget::drawCheckerboard(QPainter& painter) const {
    f32 sx = static_cast<f32>(mTextureRect.width())  / static_cast<f32>(mPixmap.width());
    f32 sy = static_cast<f32>(mTextureRect.height()) / static_cast<f32>(mPixmap.height());

    f32 pixelsPerTexel = std::min(sx, sy);

    s32 checkerSize = std::max(2, static_cast<s32>(std::round(pixelsPerTexel * 4.0f)));

    PaintUtil::drawCheckerboard(painter, mTextureRect, checkerSize, mTextureRect.size());
}


void ThumbnailWidget::drawTexture(QPainter& painter) const {
    if (mPixmap.isNull()) {
        return;
    }

    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.drawPixmap(mTextureRect, mPixmap);
}

void ThumbnailWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);
    drawCheckerboard(painter);
    drawTexture(painter);
}

void ThumbnailWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    calcTextureRect();
    updateScrollBars();
}

void ThumbnailWidget::wheelEvent(QWheelEvent* event) {
    if (!mZoomEnabled || mPixmap.isNull() || mTextureRect.isNull()) {
        return;
    }

    QPointF cursor = event->position();

    f32 scaleX = static_cast<f32>(mTextureRect.width()) / static_cast<f32>(mPixmap.width());
    f32 scaleY = static_cast<f32>(mTextureRect.height()) / static_cast<f32>(mPixmap.height());

    f32 imgX = scaleX > 0.f ? (static_cast<f32>(cursor.x()) - static_cast<f32>(mTextureRect.x())) / scaleX : 0.f;
    f32 imgY = scaleY > 0.f ? (static_cast<f32>(cursor.y()) - static_cast<f32>(mTextureRect.y())) / scaleY : 0.f;

    double delta = event->angleDelta().y() / 120.0;
    mZoom *= static_cast<f32>(std::pow(1.1, delta));
    mZoom = qBound(0.1f, mZoom, 20.0f);

    QSize baseSize = mPixmap.size();
    baseSize.scale(mThumbnailSize, Qt::KeepAspectRatio);

    s32 zw = static_cast<s32>(static_cast<f32>(baseSize.width()) * mZoom);
    s32 zh = static_cast<s32>(static_cast<f32>(baseSize.height()) * mZoom);

    f32 newScaleX = static_cast<f32>(zw) / static_cast<f32>(mPixmap.width());
    f32 newScaleY = static_cast<f32>(zh) / static_cast<f32>(mPixmap.height());

    f32 desiredX = static_cast<f32>(cursor.x()) - imgX * newScaleX;
    f32 desiredY = static_cast<f32>(cursor.y()) - imgY * newScaleY;

    mScrollOffset.rx() = static_cast<s32>(desiredX - static_cast<f32>(width() - zw) * 0.5f);
    mScrollOffset.ry() = static_cast<s32>(desiredY - static_cast<f32>(height() - zh) * 0.5f);

    calcTextureRect();
    updateScrollBars();
    update();
}

void ThumbnailWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }

    QWidget::mousePressEvent(event);
}


// ========================================================================== //
