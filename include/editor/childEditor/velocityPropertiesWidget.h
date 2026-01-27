#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/childEditor/childEditorWidget.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildEditorWidget::VelocityPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit VelocityPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::ChildData::VelocityProperties& properties, bool inheritVelocity);

signals:
    void propertiesUpdated(const Ptcl::ChildData::VelocityProperties& properties);
    void inheritVelUpdated(bool inherit);

private:
    void setupConnections();

private:
    Ptcl::ChildData::VelocityProperties mProps{};

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
