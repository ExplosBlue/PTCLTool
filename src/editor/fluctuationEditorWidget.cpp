#include "editor/fluctuationEditorWidget.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


FluctuationEditorWidget::FluctuationEditorWidget(QWidget* parent) :
    QWidget{parent} {
    // TODO: Determine better ranges?
    mScaleSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mFreqSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mPhaseRndSpinBox.setRange(std::numeric_limits<s32>::min(), std::numeric_limits<s32>::max());
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
    controlsLayout->addRow("Phase Random:", &mPhaseRndSpinBox);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Fluctuation:", &mEnabledCheckBox);
    mainLayout->addWidget(mControlsContainer);

    setupConnections();
}

void FluctuationEditorWidget::setupConnections() {
    // Enabled
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        mControlsContainer->setEnabled(checked);
        mFluxFlag.set(Ptcl::FluctuationFlag::Enabled, checked);
        emit flagsUpdated(mFluxFlag);
    });

    // Apply to alpha
    connect(&mApplyAlphaCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        mFluxFlag.set(Ptcl::FluctuationFlag::ApplyAlpha, checked);
        emit flagsUpdated(mFluxFlag);
    });

    // Apply to scale
    connect(&mApplyScaleCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        mFluxFlag.set(Ptcl::FluctuationFlag::ApplyScale, checked);
        emit flagsUpdated(mFluxFlag);
    });

    // Scale
    connect(&mScaleSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mData.fluctuationScale = static_cast<f32>(value);
        emit dataUpdated(mData);
    });

    // Freq
    connect(&mFreqSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mData.fluctuationFreq = static_cast<f32>(value);
        emit dataUpdated(mData);
    });

    // Phase Rnd
    connect(&mPhaseRndSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mData.fluctuationPhaseRnd = value;
        emit dataUpdated(mData);
    });
}

void FluctuationEditorWidget::setData(const Ptcl::FluctuationData& data, const BitFlag<Ptcl::FluctuationFlag>& fluxFlag) {
    QSignalBlocker b1(mScaleSpinBox);
    QSignalBlocker b2(mFreqSpinBox);
    QSignalBlocker b3(mPhaseRndSpinBox);
    QSignalBlocker b4(mApplyAlphaCheckBox);
    QSignalBlocker b5(mApplyScaleCheckBox);
    QSignalBlocker b6(mEnabledCheckBox);

    mData = data;
    mFluxFlag = fluxFlag;

    mScaleSpinBox.setValue(mData.fluctuationScale);
    mFreqSpinBox.setValue(mData.fluctuationFreq);
    mPhaseRndSpinBox.setValue(mData.fluctuationPhaseRnd);

    mApplyAlphaCheckBox.setChecked(mFluxFlag.isSet(Ptcl::FluctuationFlag::ApplyAlpha));
    mApplyScaleCheckBox.setChecked( mFluxFlag.isSet(Ptcl::FluctuationFlag::ApplyScale));

    const bool isEnabled = mFluxFlag.isSet(Ptcl::FluctuationFlag::Enabled);
    mEnabledCheckBox.setChecked(isEnabled);
    mControlsContainer->setEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
