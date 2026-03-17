#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QDoubleSpinBox>
#include <QVector3D>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class VelocityInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit VelocityInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    QDoubleSpinBox mFigureVelSpinbox{};
    VectorSpinBox<Math::Vector3f> mVelDirSpinbox{};
    QDoubleSpinBox mInitVelSpinbox{};
    QDoubleSpinBox mVelRandomSpinbox{};
    VectorSpinBox<Math::Vector3f> mSpreadVecSpinbox{};
    QDoubleSpinBox mAirResistanceSpinbox{};
};


// ========================================================================== //


} // namespace PtclEditor
