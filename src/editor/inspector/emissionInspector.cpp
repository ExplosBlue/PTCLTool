#include "editor/inspector/emissionInspector.h"

#include <QGridLayout>
#include <QLabel>


namespace PtclEditor {


// ========================================================================== //


EmissionInspector::EmissionInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QGridLayout(this);

    mStartFrameSpinBox.setRange(0, sEmitInfinite - 2);
    mEndFrameSpinBox.setRange(1, sEmitInfinite - 1);
    mLifeStepSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mLifeStepRndSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mEmitRateSpinBox.setRange(0, std::numeric_limits<s32>::max());

    mainLayout->addWidget(&mInfiniteEmitCheckBox, 0, 1);
    mainLayout->addWidget(new QLabel("Infinite Emission:"), 0, 0);

    mainLayout->addWidget(new QLabel("Emission Start Frame:"), 1, 0);
    mainLayout->addWidget(&mStartFrameSpinBox, 1, 1);
    mainLayout->addWidget(new QLabel("Emission End Frame:"), 1, 2);
    mainLayout->addWidget(&mEndFrameSpinBox, 1, 3);

    mainLayout->addWidget(new QLabel("Emission Interval:"), 2, 0);
    mainLayout->addWidget(&mLifeStepSpinBox, 2, 1);
    mainLayout->addWidget(new QLabel("Random Emission Interval:"), 2, 2);
    mainLayout->addWidget(&mLifeStepRndSpinBox, 2, 3);

    mainLayout->addWidget(new QLabel("Emission Rate:"), 3, 0);
    mainLayout->addWidget(&mEmitRateSpinBox, 3, 1);

    mainLayout->setColumnStretch(1, 1);
    mainLayout->setColumnStretch(3, 1);

    setupConnections();
}

void EmissionInspector::setupConnections() {
    connect(&mStartFrameSpinBox, &QSpinBox::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Emission Start Frame",
            "SetEmitStartFrame",
            &Ptcl::Emitter::emitStartFrame,
            &Ptcl::Emitter::setEmitStartFrame,
            static_cast<s32>(value)
        );
    });

    connect(&mEndFrameSpinBox, &QSpinBox::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Emission End Frame",
            "SetEmitEndFrame",
            &Ptcl::Emitter::emitEndFrame,
            &Ptcl::Emitter::setEmitEndFrame,
            static_cast<s32>(value)
        );
    });

    connect(&mInfiniteEmitCheckBox, &QCheckBox::checkStateChanged, this, [this](bool checked) {
        QSignalBlocker b1(mEndFrameSpinBox);

        const s32 endFrame = checked ? sEmitInfinite : 1;

        setEmitterProperty(
            "Toggle Infinite Emission",
            "SetEmitInfinite",
            &Ptcl::Emitter::emitEndFrame,
            &Ptcl::Emitter::setEmitEndFrame,
            endFrame
        );
    });

    connect(&mLifeStepSpinBox, &QSpinBox::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Emission Interval",
            "SetEmitLifeStep",
            &Ptcl::Emitter::lifeStep,
            &Ptcl::Emitter::setLifeStep,
            static_cast<s32>(value)
        );
    });

    connect(&mLifeStepRndSpinBox, &QSpinBox::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Emission Random Interval",
            "SetEmitLifeStepRandom",
            &Ptcl::Emitter::lifeStepRandom,
            &Ptcl::Emitter::setLifeStepRandom,
            static_cast<s32>(value)
        );
    });

    connect(&mEmitRateSpinBox, &QSpinBox::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Emission Rate",
            "SetEmitRate",
            &Ptcl::Emitter::emitRate,
            &Ptcl::Emitter::setEmitRate,
            static_cast<s32>(value)
        );
    });
}

void EmissionInspector::populateProperties() {
    QSignalBlocker b1(mStartFrameSpinBox);
    QSignalBlocker b2(mEndFrameSpinBox);
    QSignalBlocker b3(mLifeStepSpinBox);
    QSignalBlocker b4(mLifeStepRndSpinBox);
    QSignalBlocker b5(mInfiniteEmitCheckBox);
    QSignalBlocker b6(mEmitRateSpinBox);

    mStartFrameSpinBox.setValue(mEmitter->emitStartFrame());

    const s32 endFrame = mEmitter->emitEndFrame();
    const bool infiniteEmission = (endFrame == sEmitInfinite);

    mEndFrameSpinBox.setValue(endFrame);
    mEndFrameSpinBox.setDisabled(infiniteEmission);
    mInfiniteEmitCheckBox.setChecked(infiniteEmission);

    mLifeStepSpinBox.setValue(mEmitter->lifeStep());
    mLifeStepRndSpinBox.setValue(mEmitter->lifeStepRandom());

    mEmitRateSpinBox.setValue(mEmitter->emitRate());
}


// ========================================================================== //


} // namespace PtclEditor
