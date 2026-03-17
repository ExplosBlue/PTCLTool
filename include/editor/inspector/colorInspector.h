#pragma once

#include "editor/components/colorGradientEditor.h"
#include "editor/components/enumComboBox.h"
#include "editor/components/rgbaColorWidget.h"
#include "editor/components/sizedSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QComboBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


enum class Behavior { Constant = 0, Random = 1, Animation = 2 };

static Behavior behaviorFromIndex(s32 index) {
    switch (index) {
    case 1: return Behavior::Random;
    case 2: return Behavior::Animation;
    default: return Behavior::Constant;
    }
}

static s32 behaviorToIndex(Behavior behavior) {
    return static_cast<s32>(behavior);
}


// ========================================================================== //


class ColorInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit ColorInspector(QWidget* parent = nullptr);

private slots:
    void updateColorSection(ColorGradientEditor::HandleType handleType);
    void handleBehaviorChanged(s32 index);

private:
    void populateProperties() final;
    void setupConnections();

private:
    QComboBox mColorBehavior{};

    RGBAColorWidget mPrimaryColorWidget{};
    QWidget* mPrimaryColorUi{nullptr};

    RGBAColorWidget mRandomColorAWidget{};
    RGBAColorWidget mRandomColorBWidget{};
    RGBAColorWidget mRandomColorCWidget{};
    QWidget* mRandomColorUi{nullptr};

    RGBAColorWidget mStartColorWidget{};
    RGBAColorWidget mMidColorWidget{};
    RGBAColorWidget mEndColorWidget{};
    QWidget* mAnimColorUi{nullptr};

    RGBAColorWidget mSecondaryColorWidget{};

    ColorGradientEditor mColorSections{};
    SizedSpinBox<s32> mColorNumRepeatSpinBox{};
    QLabel mRepetitionCountLabel{};
    EnumComboBox<Ptcl::ColorCalcType> mColorCalcTypeSpinBox{};
};


// ========================================================================== //


} // namespace PtclEditor
