#include "editor/inspector/field/fieldMagnetInspector.h"

#include <QFormLayout>
#include <QHBoxLayout>


namespace PtclEditor {


// ========================================================================== //


FieldMagnetInspector::FieldMagnetInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    addSectionHeader(mainLayout, "Magnet", this);

    mEnabledCheckBox.setToolTip("Attracts particles toward a target point by adjusting their velocity.");
    mainLayout->addRow("Enable Magnetic Force:", &mEnabledCheckBox);

    addSectionHeader(mainLayout, "Target", this);

    mMagnetPowerSpinBox.setRange(0.0, 10.0);
    mMagnetPowerSpinBox.setDecimals(3);
    mMagnetPowerSpinBox.setSingleStep(0.1);
    mMagnetPowerSpinBox.setToolTip("How strongly particles are pulled toward the target. Higher values = faster attraction.");
    mainLayout->addRow("Magnet Power:", &mMagnetPowerSpinBox);

    auto* axisWidget = new QWidget(this);
    auto* axisLayout = new QHBoxLayout(axisWidget);
    axisLayout->setContentsMargins(0, 0, 0, 0);

    mAxisXCheckBox.setText("X");
    mAxisXCheckBox.setToolTip("When checked, the magnetic force affects movement along the X axis.");
    axisLayout->addWidget(&mAxisXCheckBox);

    mAxisYCheckBox.setText("Y");
    mAxisYCheckBox.setToolTip("When checked, the magnetic force affects movement along the Y axis.");
    axisLayout->addWidget(&mAxisYCheckBox);

    mAxisZCheckBox.setText("Z");
    mAxisZCheckBox.setToolTip("When checked, the magnetic force affects movement along the Z axis.");
    axisLayout->addWidget(&mAxisZCheckBox);

    axisLayout->addStretch();

    mainLayout->addRow("Target Axes:", axisWidget);

    mMagnetPosSpinBox.setRange(-9999.0f, 9999.0f);
    mMagnetPosSpinBox.setToolTip("The point in space that particles are attracted toward.");
    mMagnetPosSpinBox.setAxisToolTip(VectorSpinBoxBase::Axis::X, "Target position on the X axis.");
    mMagnetPosSpinBox.setAxisToolTip(VectorSpinBoxBase::Axis::Y, "Target position on the Y axis.");
    mMagnetPosSpinBox.setAxisToolTip(VectorSpinBoxBase::Axis::Z, "Target position on the Z axis.");
    mainLayout->addRow("Target Position:", &mMagnetPosSpinBox);

    setupConnections();
}


void FieldMagnetInspector::setupConnections() {
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Field Magnet",
            "ToggleFieldMagnet",
            &Ptcl::Emitter::isFieldMagnetEnabled,
            &Ptcl::Emitter::setFieldMagnetEnabled,
            checked
        );
        setWidgetsEnabled(checked);
    });

    connect(&mMagnetPowerSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Magnet Power",
            "SetFieldMagnetPower",
            &Ptcl::Emitter::fieldMagnetPower,
            &Ptcl::Emitter::setFieldMagnetPower,
            static_cast<f32>(value)
        );
    });

    connect(&mMagnetPosSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto pos = mMagnetPosSpinBox.getVector();
        setEmitterProperty(
            "Set Magnet Pos",
            "SetFieldMagnetPos",
            &Ptcl::Emitter::fieldMagnetPos,
            &Ptcl::Emitter::setFieldMagnetPos,
            pos
        );
    });

    connect(&mAxisXCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Magnet X-Target",
            "ToggleFieldMagnetTargetX",
            &Ptcl::Emitter::isFieldMagnetAxisTargetX,
            &Ptcl::Emitter::setFieldMagnetAxisTargetX,
            checked
        );
        mMagnetPosSpinBox.disableAxis(VectorSpinBoxBase::Axis::X, !checked);
    });

    connect(&mAxisYCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Magnet Y-Target",
            "ToggleFieldMagnetTargetY",
            &Ptcl::Emitter::isFieldMagnetAxisTargetY,
            &Ptcl::Emitter::setFieldMagnetAxisTargetY,
            checked
        );
        mMagnetPosSpinBox.disableAxis(VectorSpinBoxBase::Axis::Y, !checked);
    });

    connect(&mAxisZCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Magnet Z-Target",
            "ToggleFieldMagnetTargetZ",
            &Ptcl::Emitter::isFieldMagnetAxisTargetZ,
            &Ptcl::Emitter::setFieldMagnetAxisTargetZ,
            checked
        );
        mMagnetPosSpinBox.disableAxis(VectorSpinBoxBase::Axis::Z, !checked);
    });
}


void FieldMagnetInspector::setWidgetsEnabled(bool enable) {
    mMagnetPowerSpinBox.setEnabled(enable);
    mMagnetPosSpinBox.setEnabled(enable);
    mAxisXCheckBox.setEnabled(enable);
    mAxisYCheckBox.setEnabled(enable);
    mAxisZCheckBox.setEnabled(enable);

    if (enable) {
        mMagnetPosSpinBox.disableAxis(VectorSpinBoxBase::Axis::X, !mAxisXCheckBox.isChecked());
        mMagnetPosSpinBox.disableAxis(VectorSpinBoxBase::Axis::Y, !mAxisYCheckBox.isChecked());
        mMagnetPosSpinBox.disableAxis(VectorSpinBoxBase::Axis::Z, !mAxisZCheckBox.isChecked());
    }
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
    setWidgetsEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
