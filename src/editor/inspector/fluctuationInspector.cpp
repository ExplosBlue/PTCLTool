#include "editor/inspector/fluctuationInspector.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


FluctuationInspector::FluctuationInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {
    // TODO: Determine better ranges?
    mScaleSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mFreqSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mPhaseRndCheckBox.setText("Randomness");
    mEnabledCheckBox.setText("Enabled");
    mApplyAlphaCheckBox.setText("Apply to alpha");
    mApplyScaleCheckBox.setText("Apply to scale");

    mControlsContainer = new QWidget(this);
    auto* controlsLayout = new QFormLayout(mControlsContainer);
    controlsLayout->setContentsMargins(0, 0, 0, 0);

    controlsLayout->addRow("Apply to alpha:", &mApplyAlphaCheckBox);
    controlsLayout->addRow("Apply to scale:", &mApplyScaleCheckBox);
    controlsLayout->addRow("Fluctuation Scale:", &mScaleSpinBox);
    controlsLayout->addRow("Fluctuation Frequency:", &mFreqSpinBox);
    controlsLayout->addRow("Phase:", &mPhaseRndCheckBox);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Fluctuation:", &mEnabledCheckBox);
    mainLayout->addWidget(mControlsContainer);

    setupConnections();
}

void FluctuationInspector::setupConnections() {
    // Enabled
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Fluctuation",
            "SetFluxEnable",
            &Ptcl::Emitter::isFluctuationEnabled,
            &Ptcl::Emitter::setFluctuationEnabled,
            checked
        );
    });

    // Apply to alpha
    connect(&mApplyAlphaCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Fluctuation Apply Alpha",
            "SetFluxApplyAlpha",
            &Ptcl::Emitter::isFluctuationApplyAlpha,
            &Ptcl::Emitter::setFluctuationApplyAlpha,
            checked
        );
    });

    // Apply to scale
    connect(&mApplyScaleCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Fluctuation Apply Scale",
            "SetFluxApplyScale",
            &Ptcl::Emitter::isFluctuationApplyScale,
            &Ptcl::Emitter::setFluctuationApplyScale,
            checked
        );
    });

    // Scale
    connect(&mScaleSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Fluctuation Scale",
            "SetFluxScale",
            &Ptcl::Emitter::fluctuationScale,
            &Ptcl::Emitter::setFluctuationScale,
            static_cast<f32>(value)
        );
    });

    // Freq
    connect(&mFreqSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Fluctuation Frequency",
            "SetFluxFreq",
            &Ptcl::Emitter::fluctuationFrequency,
            &Ptcl::Emitter::setFluctuationFrequency,
            static_cast<f32>(value)
        );
    });

    // Phase Rnd
    connect(&mPhaseRndCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Fluctuation Phase Randomness",
            "SetFluxRandom",
            &Ptcl::Emitter::isFluctuationPhaseRandom,
            &Ptcl::Emitter::setFluctuationPhaseRandom,
            checked
        );
    });
}

void FluctuationInspector::populateProperties() {
    QSignalBlocker b1(mScaleSpinBox);
    QSignalBlocker b2(mFreqSpinBox);
    QSignalBlocker b3(mPhaseRndCheckBox);
    QSignalBlocker b4(mApplyAlphaCheckBox);
    QSignalBlocker b5(mApplyScaleCheckBox);
    QSignalBlocker b6(mEnabledCheckBox);

    mScaleSpinBox.setValue(mEmitter->fluctuationScale());
    mFreqSpinBox.setValue(mEmitter->fluctuationFrequency());
    mPhaseRndCheckBox.setChecked(mEmitter->isFluctuationPhaseRandom());

    mApplyAlphaCheckBox.setChecked(mEmitter->isFluctuationApplyAlpha());
    mApplyScaleCheckBox.setChecked(mEmitter->isFluctuationApplyScale());

    const bool isEnabled = mEmitter->isFluctuationEnabled();
    mEnabledCheckBox.setChecked(isEnabled);
    mControlsContainer->setEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
