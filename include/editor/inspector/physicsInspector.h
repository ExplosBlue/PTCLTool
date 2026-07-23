#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class PhysicsInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit PhysicsInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    // Gravity
    QCheckBox mIsDirectionalCheckBox{};
    VectorSpinBox<Math::Vector3f> mGravitySpinBox{};

    // Velocity
    QDoubleSpinBox mFigureVelSpinbox{};
    VectorSpinBox<Math::Vector3f> mVelDirSpinbox{};
    QDoubleSpinBox mInitVelSpinbox{};
    QDoubleSpinBox mVelRandomSpinbox{};
    VectorSpinBox<Math::Vector3f> mSpreadVecSpinbox{};
    QDoubleSpinBox mAirResistanceSpinbox{};
};


// ========================================================================== //


} // namespace PtclEditor
