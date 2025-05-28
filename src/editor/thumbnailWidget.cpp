#include "editor/thumbnailWidget.h"

#include <QPainter>
#include <QStyleOption>
#include <QMouseEvent>

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
    drawCheckerBackground(painter, bounds);

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

void ThumbnailWidget::drawCheckerBackground(QPainter& painter, const QRect& rect) {
    const int checkerSize = 8;
    QColor light = QColor(220, 220, 220);
    QColor dark = QColor(180, 180, 180);

    for (int y = rect.top(); y < rect.bottom(); y += checkerSize) {
        for (int x = rect.left(); x < rect.right(); x += checkerSize) {
            QRect cell(x, y, checkerSize, checkerSize);
            bool isLight = ((x / checkerSize + y / checkerSize) % 2) == 0;
            painter.fillRect(cell, isLight ? light : dark);
        }
    }
}
