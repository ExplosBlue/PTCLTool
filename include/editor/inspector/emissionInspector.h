#pragma once

#include "editor/inspector/inspectorWidgetBase.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmissionInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit EmissionInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    QSpinBox mStartFrameSpinBox{};
    QSpinBox mEndFrameSpinBox{};
    QCheckBox mInfiniteEmitCheckBox{};
    QSpinBox mLifeStepSpinBox{};
    QSpinBox mLifeStepRndSpinBox{};
    QSpinBox mEmitRateSpinBox{};

    static constexpr s32 sEmitInfinite = std::numeric_limits<s32>::max();
};


// ========================================================================== //


} // namespace PtclEditor
