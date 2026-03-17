#include "editor/inspector/velocityInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


VelocityInspector::VelocityInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);

    mFigureVelSpinbox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mInitVelSpinbox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mVelRandomSpinbox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mAirResistanceSpinbox.setRange(0.0f, 1.0f);

    mainLayout->addRow("Figure Velocity:", &mFigureVelSpinbox);
    mainLayout->addRow("Velocity Direction:", &mVelDirSpinbox);
    mainLayout->addRow("Initial Velocity:", &mInitVelSpinbox);
    mainLayout->addRow("Initial Velocity Random:", &mVelRandomSpinbox);
    mainLayout->addRow("Spread Vector:", &mSpreadVecSpinbox);
    mainLayout->addRow("Air Resistance:", &mAirResistanceSpinbox);

    setupConnections();
}

void VelocityInspector::setupConnections() {
    connect(&mFigureVelSpinbox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Figure Velocity",
            "SetFigureVelocity",
            &Ptcl::Emitter::figureVelocity,
            &Ptcl::Emitter::setFigureVelocity,
            static_cast<f32>(value)
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
            "Set Initial Velocity Random",
            "SetInitVelocityRand",
            &Ptcl::Emitter::initialVelocityRandom,
            &Ptcl::Emitter::setInitialVelocityRandom,
            static_cast<f32>(value)
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
            "Set Air Resistance",
            "SetAirResistance",
            &Ptcl::Emitter::airResistance,
            &Ptcl::Emitter::setAirResistance,
            static_cast<f32>(value)
        );
    });
}

void VelocityInspector::populateProperties() {
    QSignalBlocker b1(mFigureVelSpinbox);
    QSignalBlocker b2(mVelDirSpinbox);
    QSignalBlocker b3(mInitVelSpinbox);
    QSignalBlocker b4(mVelRandomSpinbox);
    QSignalBlocker b5(mSpreadVecSpinbox);
    QSignalBlocker b6(mAirResistanceSpinbox);

    mFigureVelSpinbox.setValue(mEmitter->figureVelocity());
    mVelDirSpinbox.setVector(mEmitter->velocityDirection());
    mInitVelSpinbox.setValue(mEmitter->initialVelocity());
    mVelRandomSpinbox.setValue(mEmitter->initialVelocityRandom());
    mSpreadVecSpinbox.setVector(mEmitter->spreadVector());
    mAirResistanceSpinbox.setValue(mEmitter->airResistance());

    update();
}


// ========================================================================== //


} // namespace PtclEditor
