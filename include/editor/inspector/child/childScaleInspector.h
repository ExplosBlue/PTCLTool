#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildScaleInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit ChildScaleInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    VectorSpinBox<Math::Vector2f> mScaleSpinBox{};
    VectorSpinBox<Math::Vector2f> mScaleTargetSpinBox{};
    QDoubleSpinBox mInheritRateSpinBox{};
    QSpinBox mStartFrameSpinBox{};
    QCheckBox mInheritScaleCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
