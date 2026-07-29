#include "editor/inspector/field/fieldRandomInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


FieldRandomInspector::FieldRandomInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    addSectionHeader(mainLayout, "Random", this);

    mEnabledCheckBox.setToolTip("Randomly adjusts particle velocity at regular intervals.");
    mainLayout->addRow("Enable Random:", &mEnabledCheckBox);

    addSectionHeader(mainLayout, "Timing", this);

    mBlankSpinBox.setRange(1, 9999);
    mBlankSpinBox.setToolTip("How many frames between each random velocity kick. 1 = every frame, 10 = every 10th frame.");
    mBlankSpinBox.setSuffix(" Frames");
    mainLayout->addRow("Interval:", &mBlankSpinBox);

    addSectionHeader(mainLayout, "Velocity", this);

    mVelAddSpinBox.setRange(-9999.0f, 9999.0f);
    mVelAddSpinBox.setToolTip("Maximum velocity offset applied per axis when a random kick fires.");
    mVelAddSpinBox.setAxisToolTip(VectorSpinBoxBase::Axis::X, "Maximum velocity offset on the X axis applied when a random kick fires.");
    mVelAddSpinBox.setAxisToolTip(VectorSpinBoxBase::Axis::Y, "Maximum velocity offset on the Y axis applied when a random kick fires.");
    mVelAddSpinBox.setAxisToolTip(VectorSpinBoxBase::Axis::Z, "Maximum velocity offset on the Z axis applied when a random kick fires.");
    mainLayout->addRow("Strength:", &mVelAddSpinBox);

    setupConnections();
}


void FieldRandomInspector::setupConnections() {
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Field Random",
            "ToggleFieldRandom",
            &Ptcl::Emitter::isFieldRandomEnabled,
            &Ptcl::Emitter::setFieldRandomEnabled,
            checked
        );
        setWidgetsEnabled(checked);
    });

    connect(&mVelAddSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto vel = mVelAddSpinBox.getVector();
        setEmitterProperty(
            "Set Random Strength",
            "SetFieldRandomVelAdd",
            &Ptcl::Emitter::fieldRandomVelAdd,
            &Ptcl::Emitter::setFieldRandomVelAdd,
            vel
        );
    });

    connect(&mBlankSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        setEmitterProperty(
            "Set Random Interval",
            "SetFieldRandomBlank",
            &Ptcl::Emitter::fieldRandomBlank,
            &Ptcl::Emitter::setFieldRandomBlank,
            value
        );
    });
}


void FieldRandomInspector::setWidgetsEnabled(bool enable) {
    mBlankSpinBox.setEnabled(enable);
    mVelAddSpinBox.setEnabled(enable);
}


void FieldRandomInspector::populateProperties() {
    QSignalBlocker b1(mBlankSpinBox);
    QSignalBlocker b2(mVelAddSpinBox);
    QSignalBlocker b3(mEnabledCheckBox);

    mBlankSpinBox.setValue(mEmitter->fieldRandomBlank());
    mVelAddSpinBox.setVector(mEmitter->fieldRandomVelAdd());

    const bool isEnabled = mEmitter->isFieldRandomEnabled();
    mEnabledCheckBox.setChecked(isEnabled);
    setWidgetsEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
