#pragma once

#include "editor/colorGradientEditor.h"
#include "editor/enumComboBox.h"
#include "editor/rgbaColorWidget.h"
#include "editor/sizedSpinBox.h"
#include "ptcl/ptclEmitter.h"

#include <QComboBox>
#include <QWidget>


// ========================================================================== //


enum class Behavior { Constant = 0, Random = 1, Animation = 2 };

static Behavior behaviorFromIndex(int index) {
    switch (index) {
    case 1: return Behavior::Random;
    case 2: return Behavior::Animation;
    default: return Behavior::Constant;
    }
}

static int behaviorToIndex(Behavior behavior) {
    return static_cast<int>(behavior);
}


// ========================================================================== //


class ColorPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit ColorPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::ColorProperties& properties);
    void populateWidgets();

signals:
    void propertiesUpdated(const Ptcl::ColorProperties& properties);

private slots:
    void handleColorChanged();
    void updateColorSection(ColorGradientEditor::HandleType handleType);
    void handleBehaviorChanged(int index);

private:
    void updateUiFromFlags();
    void applyBehaviorToUI(Behavior behavior);
    void showColorWidgets(std::array<bool, 3> visibility);
    void setColorLabels(const std::array<QString, 3>& labels);

private:
    Ptcl::ColorProperties mProps{};

    QComboBox mColorBehavior{};
    std::array<QLabel, 3> mColorLabels;
    std::array<RGBAColorWidget, 3> mColorWidgets;
    ColorGradientEditor mColorSections{};
    SizedSpinBox<s32> mColorNumRepeatSpinBox{};
    QLabel mRepetitionCountLabel{};
    EnumComboBox<Ptcl::ColorCalcType> mColorCalcTypeSpinBox{};

    RGBAColorWidget mColor0Widget{};
};


// ========================================================================== //
