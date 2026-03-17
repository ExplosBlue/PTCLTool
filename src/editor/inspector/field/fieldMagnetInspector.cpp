#include "editor/inspector/field/fieldMagnetInspector.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


FieldMagnetInspector::FieldMagnetInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    // TODO: Better ranges?
    mMagnetPowerSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mMagnetPosSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mEnabledCheckBox.setText("Enabled");

    mControlsWidget = new QWidget(this);
    auto* controlsLayout = new QFormLayout(mControlsWidget);
    controlsLayout->addRow("Magnet Power:", &mMagnetPowerSpinBox);
    controlsLayout->addRow("Magnet Position:", &mMagnetPosSpinBox);
    controlsLayout->addRow("Target X-Axis:", &mAxisXCheckBox);
    controlsLayout->addRow("Target Y-Axis:", &mAxisYCheckBox);
    controlsLayout->addRow("Target Z-Axis:", &mAxisZCheckBox);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Magnetic Force:", &mEnabledCheckBox);
    mainLayout->addRow(mControlsWidget);

    setupConnections();
}

void FieldMagnetInspector::setupConnections() {
    // Is Enabled
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Field Magnet",
            "ToggleFieldMagnet",
            &Ptcl::Emitter::isFieldMagnetEnabled,
            &Ptcl::Emitter::setFieldMagnetEnabled,
            checked
        );
    });

    // Magnet Power
    connect(&mMagnetPowerSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Field Magnet Power",
            "SetFieldMagnetPower",
            &Ptcl::Emitter::fieldMagnetPower,
            &Ptcl::Emitter::setFieldMagnetPower,
            static_cast<f32>(value)
        );
    });

    // Magnet pos
    connect(&mMagnetPosSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto pos = mMagnetPosSpinBox.getVector();
        setEmitterProperty(
            "Set Field Magnet Pos",
            "SetFieldMagnetPos",
            &Ptcl::Emitter::fieldMagnetPos,
            &Ptcl::Emitter::setFieldMagnetPos,
            pos
        );
    });

    // Axis X
    connect(&mAxisXCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Field Magnet X-Target",
            "ToggleFieldMagnetTargetX",
            &Ptcl::Emitter::isFieldMagnetAxisTargetX,
            &Ptcl::Emitter::setFieldMagnetAxisTargetX,
            checked
        );
    });

    // Axis Y
    connect(&mAxisYCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Field Magnet Y-Target",
            "ToggleFieldMagnetTargetY",
            &Ptcl::Emitter::isFieldMagnetAxisTargetY,
            &Ptcl::Emitter::setFieldMagnetAxisTargetY,
            checked
        );
    });

    // Axis Z
    connect(&mAxisZCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Field Magnet Z-Target",
            "ToggleFieldMagnetTargetZ",
            &Ptcl::Emitter::isFieldMagnetAxisTargetZ,
            &Ptcl::Emitter::setFieldMagnetAxisTargetZ,
            checked
        );
    });
}

void FieldMagnetInspector::populateProperties() {
    QSignalBlocker b1(mMagnetPowerSpinBox);
    QSignalBlocker b2(mMagnetPosSpinBox);
    QSignalBlocker b3(mAxisXCheckBox);
    QSignalBlocker b4(mAxisYCheckBox);
    QSignalBlocker b5(mAxisZCheckBox);
    QSignalBlocker b6(mEnabledCheckBox);

    mMagnetPowerSpinBox.setValue(mEmitter->fieldMagnetPower());
    mMagnetPosSpinBox.setVector(mEmitter->fieldMagnetPos());
    mAxisXCheckBox.setChecked(mEmitter->isFieldMagnetAxisTargetX());
    mAxisYCheckBox.setChecked(mEmitter->isFieldMagnetAxisTargetY());
    mAxisZCheckBox.setChecked(mEmitter->isFieldMagnetAxisTargetZ());

    const bool isEnabled = mEmitter->isFieldMagnetEnabled();
    mEnabledCheckBox.setChecked(isEnabled);
    mControlsWidget->setEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
