#pragma once

#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildEmissionInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit ChildEmissionInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    QSpinBox mEmitRateSpinBox{};
    QSpinBox mEmitTimingSpinBox{};
    QSpinBox mLifeSpinBox{};
    QSpinBox mEmitStepSpinBox{};
    QCheckBox mInfiniteLifeCheckBox{};

    static constexpr s32 sLifeInfinite = std::numeric_limits<s32>::max();
};


// ========================================================================== //


} // namespace PtclEditor
