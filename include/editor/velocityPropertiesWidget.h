#pragma once

#include "editor/vectorSpinBox.h"
#include "ptcl/ptclEmitter.h"


#include <QDoubleSpinBox>
#include <QVector3D>
#include <QWidget>

// ========================================================================== //


class VelocityPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit VelocityPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::VelocityProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::VelocityProperties& properties);

private:
    Ptcl::VelocityProperties mProps{};

    QDoubleSpinBox mFigureVelSpinbox{};
    VectorSpinBox<Math::Vector3f> mVelDirSpinbox{};
    QDoubleSpinBox mInitVelSpinbox{};
    QDoubleSpinBox mVelRandomSpinbox{};
    VectorSpinBox<Math::Vector3f> mSpreadVecSpinbox{};
};


// ========================================================================== //
