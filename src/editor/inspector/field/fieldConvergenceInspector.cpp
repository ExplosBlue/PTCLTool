#include "editor/inspector/field/fieldConvergenceInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


static const std::array convergenceTypeOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::FieldConvergenceType>{ Ptcl::FieldConvergenceType::AssignedPos, "Assigned Position", "Use a fixed position as the target." },
    EnumOption<Ptcl::FieldConvergenceType>{ Ptcl::FieldConvergenceType::EmitterPos,  "Emitter Position",  "Use the emitter's current position as the target." },
};


FieldConvergenceInspector::FieldConvergenceInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    addSectionHeader(mainLayout, "Convergence", this);

    mEnabledCheckBox.setToolTip("Attracts particles toward a target position over time.");
    mainLayout->addRow("Enable Convergence:", &mEnabledCheckBox);

    addSectionHeader(mainLayout, "Target", this);

    mTypeSpinBox.setOptions(convergenceTypeOptions);
    mTypeSpinBox.setDescription("Determines how the target point is set.");
    mainLayout->addRow("Position Source:", &mTypeSpinBox);

    mPosSpinBox.setRange(-9999.0f, 9999.0f);
    mPosSpinBox.setToolTip("The point in space that particles are pulled toward.");
    mPosSpinBox.setAxisToolTip(VectorSpinBoxBase::Axis::X, "Convergence target on the X axis.");
    mPosSpinBox.setAxisToolTip(VectorSpinBoxBase::Axis::Y, "Convergence target on the Y axis.");
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

    connect(&mTypeSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mTypeSpinBox.currentEnum();
        setEmitterProperty(
            "Set Convergence Source",
            "SetFieldConvergenceType",
            &Ptcl::Emitter::fieldConvergenceType,
            &Ptcl::Emitter::setFieldConvergenceType,
            type
        );
        const bool isEnabled = mEnabledCheckBox.isChecked() && type == Ptcl::FieldConvergenceType::AssignedPos;
        mPosSpinBox.setEnabled(isEnabled);
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
        const auto currentPos = mEmitter->fieldConvergencePos();
        const Math::Vector3f pos3(currentPos.getX(), currentPos.getY(), static_cast<f32>(value));
        setEmitterProperty(
            "Set Convergence Speed",
            "SetFieldConvergenceSpeed",
            &Ptcl::Emitter::fieldConvergencePos,
            &Ptcl::Emitter::setFieldConvergencePos,
            pos3
        );
    });
}


void FieldConvergenceInspector::setWidgetsEnabled(bool enable) {
    const bool isAssigned = mTypeSpinBox.currentEnum() == Ptcl::FieldConvergenceType::AssignedPos;
    mPosSpinBox.setEnabled(enable && isAssigned);
    mTypeSpinBox.setEnabled(enable);
    mSpeedSpinBox.setEnabled(enable);
}


void FieldConvergenceInspector::populateProperties() {
    QSignalBlocker b1(mTypeSpinBox);
    QSignalBlocker b2(mPosSpinBox);
    QSignalBlocker b3(mSpeedSpinBox);
    QSignalBlocker b4(mEnabledCheckBox);

    mTypeSpinBox.setCurrentEnum(mEmitter->fieldConvergenceType());

    const auto& pos = mEmitter->fieldConvergencePos();
    mPosSpinBox.setVector(Math::Vector2f(pos.getX(), pos.getY()));
    mSpeedSpinBox.setValue(pos.getZ());

    const bool isEnabled = mEmitter->isFieldConvergenceEnabled();
    mEnabledCheckBox.setChecked(isEnabled);
    setWidgetsEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
