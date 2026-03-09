#pragma once

#include "editor/components/animGraph.h"
#include "editor/emitterWidget/emitterWidgetBase.h"

#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ScalePropertiesWidget final : public EmitterWidgetBase {
    Q_OBJECT
public:
    explicit ScalePropertiesWidget(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

    void updateAnimPoint(s32 pointIndex, const AnimGraph::GraphPoint& point, f32 (Math::Vector2f::*get)() const);
    void updateGraphs();

private:
    AnimGraph mGraphX{};
    AnimGraph mGraphY{};

    QDoubleSpinBox mRandSpinbox{};
};


// ========================================================================== //


} // namespace PtclEditor
