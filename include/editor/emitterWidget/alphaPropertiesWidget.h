#pragma once

#include "editor/components/animGraph.h"
#include "editor/emitterWidget/emitterWidgetBase.h"

#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class AlphaPropertiesWidget final : public EmitterWidgetBase {
    Q_OBJECT
public:
    explicit AlphaPropertiesWidget(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void updateAnimPoint(s32 pointIndex, const AnimGraph::GraphPoint& point);

private:
    AnimGraph mGraphA{};
};


// ========================================================================== //


} // namespace PtclEditor
