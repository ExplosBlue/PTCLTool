#include "editor/emitterWidget/gravityPropertiesWidget.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


EmitterWidget::GravityPropertiesWidget::GravityPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QFormLayout(this);

    mIsDirectionalCheckBox.setText("Apply gravity in world coords");

    mainLayout->addRow("Coordinates:", &mIsDirectionalCheckBox);
    mainLayout->addRow("Gravity Direction:", &mGravitySpinBox);

    connect(&mIsDirectionalCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        mProps.isDirectional = checked;
        emit propertiesUpdated(mProps);
    });

    connect(&mGravitySpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mProps.gravity = mGravitySpinBox.getVector();
        emit propertiesUpdated(mProps);
    });
}

void EmitterWidget::GravityPropertiesWidget::setProperties(const Ptcl::Emitter::GravityProperties& properties) {
    QSignalBlocker b1(mIsDirectionalCheckBox);
    QSignalBlocker b2(mGravitySpinBox);

    mProps = properties;

    mIsDirectionalCheckBox.setChecked(mProps.isDirectional);
    mGravitySpinBox.setVector(mProps.gravity);
}


// ========================================================================== //


} // namespace PtclEditor
