#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class FieldCollisionInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit FieldCollisionInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    QWidget* mControlsWidget{nullptr};
    EnumComboBox<Ptcl::FieldCollisionType> mCollisionTypeSpinBox{};
    QCheckBox mIsWorldCheckBox{};
    QDoubleSpinBox mCoefSpinBox{};
    QDoubleSpinBox mCoordSpinBox{};
    QCheckBox mEnabledCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
