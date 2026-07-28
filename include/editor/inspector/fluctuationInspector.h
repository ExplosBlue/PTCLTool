#pragma once

#include "editor/components/wavePreviewWidget.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class FluctuationInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit FluctuationInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

    void setWidgetsEnabled(bool enable);

private:
    QDoubleSpinBox mScaleSpinBox{};
    QDoubleSpinBox mFreqSpinBox{};
    QCheckBox mPhaseRndCheckBox{};
    QCheckBox mApplyAlphaCheckBox{};
    QCheckBox mApplyScaleCheckBox{};
    QCheckBox mEnabledCheckBox{};
    WavePreviewWidget mWavePreview{};

    static constexpr s32 sInfinitePreviewFrames = 128;
    static constexpr s32 sLifeInfinite = std::numeric_limits<s32>::max();
};

// ========================================================================== //


} // namespace PtclEditor
