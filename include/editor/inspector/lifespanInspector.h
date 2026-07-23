#pragma once

#include "editor/inspector/inspectorWidgetBase.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class LifespanInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit LifespanInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    // Lifespan
    QCheckBox mInfiniteLifeCheckBox{};
    QSpinBox mLifeSpanSpinBox{};
    QSpinBox mLifeSpanRndSpinBox{};

    // Emission
    QSpinBox mStartFrameSpinBox{};
    QSpinBox mEndFrameSpinBox{};
    QCheckBox mInfiniteEmitCheckBox{};
    QSpinBox mLifeStepSpinBox{};
    QSpinBox mLifeStepRndSpinBox{};
    QSpinBox mEmitRateSpinBox{};

    // Termination
    QCheckBox mIsStopEmitCheckBox{};
    QDoubleSpinBox mAlphaAddInSpinBox{};

private:
    static constexpr s32 sLifeInfinite = std::numeric_limits<s32>::max();    
    static constexpr s32 sEmitInfinite = std::numeric_limits<s32>::max();
};


// ========================================================================== //


} // namespace PtclEditor
