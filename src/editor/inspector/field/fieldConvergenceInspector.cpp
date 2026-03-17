#include "editor/inspector/field/fieldConvergenceInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


FieldConvergenceInspector::FieldConvergenceInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    // TODO: Better ranges?
    mPosSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mEnabledCheckBox.setText("Enabled");

    mControlsWidget = new QWidget(this);
    auto* controlsLayout = new QFormLayout(mControlsWidget);
    controlsLayout->addRow("Convergence Position:", &mPosSpinBox);
    controlsLayout->addRow("Position Follow Type:", &mTypeSpinBox);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Convergence:", &mEnabledCheckBox);
    mainLayout->addRow(mControlsWidget);

    setupConnections();
}

void FieldConvergenceInspector::setupConnections() {
    // Is Enabled
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Field Convergence",
            "ToggleFieldConvergence",
            &Ptcl::Emitter::isFieldConvergenceEnabled,
            &Ptcl::Emitter::setFieldConvergenceEnabled,
            checked
        );
    });

    // Type
    connect(&mTypeSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mTypeSpinBox.currentEnum();
        setEmitterProperty(
            "Set Field Convergence Type",
            "SetFieldConvergenceType",
            &Ptcl::Emitter::fieldConvergenceType,
            &Ptcl::Emitter::setFieldConvergenceType,
            type
        );
    });

    // Position
    connect(&mPosSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto pos = mPosSpinBox.getVector();
        setEmitterProperty(
            "Set Field Convergence Pos",
            "SetFieldConvergencePos",
            &Ptcl::Emitter::fieldConvergencePos,
            &Ptcl::Emitter::setFieldConvergencePos,
            pos
        );
    });
}

void FieldConvergenceInspector::populateProperties() {
    QSignalBlocker b1(mTypeSpinBox);
    QSignalBlocker b2(mPosSpinBox);
    QSignalBlocker b3(mEnabledCheckBox);

    mTypeSpinBox.setCurrentEnum(mEmitter->fieldConvergenceType());
    mPosSpinBox.setVector(mEmitter->fieldConvergencePos());

    const bool isEnabled = mEmitter->isFieldConvergenceEnabled();
    mEnabledCheckBox.setChecked(isEnabled);
    mControlsWidget->setEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
