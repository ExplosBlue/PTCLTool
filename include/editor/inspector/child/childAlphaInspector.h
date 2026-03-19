#pragma once

#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildAlphaInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit ChildAlphaInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    QDoubleSpinBox mAlphaSpinBox{};
    QDoubleSpinBox mAlphaTargetSpinBox{};
    QDoubleSpinBox mAlphaInitSpinBox{};
    QSpinBox mStartFrameSpinBox{};
    QSpinBox mBaseFrameSpinBox{};
    QCheckBox mInheritAlphaCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
