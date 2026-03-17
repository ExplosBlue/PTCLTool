#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class FieldConvergenceInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit FieldConvergenceInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    QWidget* mControlsWidget{nullptr};
    EnumComboBox<Ptcl::FieldConvergenceType> mTypeSpinBox{};
    VectorSpinBox<Math::Vector3f> mPosSpinBox{};
    QCheckBox mEnabledCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
