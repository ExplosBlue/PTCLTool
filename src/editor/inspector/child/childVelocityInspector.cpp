#include "editor/inspector/child/childVelocityInspector.h"

#include <QFormLayout>
#include <QVBoxLayout>

namespace PtclEditor {


// ========================================================================== //


ChildVelocityInspector::ChildVelocityInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    mGravitySpinBox.setDecimals(2);
    mGravitySpinBox.setToolTip("The direction and strength of the gravitational pull on child particles.");

    mRandVelSpinBox.setDecimals(2);
    mRandVelSpinBox.setToolTip("Random offset added to each child particle's initial velocity, per axis.");

    mFigureVelSpinBox.setRange(0.0, 100.0);
    mFigureVelSpinBox.setSuffix("%");
    mFigureVelSpinBox.setSingleStep(1.0);
    mFigureVelSpinBox.setDecimals(0);
    mFigureVelSpinBox.setToolTip("How much of the emitter's own movement is inherited by child particles.");

    mAirResistSpinBox.setRange(0.0, 100.0);
    mAirResistSpinBox.setSuffix("%");
    mAirResistSpinBox.setSingleStep(1.0);
    mAirResistSpinBox.setDecimals(0);
    mAirResistSpinBox.setToolTip("How much child particles slow down over time. 0% = no slowdown, 100% = instant stop.");

    mInheritVelCheckBox.setText("Inherit Parent Velocity");
    mInheritVelCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mInheritVelCheckBox.setToolTip("When enabled, child particles inherit the parent emitter's velocity.");

    mVelInheritSpinBox.setRange(0.0, 100.0);
    mVelInheritSpinBox.setSuffix("%");
    mVelInheritSpinBox.setSingleStep(1.0);
    mVelInheritSpinBox.setDecimals(0);
    mVelInheritSpinBox.setToolTip("How much of the parent emitter's velocity is inherited by child particles.");

    auto* mainLayout = new QVBoxLayout(this);

    // Gravity
    addSectionHeader(mainLayout, "Gravity", this);

    auto* gravityLayout = new QFormLayout;
    gravityLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    gravityLayout->addRow("Gravity Acceleration:", &mGravitySpinBox);
    mainLayout->addLayout(gravityLayout);

    // Velocity
    addSectionHeader(mainLayout, "Velocity", this);

    auto* velocityLayout = new QFormLayout;
    velocityLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    velocityLayout->addRow("Spread Vector:", &mRandVelSpinBox);
    velocityLayout->addRow("Emitter Velocity Inheritance:", &mFigureVelSpinBox);
    velocityLayout->addRow("Velocity Damping:", &mAirResistSpinBox);
    velocityLayout->addRow("Inherit Parent Velocity:", &mInheritVelCheckBox);
    velocityLayout->addRow("Velocity Inherit Rate:", &mVelInheritSpinBox);
    mainLayout->addLayout(velocityLayout);

    mainLayout->addStretch();

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
            static_cast<f32>(value / 100.0)
        );
    });

    // Figure Velocity
    connect(&mFigureVelSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Child Emitter Velocity Inheritance",
            "SetChildFigureVel",
            &Ptcl::Emitter::childFigureVelocity,
            &Ptcl::Emitter::setChildFigureVelocity,
            static_cast<f32>(value / 100.0)
        );
    });

    // Air Resistance
    connect(&mAirResistSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Child Velocity Damping",
            "SetChildAirResit",
            &Ptcl::Emitter::childAirResistance,
            &Ptcl::Emitter::setChildAirResistance,
            static_cast<f32>(1.0 - value / 100.0)
        );
    });
}

void ChildVelocityInspector::populateProperties() {
    QSignalBlocker b1(mRandVelSpinBox);
    QSignalBlocker b2(mGravitySpinBox);
    QSignalBlocker b3(mVelInheritSpinBox);
    QSignalBlocker b4(mFigureVelSpinBox);
    QSignalBlocker b5(mAirResistSpinBox);
    QSignalBlocker b6(mInheritVelCheckBox);

    mRandVelSpinBox.setVector(mEmitter->childRandVelocity());
    mGravitySpinBox.setVector(mEmitter->childGravity());
    mVelInheritSpinBox.setValue(mEmitter->childVelocityInheritRate() * 100.0);
    mVelInheritSpinBox.setEnabled(mEmitter->isChildInheritVelocity());
    mFigureVelSpinBox.setValue(mEmitter->childFigureVelocity() * 100.0);
    mAirResistSpinBox.setValue((1.0 - mEmitter->childAirResistance()) * 100.0);
    mInheritVelCheckBox.setChecked(mEmitter->isChildInheritVelocity());
}


// ========================================================================== //


} // namespace PtclEditor
