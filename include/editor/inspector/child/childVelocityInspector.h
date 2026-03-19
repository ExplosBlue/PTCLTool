#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildVelocityInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit ChildVelocityInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    VectorSpinBox<Math::Vector3f> mRandVelSpinBox{};
    VectorSpinBox<Math::Vector3f> mGravitySpinBox{};
    QDoubleSpinBox mVelInheritSpinBox{};
    QDoubleSpinBox mInitPosRandSpinBox{};
    QDoubleSpinBox mFigureVelSpinBox{};
    QDoubleSpinBox mAirResistSpinBox{};
    QCheckBox mInheritVelCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
