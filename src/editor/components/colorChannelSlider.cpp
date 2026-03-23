#include "editor/components/colorChannelSlider.h"

#include "util/paintUtil.h"

#include "typedefs.h"

#include <cmath>

#include <QStyleOptionSlider>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>


// ========================================================================== //


ColorChannelSlider::ColorChannelSlider(Qt::Orientation orientation, QWidget* parent) :
    QSlider{orientation, parent} {
    setMouseTracking(true);
    setMaximum(255);
}

void ColorChannelSlider::setChannel(Gfx::Color::Channel channel) {
    mChannel = channel;
    update();
}

void ColorChannelSlider::setColor(const Gfx::Color& color) {
    mBaseColor = color;
    setValue(static_cast<s32>(std::round(mBaseColor[mChannel] * 255.0f)));
    update();
}

double ColorChannelSlider::getHandlePercentage() const {
    return static_cast<double>(value() - minimum()) / (maximum() - minimum());
}

QPoint ColorChannelSlider::getHandlePos() const {
    const s32 grooveWidth = width() - 2 * sHorizontalPadding;
    const s32 handleX = static_cast<s32>(sHorizontalPadding + getHandlePercentage() * grooveWidth);
    const s32 handleY = height() / 2;
    return {handleX, handleY};
}

std::pair<QColor, QColor> ColorChannelSlider::calculateGradientColors() const {
    Gfx::Color color = mBaseColor;

    color[mChannel] = 0.0f;
    QColor startColor = color.toQColor();

    color[mChannel] = 1.0f;
    QColor endColor = color.toQColor();

    return { startColor, endColor };
}

void ColorChannelSlider::drawTrack(QPainter& painter) {
    s32 centerY = height() / 2;
    QRect grooveRect(sHorizontalPadding, centerY - sGrooveHeight / 2, width() - 2 * sHorizontalPadding, sGrooveHeight);

    QPainterPath groovePath;
    groovePath.addRoundedRect(grooveRect, sGrooveHeight / 2.0, sGrooveHeight / 2.0);

    painter.save();
    painter.setClipPath(groovePath);

    PaintUtil::drawCheckerboard(painter, grooveRect, 6);

    auto [startColor, endColor] = calculateGradientColors();
    QLinearGradient gradient(grooveRect.topLeft(), grooveRect.topRight());
    gradient.setColorAt(0, startColor);
    gradient.setColorAt(1, endColor);

    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(grooveRect, sGrooveHeight / 2.0, sGrooveHeight / 2.0);

    painter.restore();
}

void ColorChannelSlider::drawHandle(QPainter& painter) {
    double t = getHandlePercentage();
    auto [startColor, endColor] = calculateGradientColors();
    QColor handleColor{
        static_cast<s32>(std::lerp(startColor.red(), endColor.red(), t)),
        static_cast<s32>(std::lerp(startColor.green(), endColor.green(), t)),
        static_cast<s32>(std::lerp(startColor.blue(), endColor.blue(), t)),
        static_cast<s32>(std::lerp(startColor.alpha(), endColor.alpha(), t)),
    };

    QColor borderColor = Qt::black;

    if (mIsHovered) {
        handleColor = handleColor.lighter(120);
    }

    setCursor(mIsHovered ? Qt::PointingHandCursor : Qt::ArrowCursor);

    painter.setBrush(handleColor);
    painter.setPen(QPen(borderColor, 2));
    painter.drawEllipse(getHandlePos(), sHandleRadius, sHandleRadius);
}

void ColorChannelSlider::paintEvent(QPaintEvent* event) {
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawTrack(painter);
    drawHandle(painter);
}

void ColorChannelSlider::mouseMoveEvent(QMouseEvent* event) {
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

void ColorChannelSlider::leaveEvent(QEvent* event) {
    if (mIsHovered) {
        mIsHovered = false;
        update();
    }

    QSlider::leaveEvent(event);
}


// ========================================================================== //
