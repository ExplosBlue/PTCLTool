#include "editor/inspector/child/childColorInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ==========================================================================//


ChildColorInspector::ChildColorInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    // Color Section
    addSectionHeader(mainLayout, "Color", this);

    mInheritColorCheckBox.setText("Inherit Parent Primary Color");
    mInheritColorCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mColor1Widget.enableAlpha(false);

    mainLayout->addRow("Color:", &mInheritColorCheckBox);
    mainLayout->addRow("Secondary Color:", &mColor0Widget);
    mainLayout->addRow("Primary Color:", &mColor1Widget);

    // Alpha Section
    addSectionHeader(mainLayout, "Alpha", this);

    mAlphaSpinBox.setRange(0.0f, 1.0f);
    mAlphaSpinBox.setSingleStep(0.1f);

    mAlphaTargetSpinBox.setRange(0.0f, 1.0f);
    mAlphaTargetSpinBox.setSingleStep(0.1f);

    mAlphaInitSpinBox.setRange(0.0f, 1.0f);
    mAlphaInitSpinBox.setSingleStep(0.1f);

    mStartFrameSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mBaseFrameSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mInheritAlphaCheckBox.setText("Inherit Parent Alpha");
    mInheritAlphaCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    mainLayout->addRow("Alpha:", &mInheritAlphaCheckBox);
    mainLayout->addRow("Alpha:", &mAlphaSpinBox);
    mainLayout->addRow("Alpha Target:", &mAlphaTargetSpinBox);
    mainLayout->addRow("Alpha Init:", &mAlphaInitSpinBox);
    mainLayout->addRow("Start Frame:", &mStartFrameSpinBox);
    mainLayout->addRow("Base Frame:", &mBaseFrameSpinBox);

    setLayout(mainLayout);
    setupConnections();
}

void ChildColorInspector::setupConnections() {
    // Inherit Color
    connect(&mInheritColorCheckBox, &QCheckBox::clicked, this, [this](bool checked) {        
        setEmitterProperty(
            "Toggle Child Inherit Parent Color",
            "ToggleChildInheritColor",
            &Ptcl::Emitter::isChildInheritParentColor,
            &Ptcl::Emitter::setChildInheritParentColor,
            checked
        );
    });

    // Color 0
    connect(&mColor0Widget, &RGBAColorWidget::colorChanged, this, [this]() {
        const auto newColor = mColor0Widget.color();
        setEmitterProperty(
            "Set Child Secondary Color",
            "SetChildSecondaryColor",
            &Ptcl::Emitter::childSecondaryColor,
            &Ptcl::Emitter::setChildSecondaryColor,
            newColor
        );
    });

    // Color 1
    connect(&mColor1Widget, &RGBAColorWidget::colorChanged, this, [this]() {
        const auto color = mColor1Widget.color();
        setEmitterProperty(
            "Set Child Primary Color",
            "SetChildPrimaryColor",
            &Ptcl::Emitter::childPrimaryColor,
            &Ptcl::Emitter::setChildPrimaryColor,
            color
        );
    });

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

void ChildColorInspector::populateProperties() {
    QSignalBlocker b1(mColor0Widget);
    QSignalBlocker b2(mColor1Widget);
    QSignalBlocker b3(mInheritColorCheckBox);
    QSignalBlocker b4(mAlphaSpinBox);
    QSignalBlocker b5(mAlphaTargetSpinBox);
    QSignalBlocker b6(mAlphaInitSpinBox);
    QSignalBlocker b7(mStartFrameSpinBox);
    QSignalBlocker b8(mBaseFrameSpinBox);
    QSignalBlocker b9(mInheritAlphaCheckBox);

    const bool inheritParentColor = mEmitter->isChildInheritParentColor();

    mColor0Widget.setColor(mEmitter->childSecondaryColor());
    mColor0Widget.setDisabled(inheritParentColor);

    mColor1Widget.setColor(mEmitter->childPrimaryColor());
    mInheritColorCheckBox.setChecked(inheritParentColor);

    mAlphaSpinBox.setValue(mEmitter->childAlpha());
    mAlphaTargetSpinBox.setValue(mEmitter->childAlphaTarget());
    mAlphaInitSpinBox.setValue(mEmitter->childAlphaInit());
    mStartFrameSpinBox.setValue(mEmitter->childAlphaStartFrame());
    mBaseFrameSpinBox.setValue(mEmitter->childAlphaBaseFrame());

    mInheritAlphaCheckBox.setChecked(mEmitter->isChildInheritAlpha());
}


// ==========================================================================//


} // namespace PtclEditor
