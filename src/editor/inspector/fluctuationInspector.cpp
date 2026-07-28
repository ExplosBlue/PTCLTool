#include "editor/inspector/fluctuationInspector.h"

#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

namespace PtclEditor {


// ========================================================================== //


FluctuationInspector::FluctuationInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    addSectionHeader(mainLayout, "Fluctuation", this);

    mEnabledCheckBox.setToolTip("When enabled, fluctuation is applied to particles.");
    mainLayout->addRow("Enable Fluctuation:", &mEnabledCheckBox);

    addSectionHeader(mainLayout, "Wave", this);

    auto* waveContainer = new QWidget(this);
    auto* waveLayout = new QGridLayout(waveContainer);
    waveLayout->setContentsMargins(0, 0, 0, 0);
    waveLayout->setColumnStretch(0, 0);
    waveLayout->setColumnStretch(1, 0);
    waveLayout->setColumnStretch(2, 1);

    mScaleSpinBox.setRange(0.0f, 1.0f);
    mScaleSpinBox.setSingleStep(0.01f);
    mScaleSpinBox.setDecimals(3);
    mScaleSpinBox.setToolTip("Controls the strength of the oscillation. 0.0 = no effect, 1.0 = full wave range.");
    waveLayout->addWidget(new QLabel("Amplitude:", this), 0, 0);
    waveLayout->addWidget(&mScaleSpinBox, 0, 1);

    mFreqSpinBox.setRange(0.0f, 128.0f);
    mFreqSpinBox.setSingleStep(0.1f);
    mFreqSpinBox.setDecimals(3);
    mFreqSpinBox.setToolTip("Controls how fast the oscillation cycles. Higher values = faster pulsing.");
    waveLayout->addWidget(new QLabel("Frequency:", this), 1, 0);
    waveLayout->addWidget(&mFreqSpinBox, 1, 1);

    mPhaseRndCheckBox.setText("Use Random Phase");
    mPhaseRndCheckBox.setToolTip("When enabled, each particle starts at a random point in the wave cycle, creating a staggering effect. When disabled, all particles pulse in sync.");
    waveLayout->addWidget(new QLabel("Phase:", this), 2, 0);
    waveLayout->addWidget(&mPhaseRndCheckBox, 2, 1);

    mWavePreview.setMinimumHeight(140);
    mWavePreview.setToolTip("Visual preview of the fluctuation waveform.\nThe wave oscillates down from 1.0 based on amplitude.");
    waveLayout->addWidget(&mWavePreview, 0, 2, 3, 1);

    mainLayout->addRow(waveContainer);

    addSectionHeader(mainLayout, "Targets", this);

    mApplyAlphaCheckBox.setText("Alpha");
    mApplyAlphaCheckBox.setToolTip("Oscillate particle transparency over time, creating a pulsing fade effect.");

    mApplyScaleCheckBox.setText("Scale");
    mApplyScaleCheckBox.setToolTip("Oscillate particle size over time, creating a pulsing size effect.");

    auto* applyToLayout = new QHBoxLayout();
    applyToLayout->addWidget(&mApplyAlphaCheckBox);
    applyToLayout->addWidget(&mApplyScaleCheckBox);
    applyToLayout->addStretch();
    mainLayout->addRow("Apply to:", applyToLayout);

    setupConnections();
}

void FluctuationInspector::setupConnections() {
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Fluctuation Enabled",
            "SetFluxEnable",
            &Ptcl::Emitter::isFluctuationEnabled,
            &Ptcl::Emitter::setFluctuationEnabled,
            checked
        );
    });

    connect(&mApplyAlphaCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Flux Alpha",
            "SetFluxApplyAlpha",
            &Ptcl::Emitter::isFluctuationApplyAlpha,
            &Ptcl::Emitter::setFluctuationApplyAlpha,
            checked
        );
    });

    connect(&mApplyScaleCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Flux Scale",
            "SetFluxApplyScale",
            &Ptcl::Emitter::isFluctuationApplyScale,
            &Ptcl::Emitter::setFluctuationApplyScale,
            checked
        );
    });

    connect(&mScaleSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Flux Amplitude",
            "SetFluxScale",
            &Ptcl::Emitter::fluctuationScale,
            &Ptcl::Emitter::setFluctuationScale,
            static_cast<f32>(value)
        );
        mWavePreview.setAmplitude(static_cast<f32>(value));
    });

    connect(&mFreqSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Flux Frequency",
            "SetFluxFreq",
            &Ptcl::Emitter::fluctuationFrequency,
            &Ptcl::Emitter::setFluctuationFrequency,
            static_cast<f32>(value)
        );
        mWavePreview.setFrequency(static_cast<f32>(value));
    });

    connect(&mPhaseRndCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Flux Phase",
            "SetFluxRandom",
            &Ptcl::Emitter::isFluctuationPhaseRandom,
            &Ptcl::Emitter::setFluctuationPhaseRandom,
            checked
        );
    });
}

void FluctuationInspector::setWidgetsEnabled(bool enable) {
    mScaleSpinBox.setEnabled(enable);
    mFreqSpinBox.setEnabled(enable);
    mPhaseRndCheckBox.setEnabled(enable);
    mApplyAlphaCheckBox.setEnabled(enable);
    mApplyScaleCheckBox.setEnabled(enable);
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

    mWavePreview.setAmplitude(mEmitter->fluctuationScale());
    mWavePreview.setFrequency(mEmitter->fluctuationFrequency());

    const s32 ptclLife = mEmitter->ptclLife();
    const bool isInfinite = (ptclLife >= sLifeInfinite);
    const s32 previewFrames = isInfinite ? sInfinitePreviewFrames : ptclLife;
    mWavePreview.setPreviewFrames(previewFrames);

    mApplyAlphaCheckBox.setChecked(mEmitter->isFluctuationApplyAlpha());
    mApplyScaleCheckBox.setChecked(mEmitter->isFluctuationApplyScale());

    const bool isEnabled = mEmitter->isFluctuationEnabled();
    mEnabledCheckBox.setChecked(isEnabled);
    setWidgetsEnabled(isEnabled);
}


// ==========================================================================//


} // namespace PtclEditor
