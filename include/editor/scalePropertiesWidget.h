#pragma once

#include "ptcl/ptclEmitter.h"

#include "editor/animGraph.h"

#include <QWidget>


// ========================================================================== //

class scalePropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit scalePropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::ScaleProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::ScaleProperties& properties);

private:
    void updateAnimPoint(s32 pointIndex, const GraphPoint& point, f32 (QVector2D::*get)() const);
    void updateGraphs();

private:
    Ptcl::ScaleProperties mProps{};

    AnimGraph mGraphX{};
    AnimGraph mGraphY{};

    QDoubleSpinBox mRandSpinbox{};

};

// ========================================================================== //
