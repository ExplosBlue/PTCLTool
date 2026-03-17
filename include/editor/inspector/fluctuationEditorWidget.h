#pragma once

#include "editor/inspector/inspectorWidgetBase.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class FluctuationEditorWidget final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit FluctuationEditorWidget(QWidget* parent = nullptr);

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
