#include "editor/inspector/field/fieldCollisionInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


FieldCollisionInspector::FieldCollisionInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    // TODO: Better ranges?
    mIsWorldCheckBox.setText("Collision in world coordinates");
    mCoordSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mCoefSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mEnabledCheckBox.setText("Enabled");

    mControlsWidget = new QWidget(this);
    auto* controlsLayout = new QFormLayout(mControlsWidget);
    controlsLayout->addRow("Collision Behavior:", &mCollisionTypeSpinBox);
    controlsLayout->addRow("Coordinate System:", &mIsWorldCheckBox);
    controlsLayout->addRow("Plane Y-Coord:", &mCoordSpinBox);
    controlsLayout->addRow("Bounce Rate:", &mCoefSpinBox);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Collision:", &mEnabledCheckBox);
    mainLayout->addRow(mControlsWidget);

    setupConnections();
}

void FieldCollisionInspector::setupConnections() {
    // Is Enabled
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Field Collision",
            "ToggleFieldCollision",
            &Ptcl::Emitter::isFieldCollisionEnabled,
            &Ptcl::Emitter::setFieldCollisionEnabled,
            checked
        );
    });

    // CollisionType
    connect(&mCollisionTypeSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mCollisionTypeSpinBox.currentEnum();
        setEmitterProperty(
            "Set Field Collision Type",
            "SetFieldCollisionType",
            &Ptcl::Emitter::fieldCollisionType,
            &Ptcl::Emitter::setFieldCollisionType,
            type
        );
    });

    // Is World
    connect(&mIsWorldCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Field Collision WorldCoords",
            "ToggleFieldCollisionInWorld",
            &Ptcl::Emitter::fieldCollisionIsWorld,
            &Ptcl::Emitter::setFieldCollisionIsWorld,
            checked
        );
    });

    // Coef
    connect(&mCoefSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {        
        setEmitterProperty(
            "Set Field Collision Bounce Rate",
            "SetFieldCollisionCoef",
            &Ptcl::Emitter::fieldCollisionCoef,
            &Ptcl::Emitter::setFieldCollisionCoef,
            static_cast<f32>(value)
        );
    });

    // Coord
    connect(&mCoordSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Field Collision Plane Coord",
            "SetFieldCollisionCoord",
            &Ptcl::Emitter::fieldCollisionCoord,
            &Ptcl::Emitter::setFieldCollisionCoord,
            static_cast<f32>(value)
        );
    });
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
    mControlsWidget->setEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
