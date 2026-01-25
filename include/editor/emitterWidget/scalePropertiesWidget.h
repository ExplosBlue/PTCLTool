#pragma once

#include "editor/components/animGraph.h"
#include "editor/emitterWidget/emitterWidget.h"

#include "ptcl/ptclEmitter.h"

#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterWidget::ScalePropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit ScalePropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::Emitter::ScaleProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::Emitter::ScaleProperties& properties);

private:
    void updateAnimPoint(s32 pointIndex, const AnimGraph::GraphPoint& point, f32 (Math::Vector2f::*get)() const);
    void updateGraphs();

private:
    Ptcl::Emitter::ScaleProperties mProps{};

    AnimGraph mGraphX{};
    AnimGraph mGraphY{};

    QDoubleSpinBox mRandSpinbox{};

};


// ========================================================================== //


} // namespace PtclEditor
