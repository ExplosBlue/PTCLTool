#pragma once

#include "gfx/color.h"

#include "editor/components/clickableLabel.h"
#include "editor/components/colorChannelSlider.h"

#include <QDoubleSpinBox>
#include <QLayoutItem>
#include <QVBoxLayout>
#include <QWidget>


// ========================================================================== //


class ColorChannelRow : public QWidget {
    Q_OBJECT
public:
    explicit ColorChannelRow(const QString& name, Gfx::Color::Channel channel, QWidget* parent = nullptr);

    void setColor(const Gfx::Color& color);
    Gfx::Color::Channel channel() const { return mChannel; }

signals:
    void valueChanging(Gfx::Color::Channel channel, f32 value);
    void valueCommitted(Gfx::Color::Channel channel, f32 value);

private slots:
    void spinChanged(double value);
    void sliderChanged(s32 value);

private:
    Gfx::Color::Channel mChannel{};
    Gfx::Color mColor{};

    QLabel mLabel{};
    QDoubleSpinBox mSpinBox{};
    ColorChannelSlider* mSlider{nullptr};
};


// ========================================================================== //


class RGBAColorWidget : public QWidget {
    Q_OBJECT
public:
    explicit RGBAColorWidget(QWidget* parent = nullptr);

    void setColor(const Gfx::Color& color);
    Gfx::Color color() const { return mColor; }

    void enableAlpha(bool enable);

signals:
    void colorChanged();

private slots:
    void openColorDialog();
    void onChannelChanged(Gfx::Color::Channel channel, f32 value);

private:
    void updatePreview();

private:
    Gfx::Color mColor{};

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

