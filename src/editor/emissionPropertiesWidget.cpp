#include "editor/emissionPropertiesWidget.h"

#include <QGridLayout>
#include <QLabel>

// ========================================================================== //


EmissionPropertiesWidget::EmissionPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QGridLayout(this);

    mStartFrameSpinBox.setRange(0, sEmitInfinite - 2);
    mEndFrameSpinBox.setRange(1, sEmitInfinite - 1);
    mLifeStepSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mLifeStepRndSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mEmitRateSpinBox.setRange(0, std::numeric_limits<s32>::max());

    // Row 0: Infinite Emission
    mainLayout->addWidget(&mInfiniteEmitCheckBox, 0, 1);
    mainLayout->addWidget(new QLabel("Infinite Emission:"), 0, 0);

    // Row 1: Start and End Frame
    mainLayout->addWidget(new QLabel("Emission Start Frame:"), 1, 0);
    mainLayout->addWidget(&mStartFrameSpinBox, 1, 1);
    mainLayout->addWidget(new QLabel("Emission End Frame:"), 1, 2);
    mainLayout->addWidget(&mEndFrameSpinBox, 1, 3);

    // Row 2: Interval and Interval Rand
    mainLayout->addWidget(new QLabel("Emission Interval:"), 2, 0);
    mainLayout->addWidget(&mLifeStepSpinBox, 2, 1);
    mainLayout->addWidget(new QLabel("Random Emission Interval:"), 2, 2);
    mainLayout->addWidget(&mLifeStepRndSpinBox, 2, 3);

    // Row 3: Emission Rate
    mainLayout->addWidget(new QLabel("Emission Rate:"), 3, 0);
    mainLayout->addWidget(&mEmitRateSpinBox, 3, 1);

    mainLayout->setColumnStretch(1, 1);
    mainLayout->setColumnStretch(3, 1);

    connect(&mStartFrameSpinBox, &QSpinBox::valueChanged, this, [this](u64 value) {
        mProps.startFrame = static_cast<s32>(value);
        emit propertiesUpdated(mProps);
    });

    connect(&mEndFrameSpinBox, &QSpinBox::valueChanged, this, [this](u64 value) {
        mProps.endFrame = static_cast<s32>(value);
        emit propertiesUpdated(mProps);
    });

    connect(&mInfiniteEmitCheckBox, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state) {
        QSignalBlocker b1(mEndFrameSpinBox);

        const bool isInfinite = (state == Qt::CheckState::Checked);
        mProps.endFrame = isInfinite ? sEmitInfinite : 1;
        mEndFrameSpinBox.setValue(mProps.endFrame);
        mEndFrameSpinBox.setEnabled(!isInfinite);
        emit propertiesUpdated(mProps);
    });

    connect(&mLifeStepSpinBox, &QSpinBox::valueChanged, this, [this](u64 value) {
        mProps.lifeStep = static_cast<s32>(value);
        emit propertiesUpdated(mProps);
    });

    connect(&mLifeStepRndSpinBox, &QSpinBox::valueChanged, this, [this](u64 value) {
        mProps.lifeStepRnd = static_cast<s32>(value);
        emit propertiesUpdated(mProps);
    });

    connect(&mEmitRateSpinBox, &QSpinBox::valueChanged, this, [this](u64 value) {
        mProps.emitRate = static_cast<s32>(value);
        emit propertiesUpdated(mProps);
    });
}

void EmissionPropertiesWidget::setProperties(const Ptcl::Emitter::EmissionProperties& properties) {
    QSignalBlocker b1(mStartFrameSpinBox);
    QSignalBlocker b2(mEndFrameSpinBox);
    QSignalBlocker b3(mLifeStepSpinBox);
    QSignalBlocker b4(mLifeStepRndSpinBox);
    QSignalBlocker b5(mInfiniteEmitCheckBox);
    QSignalBlocker b6(mEmitRateSpinBox);

    mProps = properties;

    mStartFrameSpinBox.setValue(mProps.startFrame);

    const s32 endFrame = mProps.endFrame;
    const bool infiniteEmission = (endFrame == sEmitInfinite);

    mEndFrameSpinBox.setValue(endFrame);
    mEndFrameSpinBox.setDisabled(infiniteEmission);
    mInfiniteEmitCheckBox.setChecked(infiniteEmission);

    mLifeStepSpinBox.setValue(mProps.lifeStep);
    mLifeStepRndSpinBox.setValue(mProps.lifeStepRnd);

    mEmitRateSpinBox.setValue(mProps.emitRate);
}


// ========================================================================== //
