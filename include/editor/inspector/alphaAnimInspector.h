#pragma once

#include "editor/components/animGraph.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class AlphaAnimInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit AlphaAnimInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void updateAnimPoint(s32 pointIndex, const AnimGraph::GraphPoint& point);

private:
    AnimGraph mGraphA{};
};


// ========================================================================== //


} // namespace PtclEditor
