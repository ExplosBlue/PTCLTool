#include "editor/gradientSlider.h"

#include <QStyleOptionSlider>
#include <QPainter>
#include <QMouseEvent>


// ========================================================================== //


GradientSlider::GradientSlider(Qt::Orientation orientation, QWidget* parent) :
    QSlider(orientation, parent), mChannel{ChannelType::Red}, mR{0}, mG{0}, mB{0}, mIsHovered{false} {
    setMouseTracking(true);
    setMaximum(255);
}

void GradientSlider::setChannel(ChannelType channel) {
    mChannel = channel;
    update();
}

void GradientSlider::setRgbValues(int r, int g, int b) {
    mR = r;
    mG = g;
    mB = b;

    update();
}

double GradientSlider::getHandlePercentage() const {
    return static_cast<double>(value() - minimum()) / (maximum() - minimum());
}

QPoint GradientSlider::getHandlePos() const {
    const int grooveWidth = width() - 2 * sHorizontalPadding;
    const int handleX = static_cast<int>(sHorizontalPadding + getHandlePercentage() * grooveWidth);
    const int handleY = height() / 2;
    return {handleX, handleY};
}

void GradientSlider::paintEvent(QPaintEvent* event) {
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int centerY = height() / 2;

    QRect grooveRect(sHorizontalPadding, centerY - sGrooveHeight / 2, width() - 2 * sHorizontalPadding, sGrooveHeight);
    QLinearGradient gradient(grooveRect.topLeft(), grooveRect.topRight());

    QColor startColor, endColor;

    std::array<int, 3> color = { mR, mG, mB };
    int channelIndex = (mChannel == ChannelType::Red) ? 0 : (mChannel == ChannelType::Green) ? 1 : 2;

    color[channelIndex] = 0;
    startColor = QColor(color[0], color[1], color[2]);

    color[channelIndex] = 255;
    endColor = QColor(color[0], color[1], color[2]);

    gradient.setColorAt(0, startColor);
    gradient.setColorAt(1, endColor);

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

void GradientSlider::mouseMoveEvent(QMouseEvent* event) {
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

void GradientSlider::leaveEvent(QEvent* event) {
    if (mIsHovered) {
        mIsHovered = false;
        update();
    }

    QSlider::leaveEvent(event);
}


// ========================================================================== //
