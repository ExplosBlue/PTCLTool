#include "editor/alphaSlider.h"

#include <QStyleOptionSlider>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

AlphaSlider::AlphaSlider(Qt::Orientation orientation, QWidget* parent) :
    QSlider(orientation, parent), mR{0}, mG{0}, mB{0}, mIsHovered{false} {
    setMouseTracking(true);
    setMaximum(255);
}

double AlphaSlider::getHandlePercentage() const {
    return static_cast<double>(value() - minimum()) / (maximum() - minimum());
}

QPoint AlphaSlider::getHandlePos() const {
    const int grooveWidth = width() - 2 * sHorizontalPadding;
    const int handleX = static_cast<int>(sHorizontalPadding + getHandlePercentage() * grooveWidth);
    const int handleY = height() / 2;
    return {handleX, handleY};
}

void AlphaSlider::setColor(int r, int g, int b) {
    mR = r;
    mG = g;
    mB = b;
    update();
}

void AlphaSlider::paintEvent(QPaintEvent* event) {
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int centerY = height() / 2;

    QRect grooveRect(sHorizontalPadding, centerY - sGrooveHeight / 2, width() - 2 * sHorizontalPadding, sGrooveHeight);

    QPainterPath groovePath;
    groovePath.addRoundedRect(grooveRect, sGrooveHeight / 2.0, sGrooveHeight / 2.0);

    painter.save();
    painter.setClipPath(groovePath);

    const int checkerSize = 6;
    QColor light = QColor(220, 220, 220);
    QColor dark = QColor(180, 180, 180);

    for (int y = grooveRect.top(); y < grooveRect.bottom(); y += checkerSize) {
        for (int x = grooveRect.left(); x < grooveRect.right(); x += checkerSize) {
            QRect cell(x, y, checkerSize, checkerSize);
            bool isLight = ((x / checkerSize + y / checkerSize) % 2) == 0;
            painter.fillRect(cell, isLight ? light : dark);
        }
    }

    painter.restore();

    QLinearGradient gradient(grooveRect.topLeft(), grooveRect.topRight());
    gradient.setColorAt(0, QColor(mR, mG, mB, 0));
    gradient.setColorAt(1, QColor(mR, mG, mB, 255));

    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(grooveRect, sGrooveHeight / 2.0, sGrooveHeight / 2.0);

    QColor handleColor = opt.palette.color(QPalette::Button);
    QColor borderColor = opt.palette.color(QPalette::Dark);

    if (mIsHovered) {
        handleColor = handleColor.lighter(120);
    }

    setCursor(mIsHovered ? Qt::PointingHandCursor : Qt::ArrowCursor);

    painter.setBrush(handleColor);
    painter.setPen(QPen(borderColor, 1));
    painter.drawEllipse(getHandlePos(), sHandleRadius, sHandleRadius);
}

void AlphaSlider::mouseMoveEvent(QMouseEvent* event) {
    bool wasHovered = mIsHovered;

    QPoint mousePos = event->pos();
    QPoint handlePos = getHandlePos();

    QRect handleRect = QRect(handlePos - QPoint(sHandleRadius, sHandleRadius),
                             handlePos + QPoint(sHandleRadius, sHandleRadius));

    mIsHovered = handleRect.contains(mousePos);

    if (mIsHovered != wasHovered) {
        update();
    }

    QSlider::mouseMoveEvent(event);
}

void AlphaSlider::leaveEvent(QEvent* event) {
    if (mIsHovered) {
        mIsHovered = false;
        update();
    }

    QSlider::leaveEvent(event);
}
