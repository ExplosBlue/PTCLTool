#include "editor/inspector/child/childAlphaInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


ChildAlphaInspector::ChildAlphaInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    mAlphaSpinBox.setRange(0.0f, 1.0f);
    mAlphaSpinBox.setSingleStep(0.1f);

    mAlphaTargetSpinBox.setRange(0.0f, 1.0f);
    mAlphaTargetSpinBox.setSingleStep(0.1f);

    mAlphaInitSpinBox.setRange(0.0f, 1.0f);
    mAlphaInitSpinBox.setSingleStep(0.1f);

    mStartFrameSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mBaseFrameSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mInheritAlphaCheckBox.setText("Inherit Parent Alpha");

    auto* mainLayout = new QFormLayout(this);

    mainLayout->addRow("Alpha:", &mInheritAlphaCheckBox);
    mainLayout->addRow("Alpha:", &mAlphaSpinBox);
    mainLayout->addRow("Alpha Target:", &mAlphaTargetSpinBox);
    mainLayout->addRow("Alpha Init:", &mAlphaInitSpinBox);
    mainLayout->addRow("Start Frame:", &mStartFrameSpinBox);
    mainLayout->addRow("Base Frame:", &mBaseFrameSpinBox);

    setLayout(mainLayout);
    setupConnections();
}

void ChildAlphaInspector::setupConnections() {
    // Inherit Alpha
    connect(&mInheritAlphaCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Child Inherit Alpha",
            "ToggleChildInheritAlpha",
            &Ptcl::Emitter::isChildInheritAlpha,
            &Ptcl::Emitter::setChildInheritAlpha,
            checked
        );
    });

    // Alpha
    connect(&mAlphaSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Child Alpha",
            "SetChildAlpha",
            &Ptcl::Emitter::childAlpha,
            &Ptcl::Emitter::setChildAlpha,
            static_cast<f32>(value)
        );
    });

    // Alpha Target
    connect(&mAlphaTargetSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Child Alpha Target",
            "SetChildAlphaTarget",
            &Ptcl::Emitter::childAlphaTarget,
            &Ptcl::Emitter::setChildAlphaTarget,
            static_cast<f32>(value)
        );
    });

    // Alpha Init
    connect(&mAlphaInitSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Child Alpha Init",
            "SetChildAlphaInit",
            &Ptcl::Emitter::childAlphaInit,
            &Ptcl::Emitter::setChildAlphaInit,
            static_cast<f32>(value)
        );
    });

    // Start Frame
    connect(&mStartFrameSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        setEmitterProperty(
            "Set Child Alpha Start Frame",
            "SetChildAlphaStartFrame",
            &Ptcl::Emitter::childAlphaStartFrame,
            &Ptcl::Emitter::setChildAlphaStartFrame,
            value
        );
    });

    // Base Frame
    connect(&mBaseFrameSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        setEmitterProperty(
            "Set Child Alpha Base Frame",
            "SetChildAlphaBaseFrame",
            &Ptcl::Emitter::childAlphaBaseFrame,
            &Ptcl::Emitter::setChildAlphaBaseFrame,
            value
        );
    });
}

void ChildAlphaInspector::populateProperties() {
    QSignalBlocker b1(mAlphaSpinBox);
    QSignalBlocker b2(mAlphaTargetSpinBox);
    QSignalBlocker b3(mAlphaInitSpinBox);
    QSignalBlocker b4(mStartFrameSpinBox);
    QSignalBlocker b5(mBaseFrameSpinBox);
    QSignalBlocker b6(mInheritAlphaCheckBox);

    mAlphaSpinBox.setValue(mEmitter->childAlpha());
    mAlphaTargetSpinBox.setValue(mEmitter->childAlphaTarget());
    mAlphaInitSpinBox.setValue(mEmitter->childAlphaInit());
    mStartFrameSpinBox.setValue(mEmitter->childAlphaStartFrame());
    mBaseFrameSpinBox.setValue(mEmitter->childAlphaBaseFrame());

    mInheritAlphaCheckBox.setChecked(mEmitter->isChildInheritAlpha());
}


// ========================================================================== //


} // namespace PtclEditor
