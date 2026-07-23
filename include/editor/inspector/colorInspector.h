#pragma once

#include "editor/components/animGraph.h"
#include "editor/components/colorGradientEditor.h"
#include "editor/components/enumComboBox.h"
#include "editor/components/rgbaColorWidget.h"
#include "editor/components/sizedSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QFormLayout>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ColorInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    enum class Behavior {
        Random = 0,
        Animation = 1
    };

public:
    explicit ColorInspector(QWidget* parent = nullptr);

private slots:
    void updateColorSection(ColorGradientEditor::HandleType handleType);
    void handleBehaviorChanged(PtclEditor::ColorInspector::Behavior behavior);

private:
    void populateProperties() final;
    void setupConnections();
    void updateAnimPoint(s32 pointIndex, const AnimGraph::GraphPoint& point);
    void updateVisibilityForCalcType(Ptcl::ColorCalcType type);

private:
    QFormLayout* mMainLayout{nullptr};
    EnumComboBox<Behavior> mColorBehavior{};

    RGBAColorWidget mSecondaryColorWidget{};

    RGBAColorWidget mRandomColorAWidget{};
    RGBAColorWidget mRandomColorBWidget{};
    RGBAColorWidget mRandomColorCWidget{};

    RGBAColorWidget mStartColorWidget{};
    RGBAColorWidget mMidColorWidget{};
    RGBAColorWidget mEndColorWidget{};

    RGBAColorWidget mPrimaryColorWidget{};

    ColorGradientEditor mColorSections{};
    SizedSpinBox<s32> mColorNumRepeatSpinBox{};
    EnumComboBox<Ptcl::ColorCalcType> mColorCalcTypeSpinBox{};

    AnimGraph mGraphA{};
    const Ptcl::Emitter* mLastEmitter = nullptr;
};


// ========================================================================== //


} // namespace PtclEditor
