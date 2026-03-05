#include "editor/emitterWidget/gravityPropertiesWidget.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


GravityPropertiesWidget::GravityPropertiesWidget(QWidget* parent) :
    EmitterWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);

    mIsDirectionalCheckBox.setText("Apply gravity in world coords");

    mainLayout->addRow("Coordinates:", &mIsDirectionalCheckBox);
    mainLayout->addRow("Gravity Direction:", &mGravitySpinBox);

    setupConnections();
}

void GravityPropertiesWidget::setupConnections() {
    // Is Directional
    connect(&mIsDirectionalCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Set Gravity Directional",
            "GravDirectional",
            &Ptcl::Emitter::isDirectional,
            &Ptcl::Emitter::setDirectional,
            checked
        );
    });

    // Gravity
    connect(&mGravitySpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto gravity = mGravitySpinBox.getVector();
        setEmitterProperty(
            "Set Gravity Direction",
            "GravDir",
            &Ptcl::Emitter::gravity,
            &Ptcl::Emitter::setGravity,
            gravity
        );
    });
}

void GravityPropertiesWidget::populateProperties() {
    QSignalBlocker b1(mIsDirectionalCheckBox);
    QSignalBlocker b2(mGravitySpinBox);

    mIsDirectionalCheckBox.setChecked(mEmitter->isDirectional());
    mGravitySpinBox.setVector(mEmitter->gravity());
}


// ========================================================================== //


} // namespace PtclEditor
