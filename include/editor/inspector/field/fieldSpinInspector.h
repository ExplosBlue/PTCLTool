#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class FieldSpinInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit FieldSpinInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    QWidget* mControlsWidget{nullptr};
    QDoubleSpinBox mSpinRotateSpinBox{};
    EnumComboBox<Ptcl::FieldSpinAxis> mSpinAxisSpinBox{};
    QCheckBox mEnabledCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
