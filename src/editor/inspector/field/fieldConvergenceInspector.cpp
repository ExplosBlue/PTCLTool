#include "editor/inspector/field/fieldConvergenceInspector.h"

#include <QFormLayout>


namespace PtclTool {


// ========================================================================== //


FieldConvergenceInspector::FieldConvergenceInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    addSectionHeader(mainLayout, "Convergence", this);

    mEnabledCheckBox.setToolTip("Attracts particles toward a target position over time.");
    mainLayout->addRow("Enable Convergence:", &mEnabledCheckBox);

    addSectionHeader(mainLayout, "Target", this);

    mPosSpinBox.setRange(-9999.0f, 9999.0f);
    mPosSpinBox.setToolTip("The point in space that particles are pulled toward.");
    mPosSpinBox.setAxisToolTip(VectorSpinBoxBase::Axis::X, "Convergence target on the X axis.");
    mPosSpinBox.setAxisToolTip(VectorSpinBoxBase::Axis::Y, "Convergence target on the Y axis.");
    mPosSpinBox.setAxisToolTip(VectorSpinBoxBase::Axis::Z, "Convergence target on the Z axis.");
    mainLayout->addRow("Target Position:", &mPosSpinBox);

    addSectionHeader(mainLayout, "Speed", this);

    mSpeedSpinBox.setRange(0.0, 1.0);
    mSpeedSpinBox.setDecimals(3);
    mSpeedSpinBox.setSingleStep(0.01);
    mSpeedSpinBox.setToolTip("How quickly particles converge toward the target. 0.0 = no convergence, 1.0 = instant snap.");
    mainLayout->addRow("Convergence Speed:", &mSpeedSpinBox);

    setupConnections();
}

void FieldConvergenceInspector::setupConnections() {
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Convergence",
            "ToggleFieldConvergence",
            &Ptcl::Emitter::isFieldConvergenceEnabled,
            &Ptcl::Emitter::setFieldConvergenceEnabled,
            checked
        );
        setWidgetsEnabled(checked);
    });

    connect(&mPosSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto pos2 = mPosSpinBox.getVector();
        const auto currentPos = mEmitter->fieldConvergencePos();
        const Math::Vector3f pos3(pos2.getX(), pos2.getY(), currentPos.getZ());
        setEmitterProperty(
            "Set Convergence Target",
            "SetFieldConvergencePos",
            &Ptcl::Emitter::fieldConvergencePos,
            &Ptcl::Emitter::setFieldConvergencePos,
            pos3
        );
    });

    connect(&mSpeedSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Convergence Speed",
            "SetFieldConvergenceRate",
            &Ptcl::Emitter::fieldConvergenceRate,
            &Ptcl::Emitter::setFieldConvergenceRate,
            static_cast<f32>(value)
        );
    });
}

void FieldConvergenceInspector::setWidgetsEnabled(bool enable) {
    mPosSpinBox.setEnabled(enable);
    mSpeedSpinBox.setEnabled(enable);
}

void FieldConvergenceInspector::populateProperties() {
    QSignalBlocker b2(mPosSpinBox);
    QSignalBlocker b3(mSpeedSpinBox);
    QSignalBlocker b4(mEnabledCheckBox);

    const auto& pos = mEmitter->fieldConvergencePos();
    mPosSpinBox.setVector(pos);
    mSpeedSpinBox.setValue(mEmitter->fieldConvergenceRate());

    const bool isEnabled = mEmitter->isFieldConvergenceEnabled();
    mEnabledCheckBox.setChecked(isEnabled);
    setWidgetsEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclTool
