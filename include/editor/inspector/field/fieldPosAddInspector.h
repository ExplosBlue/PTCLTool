#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class FieldPosAddInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit FieldPosAddInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    QWidget* mControlsWidget{nullptr};
    VectorSpinBox<Math::Vector3f> mPosSpinBox{};
    QCheckBox mEnabledCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
