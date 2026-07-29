#include "editor/inspector/field/fieldCollisionInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


static const std::array collisionTypeOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::FieldCollisionType>{ Ptcl::FieldCollisionType::Die,    "Die",    "The particle is destroyed when it hits the collision plane." },
    EnumOption<Ptcl::FieldCollisionType>{ Ptcl::FieldCollisionType::Bounce, "Bounce", "The particle bounces off the collision plane." },
};


FieldCollisionInspector::FieldCollisionInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    addSectionHeader(mainLayout, "Collision", this);

    mEnabledCheckBox.setToolTip("Enables a horizontal collision plane. Particles below the plane are either killed or bounced.");
    mainLayout->addRow("Enable Collision:", &mEnabledCheckBox);

    addSectionHeader(mainLayout, "Plane", this);

    mIsWorldCheckBox.setText("World Space");
    mIsWorldCheckBox.setToolTip("When checked, the collision plane uses world coordinates instead of the emitter's local space.");
    mainLayout->addRow("Coordinate Space:", &mIsWorldCheckBox);

    mCoordSpinBox.setRange(-9999.999, 9999.999);
    mCoordSpinBox.setDecimals(3);
    mCoordSpinBox.setSingleStep(0.001);
    mCoordSpinBox.setToolTip("The Y position of the horizontal collision plane. Particles below this value will collide.");
    mainLayout->addRow("Plane Y:", &mCoordSpinBox);

    addSectionHeader(mainLayout, "Behavior", this);

    mCollisionTypeSpinBox.setOptions(collisionTypeOptions);
    mCollisionTypeSpinBox.setDescription("Controls what happens to particles when they hit the collision plane.");
    mainLayout->addRow("Collision Behavior:", &mCollisionTypeSpinBox);

    mCoefSpinBox.setRange(0.0, 1.0);
    mCoefSpinBox.setDecimals(2);
    mCoefSpinBox.setSingleStep(0.01);
    mCoefSpinBox.setToolTip("How bouncy the collision is. 1.0 = full bounce, 0.0 = no bounce.");
    mainLayout->addRow("Bounce Rate:", &mCoefSpinBox);

    setupConnections();
}


void FieldCollisionInspector::setupConnections() {
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Field Collision",
            "ToggleFieldCollision",
            &Ptcl::Emitter::isFieldCollisionEnabled,
            &Ptcl::Emitter::setFieldCollisionEnabled,
            checked
        );
        setWidgetsEnabled(checked);
    });

    connect(&mCollisionTypeSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mCollisionTypeSpinBox.currentEnum();
        setEmitterProperty(
            "Set Collision Type",
            "SetFieldCollisionType",
            &Ptcl::Emitter::fieldCollisionType,
            &Ptcl::Emitter::setFieldCollisionType,
            type
        );
        mCoefSpinBox.setEnabled(mEnabledCheckBox.isChecked() && type == Ptcl::FieldCollisionType::Bounce);
    });

    connect(&mIsWorldCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Collision World Coords",
            "ToggleFieldCollisionInWorld",
            &Ptcl::Emitter::fieldCollisionIsWorld,
            &Ptcl::Emitter::setFieldCollisionIsWorld,
            checked
        );
    });

    connect(&mCoefSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Collision Bounce Rate",
            "SetFieldCollisionCoef",
            &Ptcl::Emitter::fieldCollisionCoef,
            &Ptcl::Emitter::setFieldCollisionCoef,
            static_cast<f32>(value)
        );
    });

    connect(&mCoordSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Collision Plane Coord",
            "SetFieldCollisionCoord",
            &Ptcl::Emitter::fieldCollisionCoord,
            &Ptcl::Emitter::setFieldCollisionCoord,
            static_cast<f32>(value)
        );
    });
}


void FieldCollisionInspector::setWidgetsEnabled(bool enable) {
    mIsWorldCheckBox.setEnabled(enable);
    mCoordSpinBox.setEnabled(enable);
    mCollisionTypeSpinBox.setEnabled(enable);
    mCoefSpinBox.setEnabled(enable && mCollisionTypeSpinBox.currentEnum() == Ptcl::FieldCollisionType::Bounce);
}


void FieldCollisionInspector::populateProperties() {
    QSignalBlocker b1(mCollisionTypeSpinBox);
    QSignalBlocker b2(mIsWorldCheckBox);
    QSignalBlocker b3(mCoefSpinBox);
    QSignalBlocker b4(mCoordSpinBox);
    QSignalBlocker b5(mEnabledCheckBox);

    mCollisionTypeSpinBox.setCurrentEnum(mEmitter->fieldCollisionType());
    mIsWorldCheckBox.setChecked(mEmitter->fieldCollisionIsWorld());
    mCoefSpinBox.setValue(mEmitter->fieldCollisionCoef());
    mCoordSpinBox.setValue(mEmitter->fieldCollisionCoord());

    const bool isEnabled = mEmitter->isFieldCollisionEnabled();
    mEnabledCheckBox.setChecked(isEnabled);
    setWidgetsEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
