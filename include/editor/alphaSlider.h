#ifndef ALPHASLIDER_H
#define ALPHASLIDER_H

#include <QSlider>

class AlphaSlider : public QSlider {
    Q_OBJECT
private:
    static constexpr int sHandleRadius = 8;
    static constexpr int sGrooveHeight = 10;
    static constexpr int sHorizontalPadding = sHandleRadius;
    int centerY = height() / 2;

public:
    explicit AlphaSlider(Qt::Orientation orientation, QWidget* parent = nullptr);

    void setColor(int r, int g, int b);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    double getHandlePercentage() const;
    QPoint getHandlePos() const;

    int mR;
    int mG;
    int mB;

    bool mIsHovered;
};


#endif // ALPHASLIDER_H
