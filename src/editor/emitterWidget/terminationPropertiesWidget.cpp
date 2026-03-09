#include "editor/emitterWidget/terminationPropertiesWidget.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


TerminationPropertiesWidget::TerminationPropertiesWidget(QWidget* parent) :
    EmitterWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);

    mAlphaAddInSpinBox.setRange(0.0f, 1.0f);

    mainLayout->addRow("Stop Emission During Fade:", &mIsStopEmitCheckBox);
    mainLayout->addRow("Alpha Add During Fade:", &mAlphaAddInSpinBox);

    setupConnections();
}

void TerminationPropertiesWidget::setupConnections() {
    connect(&mIsStopEmitCheckBox, &QCheckBox::checkStateChanged, this, [this](bool checked) {
        setEmitterProperty(
            "Set Stop Emission",
            "SetStopEmission",
            &Ptcl::Emitter::isStopEmitInFade,
            &Ptcl::Emitter::setIsStopEmitInFade,
            checked
        );
    });

    connect(&mAlphaAddInSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Alpha Add In Fade",
            "SetAlphaAddIn",
            &Ptcl::Emitter::alphaAddInFade,
            &Ptcl::Emitter::setAlphaAddInFade,
            value
        );
    });
}

void TerminationPropertiesWidget::populateProperties() {
    QSignalBlocker b1(mIsStopEmitCheckBox);
    QSignalBlocker b2(mAlphaAddInSpinBox);

    mIsStopEmitCheckBox.setChecked(mEmitter->isStopEmitInFade());
    mAlphaAddInSpinBox.setValue(mEmitter->alphaAddInFade());
}


// ========================================================================== //


} // namespace PtclEditor
