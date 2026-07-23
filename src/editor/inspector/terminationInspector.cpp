#include "editor/inspector/terminationInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


TerminationInspector::TerminationInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    mAlphaAddInSpinBox.setRange(0.0f, 1.0f);
    mAlphaAddInSpinBox.setSingleStep(0.1f);

    mIsStopEmitCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mIsStopEmitCheckBox.setToolTip("Prevents new particles from being spawned while the emitter is fading out.");
    mainLayout->addRow("Stop Emission During Fade:", &mIsStopEmitCheckBox);

    mAlphaAddInSpinBox.setToolTip("The rate at which the emitter's alpha changes each frame during fade-in and fade-out.\nA value of 0 disables automatic fading.");
    mainLayout->addRow("Fade Speed:", &mAlphaAddInSpinBox);

    setupConnections();
}

void TerminationInspector::setupConnections() {
    connect(&mIsStopEmitCheckBox, &QCheckBox::checkStateChanged, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Stop Emission",
            "SetStopEmission",
            &Ptcl::Emitter::isStopEmitInFade,
            &Ptcl::Emitter::setIsStopEmitInFade,
            checked
        );
    });

    connect(&mAlphaAddInSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Fade Speed",
            "SetAlphaAddIn",
            &Ptcl::Emitter::alphaAddInFade,
            &Ptcl::Emitter::setAlphaAddInFade,
            value
        );
    });
}

void TerminationInspector::populateProperties() {
    QSignalBlocker b1(mIsStopEmitCheckBox);
    QSignalBlocker b2(mAlphaAddInSpinBox);

    mIsStopEmitCheckBox.setChecked(mEmitter->isStopEmitInFade());
    mAlphaAddInSpinBox.setValue(mEmitter->alphaAddInFade());
}


// ========================================================================== //


} // namespace PtclEditor
