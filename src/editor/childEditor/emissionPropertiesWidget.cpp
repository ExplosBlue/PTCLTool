#include "editor/childEditor/emissionPropertiesWidget.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


ChildEditorWidget::EmissionPropertiesWidget::EmissionPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QFormLayout(this);

    mainLayout->addRow("Emission Rate:", &mEmitRateSpinBox);
    connect(&mEmitRateSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mProps.emitRate = value;
        emit propertiesUpdated(mProps);
    });

    mainLayout->addRow("Emission Timing:", &mEmitTimingSpinBox);
    connect(&mEmitTimingSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mProps.emitTiming = value;
        emit propertiesUpdated(mProps);
    });

    mainLayout->addRow("Life:", &mLifeSpinBox);
    connect(&mLifeSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mProps.life = value;
        emit propertiesUpdated(mProps);
    });

    mainLayout->addRow("Emission Step:", &mEmitStepSpinBox);
    connect(&mEmitStepSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mProps.emitStep = value;
        emit propertiesUpdated(mProps);
    });
}

void ChildEditorWidget::EmissionPropertiesWidget::setProperties(const Ptcl::ChildData::EmissionProperties& properties) {
    QSignalBlocker b1(mEmitRateSpinBox);
    QSignalBlocker b2(mEmitTimingSpinBox);
    QSignalBlocker b3(mLifeSpinBox);
    QSignalBlocker b4(mEmitStepSpinBox);

    mProps = properties;

    mEmitRateSpinBox.setValue(mProps.emitRate);
    mEmitTimingSpinBox.setValue(mProps.emitTiming);
    mLifeSpinBox.setValue(mProps.life);
    mEmitStepSpinBox.setValue(mProps.emitStep);
}


// ========================================================================== //


} // namespace PtclEditor
