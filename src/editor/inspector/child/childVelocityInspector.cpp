#include "editor/inspector/child/childVelocityInspector.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


ChildVelocityInspector::ChildVelocityInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

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

void ChildVelocityInspector::setupConnections() {
    // Rand Velocity
    connect(&mRandVelSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto vel = mRandVelSpinBox.getVector();
        setEmitterProperty(
            "Set Child Random Velocity",
            "SetChildRandVel",
            &Ptcl::Emitter::childRandVelocity,
            &Ptcl::Emitter::setChildRandVelocity,
            vel
        );
    });

    // Gravity
    connect(&mGravitySpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto gravity = mGravitySpinBox.getVector();
        setEmitterProperty(
            "Set Child Gravity",
            "SetChildGravity",
            &Ptcl::Emitter::childGravity,
            &Ptcl::Emitter::setChildGravity,
            gravity
        );
    });

    // Inherit Velocity
    connect(&mInheritVelCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Child Inherit Velocity",
            "ToggleChildInheritVel",
            &Ptcl::Emitter::isChildInheritVelocity,
            &Ptcl::Emitter::setChildInheritVelocity,
            checked
        );
    });

    // Inherit Rate
    connect(&mVelInheritSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Child Velocity Inherit Rate",
            "SetChildVelInheritRate",
            &Ptcl::Emitter::childVelocityInheritRate,
            &Ptcl::Emitter::setChildVelocityInheritRate,
            static_cast<f32>(value)
        );
    });

    // Init Pos Rand
    connect(&mInitPosRandSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Child Initial Position Random",
            "SetChildInitPosRand",
            &Ptcl::Emitter::childInitalPositionRand,
            &Ptcl::Emitter::setChildInitialPositionRand,
            static_cast<f32>(value)
        );
    });

    // Figure Velocity
    connect(&mFigureVelSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Child Figure Velocity",
            "SetChildFigureVel",
            &Ptcl::Emitter::childFigureVelocity,
            &Ptcl::Emitter::setChildFigureVelocity,
            static_cast<f32>(value)
        );
    });

    // Air Resistance
    connect(&mAirResistSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Child Air Resistance",
            "SetChildAirResit",
            &Ptcl::Emitter::childAirResistance,
            &Ptcl::Emitter::setChildAirResistance,
            static_cast<f32>(value)
        );
    });
}

void ChildVelocityInspector::populateProperties() {
    QSignalBlocker b1(mRandVelSpinBox);
    QSignalBlocker b2(mGravitySpinBox);
    QSignalBlocker b3(mVelInheritSpinBox);
    QSignalBlocker b4(mInitPosRandSpinBox);
    QSignalBlocker b5(mFigureVelSpinBox);
    QSignalBlocker b6(mAirResistSpinBox);
    QSignalBlocker b7(mInheritVelCheckBox);

    mRandVelSpinBox.setVector(mEmitter->childRandVelocity());
    mGravitySpinBox.setVector(mEmitter->childGravity());
    mVelInheritSpinBox.setValue(mEmitter->childVelocityInheritRate());
    mVelInheritSpinBox.setEnabled(mEmitter->isChildInheritVelocity());
    mInitPosRandSpinBox.setValue(mEmitter->childInitalPositionRand());
    mFigureVelSpinBox.setValue(mEmitter->childFigureVelocity());
    mAirResistSpinBox.setValue(mEmitter->childAirResistance());
    mInheritVelCheckBox.setChecked(mEmitter->isChildInheritVelocity());
}


// ========================================================================== //


} // namespace PtclEditor
