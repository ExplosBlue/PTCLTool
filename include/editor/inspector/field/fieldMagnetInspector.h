#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class FieldMagnetInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit FieldMagnetInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    QWidget* mControlsWidget{nullptr};
    QDoubleSpinBox mMagnetPowerSpinBox{};
    VectorSpinBox<Math::Vector3f> mMagnetPosSpinBox{};
    QCheckBox mAxisXCheckBox{};
    QCheckBox mAxisYCheckBox{};
    QCheckBox mAxisZCheckBox{};
    QCheckBox mEnabledCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
