#include "editor/childEditor/velocityPropertiesWidget.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


ChildEditorWidget::VelocityPropertiesWidget::VelocityPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    // TODO: Determine better ranges?
    mRandVelSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mGravitySpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mVelInheritSpinBox.setRange(0.0f, 1.0f);
    mInitPosRandSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mFigureVelSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mAirResistSpinBox.setRange(0.0f, 1.0f);

    auto* mainLayout = new QFormLayout(this);

    mainLayout->addRow("Random Velocity:", &mRandVelSpinBox);
    mainLayout->addRow("Gravity:", &mGravitySpinBox);
    mainLayout->addRow("Inherit Parent Velocity:", &mInheritVelCheckBox);
    mainLayout->addRow("Velocity Inherit Rate:", &mVelInheritSpinBox);
    mainLayout->addRow("Inital Position Random:", &mInitPosRandSpinBox);
    mainLayout->addRow("Figure Velocity:", &mFigureVelSpinBox);
    mainLayout->addRow("Air Resistance:", &mAirResistSpinBox);

    setupConnections();
}

void ChildEditorWidget::VelocityPropertiesWidget::setupConnections() {
    // Rand Velocity
    connect(&mRandVelSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mProps.randVel = mRandVelSpinBox.getVector();
        emit propertiesUpdated(mProps);
    });

    // Gravity
    connect(&mGravitySpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mProps.gravity = mGravitySpinBox.getVector();
        emit propertiesUpdated(mProps);
    });

    // Inherit Rate
    connect(&mInheritVelCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        QSignalBlocker b1(mVelInheritSpinBox);
        mVelInheritSpinBox.setEnabled(checked);
        emit inheritVelUpdated(checked);
    });

    // Inherit Rate
    connect(&mVelInheritSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mProps.velInheritRate = static_cast<f32>(value);
        emit propertiesUpdated(mProps);
    });

    // Init Pos Rand
    connect(&mInitPosRandSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mProps.initPosRand = static_cast<f32>(value);
        emit propertiesUpdated(mProps);
    });

    // Figure Velocity
    connect(&mFigureVelSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mProps.figurVel = static_cast<f32>(value);
        emit propertiesUpdated(mProps);
    });

    // Air Resistance
    connect(&mAirResistSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mProps.airResist = static_cast<f32>(value);
        emit propertiesUpdated(mProps);
    });
}

void ChildEditorWidget::VelocityPropertiesWidget::setProperties(const Ptcl::ChildData::VelocityProperties& properties, bool inheritVelocity) {
    QSignalBlocker b1(mRandVelSpinBox);
    QSignalBlocker b2(mGravitySpinBox);
    QSignalBlocker b3(mVelInheritSpinBox);
    QSignalBlocker b4(mInitPosRandSpinBox);
    QSignalBlocker b5(mFigureVelSpinBox);
    QSignalBlocker b6(mAirResistSpinBox);
    QSignalBlocker b7(mInheritVelCheckBox);

    mProps = properties;

    mRandVelSpinBox.setVector(mProps.randVel);
    mGravitySpinBox.setVector(mProps.gravity);
    mVelInheritSpinBox.setValue(mProps.velInheritRate);
    mVelInheritSpinBox.setEnabled(inheritVelocity);
    mInitPosRandSpinBox.setValue(mProps.initPosRand);
    mFigureVelSpinBox.setValue(mProps.figurVel);
    mAirResistSpinBox.setValue(mProps.airResist);
    mInheritVelCheckBox.setChecked(inheritVelocity);
}


// ========================================================================== //


} // namespace PtclEditor
