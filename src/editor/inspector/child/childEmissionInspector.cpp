#include "editor/inspector/child/childEmissionInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


ChildEmissionInspector::ChildEmissionInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

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
