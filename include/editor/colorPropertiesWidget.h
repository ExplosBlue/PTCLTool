#pragma once

#include "editor/colorGradientEditor.h"
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


class ColorPropertiesWidget : public QWidget {
    Q_OBJECT
public:
    explicit ColorPropertiesWidget(QWidget* parent = nullptr);

    void setEmitter(Ptcl::Emitter* emitter);
    void populateWidgets();

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
    Ptcl::Emitter* mEmitterPtr;

    QComboBox mColorBehavior;
    std::array<QLabel, 3> mColorLabels;
    std::array<RGBAColorWidget, 3> mColorWidgets;
    ColorGradientEditor mColorSections;
    SizedSpinBox<s32> mColorNumRepeatSpinBox;
    QLabel mRepetitionCountLabel;

    bool mIsPopulating;
};


// ========================================================================== //
