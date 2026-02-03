#pragma once

#include "editor/components/colorGradientEditor.h"
#include "editor/components/enumComboBox.h"
#include "editor/components/rgbaColorWidget.h"
#include "editor/components/sizedSpinBox.h"
#include "editor/emitterWidget/emitterWidget.h"

#include "ptcl/ptclEmitter.h"

#include <QComboBox>
#include <QWidget>


namespace PtclEditor {


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


class EmitterWidget::ColorPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit ColorPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::Emitter::ColorProperties& properties);
    void populateWidgets();

signals:
    void propertiesUpdated(const Ptcl::Emitter::ColorProperties& properties);

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
    Ptcl::Emitter::ColorProperties mProps{};

    QComboBox mColorBehavior{};
    std::array<QLabel, 3> mColorLabels;
    std::array<RGBAColorWidget, 3> mColor0Widgets;
    ColorGradientEditor mColorSections{};
    SizedSpinBox<s32> mColorNumRepeatSpinBox{};
    QLabel mRepetitionCountLabel{};
    EnumComboBox<Ptcl::ColorCalcType> mColorCalcTypeSpinBox{};

    RGBAColorWidget mColor1Widget{};
};


// ========================================================================== //


} // namespace PtclEditor
