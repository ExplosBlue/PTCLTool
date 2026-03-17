#include "editor/inspector/field/fieldPosAddInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


FieldPosAddInspector::FieldPosAddInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    // TODO: Set better limits?
    mPosSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mEnabledCheckBox.setText("Enabled");

    mControlsWidget = new QWidget(this);
    auto* controlsLayout = new QFormLayout(mControlsWidget);
    controlsLayout->addRow("Add-on value:", &mPosSpinBox);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Add to Position:", &mEnabledCheckBox);
    mainLayout->addRow(mControlsWidget);

    setupConnections();
}

void FieldPosAddInspector::setupConnections() {
    // Is Enabled
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Field PosAdd",
            "ToggleFieldPosAdd",
            &Ptcl::Emitter::isFieldPosAddEnabled,
            &Ptcl::Emitter::setFieldPosAddEnabled,
            checked
        );
    });

    // Pos Add
    connect(&mPosSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto pos = mPosSpinBox.getVector();
        setEmitterProperty(
            "Set Field PosAdd Position",
            "SetFieldPosAdd",
            &Ptcl::Emitter::fieldPosAddPosition,
            &Ptcl::Emitter::setFieldPosAddPosition,
            pos
        );
    });
}

void FieldPosAddInspector::populateProperties() {
    QSignalBlocker b1(mPosSpinBox);
    QSignalBlocker b2(mEnabledCheckBox);

    mPosSpinBox.setVector(mEmitter->fieldPosAddPosition());

    const bool isEnabled = mEmitter->isFieldPosAddEnabled();
    mEnabledCheckBox.setChecked(isEnabled);
    mControlsWidget->setEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
