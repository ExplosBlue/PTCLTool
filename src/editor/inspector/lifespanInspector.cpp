#include "editor/inspector/lifespanInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


LifespanInspector::LifespanInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);

    mLifeSpanSpinBox.setRange(0, sLifeInfinite - 1);
    mLifeSpanSpinBox.setSuffix(" Frames");

    // TODO: unsure that this is actually treated as a percentage
    // Assumed based on behavior seen in EFT PTCL
    mLifeSpanRndSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mLifeSpanRndSpinBox.setSuffix("%");

    mainLayout->addRow("Infinite Life", &mInfiniteLifeCheckBox);

    mainLayout->addRow("Lifespan:", &mLifeSpanSpinBox);
    mainLayout->addRow("Lifespan Randomness:", &mLifeSpanRndSpinBox);

    setupConnections();
}

void LifespanInspector::setupConnections() {
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
}

void LifespanInspector::populateProperties() {
    QSignalBlocker b1(mInfiniteLifeCheckBox);
    QSignalBlocker b2(mLifeSpanSpinBox);
    QSignalBlocker b3(mLifeSpanRndSpinBox);

    const s32 lifeSpan = mEmitter->ptclLife();
    const bool infiniteLife = (lifeSpan == sLifeInfinite);

    mLifeSpanSpinBox.setValue(lifeSpan);
    mLifeSpanSpinBox.setDisabled(infiniteLife);
    mInfiniteLifeCheckBox.setChecked(infiniteLife);

    mLifeSpanRndSpinBox.setValue(mEmitter->ptclLifeRandom());
}


// ========================================================================== //


} // namespace PtclEditor
