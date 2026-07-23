#include "editor/inspector/physicsInspector.h"

#include <QFormLayout>
#include <QVBoxLayout>


namespace PtclEditor {


// ========================================================================== //


PhysicsInspector::PhysicsInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    // Gravity
    addSectionHeader(mainLayout, "Gravity", this);

    mGravitySpinBox.setDecimals(2);
    mGravitySpinBox.setToolTip("The direction and strength of the gravitational pull on particles.");
    mainLayout->addRow("Gravity Acceleration:", &mGravitySpinBox);

    mIsDirectionalCheckBox.setText("Directional Gravity");
    mIsDirectionalCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mIsDirectionalCheckBox.setToolTip("Applies gravity relative to the emitter's rotation instead of world space.");
    mainLayout->addRow("Directional:", &mIsDirectionalCheckBox);

    // Velocity
    addSectionHeader(mainLayout, "Velocity", this);

    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    mInitVelSpinbox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mInitVelSpinbox.setToolTip("The base speed at which particles are emitted.");
    mainLayout->addRow("Initial Velocity:", &mInitVelSpinbox);

    mVelRandomSpinbox.setRange(0.0, 100.0);
    mVelRandomSpinbox.setSuffix("%");
    mVelRandomSpinbox.setSingleStep(1.0);
    mVelRandomSpinbox.setDecimals(0);
    mVelRandomSpinbox.setToolTip("Random variation applied to each particle's initial speed.");
    mainLayout->addRow("Initial Velocity Variation:", &mVelRandomSpinbox);

    mVelDirSpinbox.setDecimals(2);
    mVelDirSpinbox.setToolTip("The direction particles travel when emitted.");
    mainLayout->addRow("Velocity Direction:", &mVelDirSpinbox);

    mSpreadVecSpinbox.setDecimals(2);
    mSpreadVecSpinbox.setToolTip("Random offset added to each particle's initial velocity, per axis.");
    mainLayout->addRow("Spread Vector:", &mSpreadVecSpinbox);

    mFigureVelSpinbox.setRange(0.0, 100.0);
    mFigureVelSpinbox.setSuffix("%");
    mFigureVelSpinbox.setSingleStep(1.0);
    mFigureVelSpinbox.setDecimals(0);
    mFigureVelSpinbox.setToolTip("How much of the emitter's own movement is inherited by particles.");
    mainLayout->addRow("Emitter Velocity Inheritance:", &mFigureVelSpinbox);

    mAirResistanceSpinbox.setRange(0.0, 100.0);
    mAirResistanceSpinbox.setSuffix("%");
    mAirResistanceSpinbox.setSingleStep(1.0);
    mAirResistanceSpinbox.setDecimals(0);
    mAirResistanceSpinbox.setToolTip("How much particles slow down over time. 0% = no slowdown, 100% = instant stop.");
    mainLayout->addRow("Velocity Damping:", &mAirResistanceSpinbox);

    setupConnections();
}

void PhysicsInspector::setupConnections() {
    // Gravity
    connect(&mIsDirectionalCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Set Gravity Directional",
            "GravDirectional",
            &Ptcl::Emitter::isDirectional,
            &Ptcl::Emitter::setDirectional,
            checked
        );
    });

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

    // Velocity
    connect(&mFigureVelSpinbox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Emitter Velocity Inheritance",
            "SetFigureVelocity",
            &Ptcl::Emitter::figureVelocity,
            &Ptcl::Emitter::setFigureVelocity,
            static_cast<f32>(value / 100.0)
        );
    });

    connect(&mVelDirSpinbox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto direction = mVelDirSpinbox.getVector();

        setEmitterProperty(
            "Set Velocity Direction",
            "SetVelocityDir",
            &Ptcl::Emitter::velocityDirection,
            &Ptcl::Emitter::setVelocityDirection,
            direction
        );
    });

    connect(&mInitVelSpinbox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Initial Velocity",
            "SetInitialVelocity",
            &Ptcl::Emitter::initialVelocity,
            &Ptcl::Emitter::setInitialVelocity,
            static_cast<f32>(value)
        );
    });

    connect(&mVelRandomSpinbox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Initial Velocity Variation",
            "SetInitVelocityRand",
            &Ptcl::Emitter::initialVelocityRandom,
            &Ptcl::Emitter::setInitialVelocityRandom,
            static_cast<f32>(value / 100.0)
        );
    });

    connect(&mSpreadVecSpinbox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto spread = mSpreadVecSpinbox.getVector();

        setEmitterProperty(
            "Set Spread Vector",
            "SetSpreadVector",
            &Ptcl::Emitter::spreadVector,
            &Ptcl::Emitter::setSpreadVector,
            spread
        );
    });

    connect(&mAirResistanceSpinbox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Velocity Damping",
            "SetAirResistance",
            &Ptcl::Emitter::airResistance,
            &Ptcl::Emitter::setAirResistance,
            static_cast<f32>(1.0 - value / 100.0)
        );
    });
}

void PhysicsInspector::populateProperties() {
    // Gravity
    QSignalBlocker b1(mIsDirectionalCheckBox);
    QSignalBlocker b2(mGravitySpinBox);

    mIsDirectionalCheckBox.setChecked(mEmitter->isDirectional());
    mGravitySpinBox.setVector(mEmitter->gravity());

    // Velocity
    QSignalBlocker b3(mFigureVelSpinbox);
    QSignalBlocker b4(mVelDirSpinbox);
    QSignalBlocker b5(mInitVelSpinbox);
    QSignalBlocker b6(mVelRandomSpinbox);
    QSignalBlocker b7(mSpreadVecSpinbox);
    QSignalBlocker b8(mAirResistanceSpinbox);

    mFigureVelSpinbox.setValue(mEmitter->figureVelocity() * 100.0);
    mVelDirSpinbox.setVector(mEmitter->velocityDirection());
    mInitVelSpinbox.setValue(mEmitter->initialVelocity());
    mVelRandomSpinbox.setValue(mEmitter->initialVelocityRandom() * 100.0);
    mSpreadVecSpinbox.setVector(mEmitter->spreadVector());
    mAirResistanceSpinbox.setValue((1.0 - mEmitter->airResistance()) * 100.0);

    update();
}


// ========================================================================== //


} // namespace PtclEditor
