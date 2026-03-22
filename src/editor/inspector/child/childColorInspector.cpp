#include "editor/inspector/child/childColorInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


ChildColorInspector::ChildColorInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);

    mInheritColorCheckBox.setText("Inherit Parent Primary Color");
    mColor1Widget.enableAlpha(false);

    mainLayout->addRow("Color:", &mInheritColorCheckBox);
    mainLayout->addRow("Primary Color:", &mColor0Widget);
    mainLayout->addRow("Secondary Color:", &mColor1Widget);

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
            "Set Child Primary Color",
            "SetChildPrimaryColor",
            &Ptcl::Emitter::childPrimaryColor,
            &Ptcl::Emitter::setChildPrimaryColor,
            newColor
        );
    });

    // Color 1
    connect(&mColor1Widget, &RGBAColorWidget::colorChanged, this, [this]() {
        const auto color = mColor1Widget.color();
        setEmitterProperty(
            "Set Child Secondary Color",
            "SetChildSecondaryColor",
            &Ptcl::Emitter::childSecondaryColor,
            &Ptcl::Emitter::setChildSecondaryColor,
            color
        );
    });
}

void ChildColorInspector::populateProperties() {
    QSignalBlocker b1(mColor0Widget);
    QSignalBlocker b2(mColor1Widget);
    QSignalBlocker b3(mInheritColorCheckBox);

    const bool inheritParentColor = mEmitter->isChildInheritParentColor();

    mColor0Widget.setColor(mEmitter->childPrimaryColor());
    mColor0Widget.setDisabled(inheritParentColor);

    mColor1Widget.setColor(mEmitter->childSecondaryColor());
    mInheritColorCheckBox.setChecked(inheritParentColor);
}


// ========================================================================== //


} // namespace PtclEditor
