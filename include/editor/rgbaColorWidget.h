#ifndef RGBACOLORWIDGET_H
#define RGBACOLORWIDGET_H

#include "editor/alphaSlider.h"
#include "editor/clickableLabel.h"
#include "editor/gradientSlider.h"
#include "ptcl/ptclBinary.h"

#include <QDoubleSpinBox>
#include <QWidget>


// ========================================================================== //


class RGBAColorWidget : public QWidget {
    Q_OBJECT
public:
    explicit RGBAColorWidget(QWidget* parent = nullptr);

    void setColor(const Ptcl::binColor4f& color);
    Ptcl::binColor4f color() const;
    QColor toQColor() const;

signals:
    void colorChanged();

private slots:
    void updateColorFromSliders();
    void updateColorFromSpinBoxes();
    void openColorDialog();

private:
    GradientSlider mSliderR;
    GradientSlider mSliderG;
    GradientSlider mSliderB;
    AlphaSlider mSliderA;

    QDoubleSpinBox mSpinBoxR;
    QDoubleSpinBox mSpinBoxG;
    QDoubleSpinBox mSpinBoxB;
    QDoubleSpinBox mSpinBoxA;

    ClickableLabel mPreview;

    void updatePreview();
};


// ========================================================================== //


#endif // RGBACOLORWIDGET_H
