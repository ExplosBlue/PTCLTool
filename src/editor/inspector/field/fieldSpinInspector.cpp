#include "editor/inspector/field/fieldSpinInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


FieldSpinInspector::FieldSpinInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    mSpinRotateSpinBox.setRange(-180.0f, 180.0f);
    mEnabledCheckBox.setText("Enabled");

    mControlsWidget = new QWidget(this);
    auto* controlsLayout = new QFormLayout(mControlsWidget);
    controlsLayout->addRow("Spin Rotation:", &mSpinRotateSpinBox);
    controlsLayout->addRow("Spin Axis:", &mSpinAxisSpinBox);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Spin:", &mEnabledCheckBox);
    mainLayout->addRow(mControlsWidget);

    setupConnections();
}

void FieldSpinInspector::setupConnections() {
    // Is Enabled
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Field Spin",
            "ToggleFieldSpin",
            &Ptcl::Emitter::isFieldSpinEnabled,
            &Ptcl::Emitter::setFieldSpinEnabled,
            checked
        );
    });

    // Spin Rotate
    connect(&mSpinRotateSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        const f32 rotate = Math::Util::deg2idx(static_cast<f32>(value));
        setEmitterProperty(
            "Set Field Spin Rotation",
            "SetFieldSpinRotation",
            &Ptcl::Emitter::fieldSpinRotate,
            &Ptcl::Emitter::setFieldSpinRotate,
            rotate
        );
    });

    // Spin Axis
    connect(&mSpinAxisSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto axis = mSpinAxisSpinBox.currentEnum();
        setEmitterProperty(
            "Set Field Spin Axis",
            "SetFieldSpinAxis",
            &Ptcl::Emitter::fieldSpinAxis,
            &Ptcl::Emitter::setFieldSpinAxis,
            axis
        );
    });
}

void FieldSpinInspector::populateProperties() {
    QSignalBlocker b1(mSpinRotateSpinBox);
    QSignalBlocker b2(mSpinAxisSpinBox);
    QSignalBlocker b3(mEnabledCheckBox);

    mSpinRotateSpinBox.setValue(Math::Util::to180(Math::Util::idx2deg(mEmitter->fieldSpinRotate())));
    mSpinAxisSpinBox.setCurrentEnum(mEmitter->fieldSpinAxis());

    const bool isEnabled = mEmitter->isFieldSpinEnabled();
    mEnabledCheckBox.setChecked(isEnabled);
    mControlsWidget->setEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
