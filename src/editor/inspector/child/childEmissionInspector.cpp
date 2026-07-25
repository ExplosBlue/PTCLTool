#include "editor/inspector/child/childEmissionInspector.h"

#include <QFormLayout>
#include <QVBoxLayout>


namespace PtclEditor {


// ========================================================================== //


ChildEmissionInspector::ChildEmissionInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    // Emission Rate
    mEmitRateSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mEmitRateSpinBox.setSuffix(" Particles");
    mEmitRateSpinBox.setToolTip("The amount of child particles emitted with each emission.");

    // Emission Timing
    mEmitTimingSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mEmitTimingSpinBox.setPrefix("Frame ");
    mEmitTimingSpinBox.setToolTip("The frame on which the child emitter begins emitting child particles.");

    // Lifespan
    mLifeSpinBox.setRange(0, sLifeInfinite - 1);
    mLifeSpinBox.setSuffix(" Frames");
    mLifeSpinBox.setToolTip("Maximum lifespan of each spawned child particle in frames.\nEach particle's actual lifespan is this value minus a random amount.");

    // Infinite Life
    mInfiniteLifeCheckBox.setText("Infinite");
    mInfiniteLifeCheckBox.setToolTip("When checked, spawned child particles live until the emitter is killed.");

    // Emission Step
    mEmitStepSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mEmitStepSpinBox.setSuffix(" Frames");
    mEmitStepSpinBox.setToolTip("The number of frames between each child emission.");

    auto* mainLayout = new QVBoxLayout(this);

    // Lifetime
    addSectionHeader(mainLayout, "Lifetime", this);

    auto* lifetimeLayout = new QFormLayout;
    lifetimeLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    auto* lifeRow = new QWidget(this);
    auto* lifeRowLayout = new QHBoxLayout(lifeRow);
    lifeRowLayout->setContentsMargins(0, 0, 0, 0);
    lifeRowLayout->addWidget(&mLifeSpinBox, 1);
    lifeRowLayout->addWidget(&mInfiniteLifeCheckBox);
    lifetimeLayout->addRow("Max Child Lifespan:", lifeRow);
    mainLayout->addLayout(lifetimeLayout);

    // Emission
    addSectionHeader(mainLayout, "Emission", this);

    auto* emissionLayout = new QFormLayout;
    emissionLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    emissionLayout->addRow("Particles Per Emission:", &mEmitRateSpinBox);
    emissionLayout->addRow("Emission Start Frame:", &mEmitTimingSpinBox);
    emissionLayout->addRow("Emission Interval:", &mEmitStepSpinBox);
    mainLayout->addLayout(emissionLayout);

    mainLayout->addStretch();

    setupConnections();
}

void ChildEmissionInspector::setupConnections() {
    // Emission Rate
    connect(&mEmitRateSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        setEmitterProperty(
            "Set Child Emission Rate",
            "SetChildEmissionRate",
            &Ptcl::Emitter::childEmitRate,
            &Ptcl::Emitter::setChildEmitRate,
            value
        );
    });

    // Emission Timing
    connect(&mEmitTimingSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        setEmitterProperty(
            "Set Child Emission Start Time",
            "SetChildEmissionStartTime",
            &Ptcl::Emitter::childEmitTiming,
            &Ptcl::Emitter::setChildEmitTiming,
            value
        );
    });

    // Lifespan
    connect(&mLifeSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        setEmitterProperty(
            "Set Child Lifespan",
            "SetChildLifespan",
            &Ptcl::Emitter::childLife,
            &Ptcl::Emitter::setChildLife,
            value
        );
    });

    // Infinite Life
    connect(&mInfiniteLifeCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        const s32 newLife = checked ? sLifeInfinite : 100;
        setEmitterProperty(
            "Toggle Child Infinite Life",
            "ToggleChildInfiniteLife",
            &Ptcl::Emitter::childLife,
            &Ptcl::Emitter::setChildLife,
            newLife
        );
    });

    // Emission Step
    connect(&mEmitStepSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        setEmitterProperty(
            "Toggle Child Emission Interval",
            "ToggleChildEmitStep",
            &Ptcl::Emitter::childEmitStep,
            &Ptcl::Emitter::setChildEmitStep,
            value
        );
    });
}

void ChildEmissionInspector::populateProperties() {
    QSignalBlocker b1(mEmitRateSpinBox);
    QSignalBlocker b2(mEmitTimingSpinBox);
    QSignalBlocker b3(mLifeSpinBox);
    QSignalBlocker b4(mEmitStepSpinBox);

    mEmitRateSpinBox.setValue(mEmitter->childEmitRate());
    mEmitTimingSpinBox.setValue(mEmitter->childEmitTiming());
    mEmitStepSpinBox.setValue(mEmitter->childEmitStep());

    const s32 lifeSpan = mEmitter->childLife();
    const bool infiniteLife = (lifeSpan == sLifeInfinite);
    mLifeSpinBox.setValue(lifeSpan);
    mLifeSpinBox.setDisabled(infiniteLife);
    mInfiniteLifeCheckBox.setChecked(infiniteLife);
}


// ========================================================================== //


} // namespace PtclEditor
