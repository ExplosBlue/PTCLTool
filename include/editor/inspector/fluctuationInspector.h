#pragma once

#include "editor/inspector/inspectorWidgetBase.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
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

private:
    QWidget* mControlsContainer{nullptr};
    QDoubleSpinBox mScaleSpinBox{};
    QDoubleSpinBox mFreqSpinBox{};
    QCheckBox mPhaseRndCheckBox{};
    QCheckBox mApplyAlphaCheckBox{};
    QCheckBox mApplyScaleCheckBox{};
    QCheckBox mEnabledCheckBox{};
};

// ========================================================================== //


} // namespace PtclEditor
