#pragma once

#include "gfx/color.h"

#include "editor/components/alphaSlider.h"
#include "editor/components/clickableLabel.h"
#include "editor/components/gradientSlider.h"

#include <QDoubleSpinBox>
#include <QLayoutItem>
#include <QVBoxLayout>
#include <QWidget>


// ========================================================================== //


class ColorChannelRow : public QWidget {
    Q_OBJECT
public:
    explicit ColorChannelRow(const QString& name, QSlider* slider, QWidget* parent = nullptr);

    QSlider* slider() { return mSlider; }
    QDoubleSpinBox* spinBox() { return &mSpinBox; }

signals:
    void spinChanged(double value);
    void sliderChanged(s32 value);

private:
    QLabel mLabel{};
    QDoubleSpinBox mSpinBox{};
    QSlider* mSlider{nullptr};
};


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
    GradientSlider* mSliderR{nullptr};
    GradientSlider* mSliderG{nullptr};
    GradientSlider* mSliderB{nullptr};
    AlphaSlider* mSliderA{nullptr};

    ColorChannelRow* mRowR{nullptr};
    ColorChannelRow* mRowG{nullptr};
    ColorChannelRow* mRowB{nullptr};
    ColorChannelRow* mRowA{nullptr};
    std::vector<ColorChannelRow*> mRows{};
    QWidget* mRowsContainer{nullptr};

    ClickableLabel mPreview{};

    QVBoxLayout* mSliderLayout{nullptr};
};


// ========================================================================== //

