#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
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
    void setWidgetsEnabled(bool enable);

private:
    EnumComboBox<Ptcl::FieldConvergenceType> mTypeSpinBox{};
    VectorSpinBox<Math::Vector2f> mPosSpinBox{};
    QDoubleSpinBox mSpeedSpinBox{};
    QCheckBox mEnabledCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
