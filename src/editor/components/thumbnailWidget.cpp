#include "editor/components/thumbnailWidget.h"
#include "util/paintUtil.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>


// ========================================================================== //


ThumbnailWidget::ThumbnailWidget(QWidget* parent) :
    QWidget{parent} {
    setMinimumSize(mThumbnailSize);
}

void ThumbnailWidget::setPixmap(const QPixmap& pixmap) {
    mPixmap = pixmap;

    calcTextureRect();
    update();
}

void ThumbnailWidget::setThumbnailSize(QSize size) {
    mThumbnailSize = size;
    setFixedSize(size);

    calcTextureRect();
    updateGeometry();
}

void ThumbnailWidget::clear() {
    mPixmap = {};

    calcTextureRect();
    update();
}

QSize ThumbnailWidget::sizeHint() const {
    return mThumbnailSize;
}

void ThumbnailWidget::calcTextureRect() {
    if (mPixmap.isNull()) {
        mTextureRect = {};
    }

    QSize scaledSize = mPixmap.size();
    scaledSize.scale(mThumbnailSize, Qt::KeepAspectRatio);

    mTextureRect = {
        QPoint{
            (width() - scaledSize.width()) / 2,
            (height() - scaledSize.height()) / 2
        },
        scaledSize
    };
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

void ThumbnailWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }

    QWidget::mousePressEvent(event);
}


// ========================================================================== //
