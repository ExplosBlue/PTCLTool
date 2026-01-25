#include "editor/emitterWidget/terminationPropertiesWidget.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


EmitterWidget::TerminationPropertiesWidget::TerminationPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QFormLayout(this);

    mAlphaAddInSpinBox.setRange(0.0f, 1.0f);

    mainLayout->addRow("Stop Emission During Fade:", &mIsStopEmitCheckBox);
    mainLayout->addRow("Alpha Add During Fade:", &mAlphaAddInSpinBox);

    connect(&mIsStopEmitCheckBox, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state) {
        mProps.isStopEmitInFade = (state == Qt::CheckState::Checked);
        emit propertiesUpdated(mProps);
    });

    connect(&mAlphaAddInSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mProps.alphaAddInFade = static_cast<f32>(value);
        emit propertiesUpdated(mProps);
    });
}

void EmitterWidget::TerminationPropertiesWidget::setProperties(const Ptcl::Emitter::TerminationProperties& properties) {
    QSignalBlocker b1(mIsStopEmitCheckBox);
    QSignalBlocker b2(mAlphaAddInSpinBox);

    mProps = properties;

    mIsStopEmitCheckBox.setChecked(mProps.isStopEmitInFade);
    mAlphaAddInSpinBox.setValue(mProps.alphaAddInFade);
}


// ========================================================================== //


} // namespace PtclEditor
