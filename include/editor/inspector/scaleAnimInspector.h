#pragma once

#include "editor/components/animGraph.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ScaleAnimInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit ScaleAnimInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

    void updateAnimPoint(s32 pointIndex, const AnimGraph::GraphPoint& point, f32 (Math::Vector2f::*get)() const);
    void updateGraphs();

protected:
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    AnimGraph mGraphX{};
    AnimGraph mGraphY{};

    QDoubleSpinBox mRandSpinbox{};
    const Ptcl::Emitter* mLastEmitter = nullptr;
    QWidget* mOverlay = nullptr;
};


// ========================================================================== //


} // namespace PtclEditor
