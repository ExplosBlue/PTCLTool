#pragma once

#include <QSlider>


// ========================================================================== //


class GradientSlider : public QSlider {
    Q_OBJECT
private:
    static constexpr int sHandleRadius = 8;
    static constexpr int sGrooveHeight = 10;
    static constexpr int sHorizontalPadding = sHandleRadius;
    int centerY = height() / 2;

public:
    enum class ChannelType {
        Red,
        Green,
        Blue
    };

public:
    explicit GradientSlider(Qt::Orientation orientation, QWidget* parent = nullptr);

    void setChannel(ChannelType channel);
    void setRgbValues(int r, int g, int b);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    double getHandlePercentage() const;
    QPoint getHandlePos() const;

    ChannelType mChannel;
    int mR;
    int mG;
    int mB;

    bool mIsHovered;
};


// ========================================================================== //
