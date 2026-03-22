#pragma once

#include "gfx/color.h"

#include "editor/components/alphaSlider.h"
#include "editor/components/clickableLabel.h"
#include "editor/components/gradientSlider.h"

#include <QDoubleSpinBox>
#include <QWidget>


// ========================================================================== //


class RGBAColorWidget : public QWidget {
    Q_OBJECT
public:
    explicit RGBAColorWidget(QWidget* parent = nullptr);

    void setColor(const Gfx::Color& color);
    Gfx::Color color() const;
    QColor toQColor() const;

    void enableAlpha(bool enable);

signals:
    void colorChanged();

private slots:
    void updateColorFromSliders();
    void updateColorFromSpinBoxes();
    void openColorDialog();

private:
    void updatePreview();

private:
    GradientSlider mSliderR;
    GradientSlider mSliderG;
    GradientSlider mSliderB;
    AlphaSlider mSliderA;

    QDoubleSpinBox mSpinBoxR{};
    QDoubleSpinBox mSpinBoxG{};
    QDoubleSpinBox mSpinBoxB{};
    QDoubleSpinBox mSpinBoxA{};

    ClickableLabel mPreview{};

    QWidget* mAlphaRow{nullptr};
};


// ========================================================================== //

