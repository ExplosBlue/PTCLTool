#include "editor/gravityPropertiesWidget.h"

#include <QFormLayout>


// ========================================================================== //


GravityPropertiesWidget::GravityPropertiesWidget(QWidget* parent) :
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

void GravityPropertiesWidget::setProperties(const Ptcl::GravityProperties& properties) {
    QSignalBlocker b1(mIsDirectionalCheckBox);
    QSignalBlocker b2(mGravitySpinBox);

    mProps = properties;

    mIsDirectionalCheckBox.setChecked(mProps.isDirectional);
    mGravitySpinBox.setVector(mProps.gravity);
}


// ========================================================================== //
