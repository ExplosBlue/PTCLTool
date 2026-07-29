#include "editor/inspector/field/fieldSpinInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


static const std::array spinAxisOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::FieldSpinAxis>{ Ptcl::FieldSpinAxis::AxisX, "X-Axis", "Particles rotate in the YZ plane around the X axis." },
    EnumOption<Ptcl::FieldSpinAxis>{ Ptcl::FieldSpinAxis::AxisY, "Y-Axis", "Particles rotate in the ZX plane around the Y axis." },
    EnumOption<Ptcl::FieldSpinAxis>{ Ptcl::FieldSpinAxis::AxisZ, "Z-Axis", "Particles rotate in the XY plane around the Z axis." },
};


FieldSpinInspector::FieldSpinInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    addSectionHeader(mainLayout, "Spin", this);

    mEnabledCheckBox.setToolTip("Rotates particle positions around a given axis each frame.");
    mainLayout->addRow("Enable Spin:", &mEnabledCheckBox);

    addSectionHeader(mainLayout, "Rotation", this);

    mRotationSpinBox.setRange(-180.0, 180.0);
    mRotationSpinBox.setDecimals(1);
    mRotationSpinBox.setSingleStep(1.0);
    mRotationSpinBox.setSuffix("\u00B0");
    mRotationSpinBox.setToolTip("The angle in degrees that particles rotate by each frame.");
    mainLayout->addRow("Angle:", &mRotationSpinBox);

    mAxisSpinBox.setOptions(spinAxisOptions);
    mAxisSpinBox.setDescription("Defines which axis the particles rotate around.");
    mainLayout->addRow("Rotation Axis:", &mAxisSpinBox);

    setupConnections();
}


void FieldSpinInspector::setupConnections() {
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Field Spin",
            "ToggleFieldSpin",
            &Ptcl::Emitter::isFieldSpinEnabled,
            &Ptcl::Emitter::setFieldSpinEnabled,
            checked
        );
        setWidgetsEnabled(checked);
    });

    connect(&mRotationSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        const s32 rotate = Math::Util::deg2idx(static_cast<f32>(value));
        setEmitterProperty(
            "Set Spin Rotation",
            "SetFieldSpinRotation",
            &Ptcl::Emitter::fieldSpinRotate,
            &Ptcl::Emitter::setFieldSpinRotate,
            rotate
        );
    });

    connect(&mAxisSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto axis = mAxisSpinBox.currentEnum();
        setEmitterProperty(
            "Set Spin Axis",
            "SetFieldSpinAxis",
            &Ptcl::Emitter::fieldSpinAxis,
            &Ptcl::Emitter::setFieldSpinAxis,
            axis
        );
    });
}


void FieldSpinInspector::setWidgetsEnabled(bool enable) {
    mRotationSpinBox.setEnabled(enable);
    mAxisSpinBox.setEnabled(enable);
}


void FieldSpinInspector::populateProperties() {
    QSignalBlocker b1(mRotationSpinBox);
    QSignalBlocker b2(mAxisSpinBox);
    QSignalBlocker b3(mEnabledCheckBox);

    mRotationSpinBox.setValue(Math::Util::to180(Math::Util::idx2deg(mEmitter->fieldSpinRotate())));
    mAxisSpinBox.setCurrentEnum(mEmitter->fieldSpinAxis());

    const bool isEnabled = mEmitter->isFieldSpinEnabled();
    mEnabledCheckBox.setChecked(isEnabled);
    setWidgetsEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
