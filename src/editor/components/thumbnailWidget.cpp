#include "editor/components/thumbnailWidget.h"
#include "util/paintUtil.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>


// ========================================================================== //


ThumbnailWidget::ThumbnailWidget(QWidget* parent) :
    QWidget(parent), mThumbnailSize(64, 64) {
    setMinimumSize(mThumbnailSize);
}

void ThumbnailWidget::setPixmap(const QPixmap& pixmap) {
    mPixmap = pixmap;
    update();
}

void ThumbnailWidget::setThumbnailSize(QSize size) {
    mThumbnailSize = size;
    setFixedSize(size);
    updateGeometry();
}

QSize ThumbnailWidget::sizeHint() const {
    return mThumbnailSize;
}

void ThumbnailWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect bounds = rect();
    PaintUtil::drawCheckerboard(painter, rect());

    if (!mPixmap.isNull()) {
        QPixmap scaled = mPixmap.scaled(mThumbnailSize, Qt::KeepAspectRatio, Qt::FastTransformation);

        int x = (bounds.width() - scaled.width()) / 2;
        int y = (bounds.height() - scaled.height()) / 2;

        painter.drawPixmap(x, y, scaled);
    }
}

void ThumbnailWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }

    QWidget::mousePressEvent(event);
}


// ========================================================================== //
