#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/emitterWidget/emitterWidget.h"

#include "ptcl/ptclEmitter.h"

#include <QDoubleSpinBox>
#include <QVector3D>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterWidget::VelocityPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit VelocityPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::Emitter::VelocityProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::Emitter::VelocityProperties& properties);

private:
    Ptcl::Emitter::VelocityProperties mProps{};

    QDoubleSpinBox mFigureVelSpinbox{};
    VectorSpinBox<Math::Vector3f> mVelDirSpinbox{};
    QDoubleSpinBox mInitVelSpinbox{};
    QDoubleSpinBox mVelRandomSpinbox{};
    VectorSpinBox<Math::Vector3f> mSpreadVecSpinbox{};
    QDoubleSpinBox mAirResistanceSpinbox{};
};


// ========================================================================== //


} // namespace PtclEditor
