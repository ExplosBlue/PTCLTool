#include "editor/inspector/lifespanInspector.h"

#include <QFormLayout>
#include <QHBoxLayout>


namespace PtclEditor {


// ========================================================================== //


LifespanInspector::LifespanInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QVBoxLayout(this);

    // Lifetime
    addSectionHeader(mainLayout, "Lifetime", this);

    auto* lifeLayout = new QFormLayout;
    lifeLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    mLifeSpanSpinBox.setRange(0, sLifeInfinite - 1);
    mLifeSpanSpinBox.setSuffix(" Frames");
    mLifeSpanSpinBox.setToolTip("Maximum lifespan of each spawned particle in frames.\nEach particle's actual lifespan is this value minus a random amount (see Variation).");

    mInfiniteLifeCheckBox.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    mInfiniteLifeCheckBox.setToolTip("When checked, spawned particles live until the emitter is killed.");
    mInfiniteLifeCheckBox.setText("Infinite");

    auto* lifespanLayout = new QHBoxLayout();
    lifespanLayout->addWidget(&mLifeSpanSpinBox);
    lifespanLayout->addWidget(&mInfiniteLifeCheckBox);

    lifeLayout->addRow("Max Particle Lifespan:", lifespanLayout);

    mLifeSpanRndSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mLifeSpanRndSpinBox.setSuffix(" Frames");
    mLifeSpanRndSpinBox.setToolTip("Random amount subtracted from max lifespan per particle.\nEach particle's lifespan will be between (Max - Variation) and Max frames.");
    lifeLayout->addRow("Particle Lifespan Variation:", &mLifeSpanRndSpinBox);

    mainLayout->addLayout(lifeLayout);

    // Emission
    addSectionHeader(mainLayout, "Emission", this);

    auto* emissionLayout = new QFormLayout;
    emissionLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    mStartFrameSpinBox.setRange(0, sEmitInfinite - 2);
    mStartFrameSpinBox.setToolTip("The frame on which this emitter begins emitting new particles.");
    emissionLayout->addRow("Emission Start Frame:", &mStartFrameSpinBox);

    mEndFrameSpinBox.setRange(1, sEmitInfinite - 1);
    mEndFrameSpinBox.setToolTip("The frame on which this emitter stops emitting new particles.");

    mInfiniteEmitCheckBox.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    mInfiniteEmitCheckBox.setToolTip("When checked, the emitter will spawn particles infinitely.");
    mInfiniteEmitCheckBox.setText("Infinite");

    auto* endFrameLayout = new QHBoxLayout();
    endFrameLayout->addWidget(&mEndFrameSpinBox);
    endFrameLayout->addWidget(&mInfiniteEmitCheckBox);
    emissionLayout->addRow("Emission End Frame:", endFrameLayout);

    mLifeStepSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mLifeStepSpinBox.setSuffix(" Frames");
    mLifeStepSpinBox.setToolTip("The number of frames inbetween each emission.");
    emissionLayout->addRow("Emission Interval:", &mLifeStepSpinBox);

    mLifeStepRndSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mLifeStepRndSpinBox.setSuffix(" Frames");
    mLifeStepRndSpinBox.setToolTip("Random offset added to the emission interval each cycle.\nA variation of 10 with an interval of 30 means particles spawn every 30-39 frames.");
    emissionLayout->addRow("Emission Interval Variation:", &mLifeStepRndSpinBox);

    mEmitRateSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mEmitRateSpinBox.setSuffix(" Particles");
    mEmitRateSpinBox.setToolTip("The amount of particles emitted with each emission.");
    emissionLayout->addRow("Particles Per Emission:", &mEmitRateSpinBox);

    mainLayout->addLayout(emissionLayout);

    // Termination
    addSectionHeader(mainLayout, "Termination", this);

    auto* terminationLayout = new QFormLayout;
    terminationLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    mAlphaAddInSpinBox.setRange(0.0f, 1.0f);
    mAlphaAddInSpinBox.setSingleStep(0.1f);
    mAlphaAddInSpinBox.setToolTip("The rate at which the emitter's alpha changes each frame during fade-in and fade-out.\nA value of 0 disables automatic fading.");
    terminationLayout->addRow("Fade Speed:", &mAlphaAddInSpinBox);

    mIsStopEmitCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mIsStopEmitCheckBox.setToolTip("Prevents new particles from being spawned while the emitter is fading out.");
    terminationLayout->addRow("Stop Emission During Fade:", &mIsStopEmitCheckBox);

    mainLayout->addLayout(terminationLayout);
    mainLayout->addStretch();

    setupConnections();
}

void LifespanInspector::setupConnections() {
    // Lifetime
    connect(&mLifeSpanSpinBox, &QSpinBox::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Lifespan",
            "SetLifeSpan",
            &Ptcl::Emitter::ptclLife,
            &Ptcl::Emitter::setPtclLife,
            static_cast<s32>(value)
        );
    });

    connect(&mInfiniteLifeCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        const s32 newLife = checked ? sLifeInfinite : 100;

        setEmitterProperty(
            "Toggle Infinite Life",
            "SetParticleLife",
            &Ptcl::Emitter::ptclLife,
            &Ptcl::Emitter::setPtclLife,
            newLife
        );
    });

    connect(&mLifeSpanRndSpinBox, &QSpinBox::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set LifeSpan Random",
            "SetLifeSpanRand",
            &Ptcl::Emitter::ptclLifeRandom,
            &Ptcl::Emitter::setPtclLifeRandom,
            static_cast<s32>(value)
        );
    });

    // Emission
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

    // Termination
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

void LifespanInspector::populateProperties() {
    // Lifespan
    QSignalBlocker b1(mInfiniteLifeCheckBox);
    QSignalBlocker b2(mLifeSpanSpinBox);
    QSignalBlocker b3(mLifeSpanRndSpinBox);

    const s32 lifeSpan = mEmitter->ptclLife();
    const bool infiniteLife = (lifeSpan == sLifeInfinite);

    mLifeSpanSpinBox.setValue(lifeSpan);
    mLifeSpanSpinBox.setDisabled(infiniteLife);
    mInfiniteLifeCheckBox.setChecked(infiniteLife);

    mLifeSpanRndSpinBox.setValue(mEmitter->ptclLifeRandom());

    // Emission
    QSignalBlocker b4(mStartFrameSpinBox);
    QSignalBlocker b5(mEndFrameSpinBox);
    QSignalBlocker b6(mLifeStepSpinBox);
    QSignalBlocker b7(mLifeStepRndSpinBox);
    QSignalBlocker b8(mInfiniteEmitCheckBox);
    QSignalBlocker b9(mEmitRateSpinBox);

    mStartFrameSpinBox.setValue(mEmitter->emitStartFrame());

    const s32 endFrame = mEmitter->emitEndFrame();
    const bool infiniteEmission = (endFrame == sEmitInfinite);

    mEndFrameSpinBox.setValue(endFrame);
    mEndFrameSpinBox.setDisabled(infiniteEmission);
    mInfiniteEmitCheckBox.setChecked(infiniteEmission);

    mLifeStepSpinBox.setValue(mEmitter->lifeStep());
    mLifeStepRndSpinBox.setValue(mEmitter->lifeStepRandom());

    mEmitRateSpinBox.setValue(mEmitter->emitRate());

    // Termination
    QSignalBlocker b10(mIsStopEmitCheckBox);
    QSignalBlocker b11(mAlphaAddInSpinBox);

    mIsStopEmitCheckBox.setChecked(mEmitter->isStopEmitInFade());
    mAlphaAddInSpinBox.setValue(mEmitter->alphaAddInFade());
}


// ========================================================================== //


} // namespace PtclEditor
