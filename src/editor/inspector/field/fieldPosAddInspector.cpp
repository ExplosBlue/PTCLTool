#include "editor/inspector/field/fieldPosAddInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


FieldPosAddInspector::FieldPosAddInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    addSectionHeader(mainLayout, "Wind", this);

    mEnabledCheckBox.setToolTip("Applies a constant force that pushes particles in the specified direction each frame.");
    mainLayout->addRow("Enable Wind:", &mEnabledCheckBox);

    addSectionHeader(mainLayout, "Strength", this);

    mPosSpinBox.setRange(-9999.0f, 9999.0f);
    mPosSpinBox.setToolTip("The amount added to the particle's position each frame.");
    mPosSpinBox.setAxisToolTip(VectorSpinBoxBase::Axis::X, "Wind strength on the X axis.");
    mPosSpinBox.setAxisToolTip(VectorSpinBoxBase::Axis::Y, "Wind strength on the Y axis.");
    mPosSpinBox.setAxisToolTip(VectorSpinBoxBase::Axis::Z, "Wind strength on the Z axis.");
    mainLayout->addRow("Strength:", &mPosSpinBox);

    setupConnections();
}


void FieldPosAddInspector::setupConnections() {
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Field Wind",
            "ToggleFieldPosAdd",
            &Ptcl::Emitter::isFieldPosAddEnabled,
            &Ptcl::Emitter::setFieldPosAddEnabled,
            checked
        );
        setWidgetsEnabled(checked);
    });

    connect(&mPosSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto pos = mPosSpinBox.getVector();
        setEmitterProperty(
            "Set Wind Strength",
            "SetFieldPosAdd",
            &Ptcl::Emitter::fieldPosAddPosition,
            &Ptcl::Emitter::setFieldPosAddPosition,
            pos
        );
    });
}


void FieldPosAddInspector::setWidgetsEnabled(bool enable) {
    mPosSpinBox.setEnabled(enable);
}


void FieldPosAddInspector::populateProperties() {
    QSignalBlocker b1(mPosSpinBox);
    QSignalBlocker b2(mEnabledCheckBox);

    mPosSpinBox.setVector(mEmitter->fieldPosAddPosition());

    const bool isEnabled = mEmitter->isFieldPosAddEnabled();
    mEnabledCheckBox.setChecked(isEnabled);
    setWidgetsEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
