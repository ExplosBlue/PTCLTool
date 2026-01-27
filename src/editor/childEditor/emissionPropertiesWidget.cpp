#include "editor/childEditor/emissionPropertiesWidget.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


ChildEditorWidget::EmissionPropertiesWidget::EmissionPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    // Emission Rate
    mEmitRateSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mEmitRateSpinBox.setSuffix(" Particles");

    // Emission Timing
    mEmitTimingSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mEmitTimingSpinBox.setPrefix("Frame ");

    // Lifespan
    mLifeSpinBox.setRange(0, sLifeInfinite - 1);
    mLifeSpinBox.setSuffix(" Frames");

    // Infinite Life
    mInfiniteLifeCheckBox.setText("Infinite");

    // Emission Step
    mEmitStepSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mEmitStepSpinBox.setSuffix(" Frames");

    auto* mainLayout = new QFormLayout(this);

    // Lifespan + Infinite Life
    auto* lifeRow = new QWidget(this);
    auto* lifeLayout = new QHBoxLayout(lifeRow);
    lifeLayout->setContentsMargins(0, 0, 0, 0);
    lifeLayout->addWidget(&mLifeSpinBox, 1);
    lifeLayout->addWidget(&mInfiniteLifeCheckBox);
    mainLayout->addRow("Lifespan:", lifeRow);

    mainLayout->addRow("Emission Rate:", &mEmitRateSpinBox);
    mainLayout->addRow("Emission Start Time:", &mEmitTimingSpinBox);
    mainLayout->addRow("Emission Interval:", &mEmitStepSpinBox);

    setupConnections();
}

void ChildEditorWidget::EmissionPropertiesWidget::setupConnections() {
    // Emission Rate
    connect(&mEmitRateSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mProps.emitRate = value;
        emit propertiesUpdated(mProps);
    });

    // Emission Timing
    connect(&mEmitTimingSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mProps.emitTiming = value;
        emit propertiesUpdated(mProps);
    });

    // Lifespan
    connect(&mLifeSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mProps.life = value;
        emit propertiesUpdated(mProps);
    });

    // Infinite Life
    connect(&mInfiniteLifeCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        QSignalBlocker b1(mLifeSpinBox);

        mProps.life = checked ? sLifeInfinite : 100;
        mLifeSpinBox.setValue(mProps.life);
        mLifeSpinBox.setEnabled(!checked);
        emit propertiesUpdated(mProps);
    });

    // Emission Step
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
    mEmitStepSpinBox.setValue(mProps.emitStep);

    const s32 lifeSpan = mProps.life;
    const bool infiniteLife = (lifeSpan == sLifeInfinite);
    mLifeSpinBox.setValue(lifeSpan);
    mLifeSpinBox.setDisabled(infiniteLife);
    mInfiniteLifeCheckBox.setChecked(infiniteLife);
}


// ========================================================================== //


} // namespace PtclEditor
