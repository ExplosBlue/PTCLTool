#pragma once

#include "editor/components/animGraph.h"
#include "editor/components/rgbaColorWidget.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QWidget>


namespace PtclEditor {


// ==========================================================================//


class ChildColorInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit ChildColorInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();
    void updateAlphaGraph();
    void handleAlphaPointEdited(s32 pointIndex, const AnimGraph::GraphPoint& point);

private:
    RGBAColorWidget mColor0Widget{};
    RGBAColorWidget mColor1Widget{};
    QCheckBox mInheritColorCheckBox{};

    AnimGraph mAlphaGraph{};
    QCheckBox mInheritAlphaCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
