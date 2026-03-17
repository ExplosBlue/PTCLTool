#include "editor/inspector/field/fieldRandomInspector.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


FieldRandomInspector::FieldRandomInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    // TODO: Better ranges?
    mRandomVelAddSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mRandomBlankSpinBox.setRange(std::numeric_limits<s32>::min(), std::numeric_limits<s32>::max());
    mEnabledCheckBox.setText("Enabled");

    mControlsWidget = new QWidget(this);
    auto* controlsLayout = new QFormLayout(mControlsWidget);
    controlsLayout->addRow("Random Velocity Add:", &mRandomVelAddSpinBox);
    controlsLayout->addRow("Random Blank:", &mRandomBlankSpinBox);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Randomness:", &mEnabledCheckBox);
    mainLayout->addRow(mControlsWidget);

    setupConnections();
}

void FieldRandomInspector::setupConnections() {
    // Is Enabled
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Field Random",
            "ToggleFieldRandom",
            &Ptcl::Emitter::isFieldRandomEnabled,
            &Ptcl::Emitter::setFieldRandomEnabled,
            checked
        );
    });

    // Random Vel Add
    connect(&mRandomVelAddSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto velocity = mRandomVelAddSpinBox.getVector();
        setEmitterProperty(
            "Set Field Random Velocity",
            "SetFieldRandomVelAdd",
            &Ptcl::Emitter::fieldRandomVelAdd,
            &Ptcl::Emitter::setFieldRandomVelAdd,
            velocity
        );
    });

    // Random Blank
    connect(&mRandomBlankSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        setEmitterProperty(
            "Set Field Random Blank",
            "SetFieldRandomBlank",
            &Ptcl::Emitter::fieldRandomBlank,
            &Ptcl::Emitter::setFieldRandomBlank,
            value
        );
    });
}

void FieldRandomInspector::populateProperties() {
    QSignalBlocker b1(mRandomBlankSpinBox);
    QSignalBlocker b2(mRandomVelAddSpinBox);
    QSignalBlocker b3(mEnabledCheckBox);

    mRandomBlankSpinBox.setValue(mEmitter->fieldRandomBlank());
    mRandomVelAddSpinBox.setVector(mEmitter->fieldRandomVelAdd());

    const bool isEnabled = mEmitter->isFieldRandomEnabled();
    mEnabledCheckBox.setChecked(isEnabled);
    mControlsWidget->setEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
