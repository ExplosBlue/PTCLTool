#pragma once

#include "gfx/color.h"

#include <QSlider>


// ========================================================================== //


class ColorChannelSlider : public QSlider {
    Q_OBJECT
private:
    static constexpr s32 sHandleRadius = 8;
    static constexpr s32 sGrooveHeight = 10;
    static constexpr s32 sHorizontalPadding = sHandleRadius;
    s32 centerY = height() / 2;

public:
    explicit ColorChannelSlider(Qt::Orientation orientation, QWidget* parent = nullptr);

    void setChannel(Gfx::Color::Channel channel);
    void setColor(const Gfx::Color& color);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    double getHandlePercentage() const;
    QPoint getHandlePos() const;

    void drawTrack(QPainter& painter);
    void drawHandle(QPainter& painter);

    std::pair<QColor, QColor> calculateGradientColors() const;

private:
    Gfx::Color::Channel mChannel{Gfx::Color::Channel::R};
    Gfx::Color mBaseColor{};

    bool mIsHovered{false};
};


// ========================================================================== //
