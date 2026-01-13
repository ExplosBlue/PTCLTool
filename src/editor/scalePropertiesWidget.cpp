#include "editor/scalePropertiesWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>

scalePropertiesWidget::scalePropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    // TODO: Move these somewhere else
    static constexpr QColor sColorAxisX = { 238, 51, 79 };
    static constexpr QColor sColorAxisY = { 42, 125, 212 };
    static constexpr QColor sColorAxisZ = { 137, 214, 1 };

    mGraphX.setLineColor(sColorAxisX);
    mGraphY.setLineColor(sColorAxisY);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("X Scale Anim:", &mGraphX);
    mainLayout->addRow("Y Scale Anim:", &mGraphY);
    mainLayout->addRow("Scale Random:", &mRandSpinbox);

    connect(&mGraphX, &AnimGraph::pointEdited, this, [this](s32 pointIndex, const AnimGraph::GraphPoint& point) {
        updateAnimPoint(pointIndex, point, &QVector2D::x);
    });

    connect(&mGraphY, &AnimGraph::pointEdited, this, [this](s32 pointIndex, const AnimGraph::GraphPoint& point) {
        updateAnimPoint(pointIndex, point, &QVector2D::y);
    });

}

void scalePropertiesWidget::updateAnimPoint(s32 pointIndex, const AnimGraph::GraphPoint& point, f32 (QVector2D::*get)() const) {
    auto set = [&](QVector2D& v, f32 val) {
        if (get == &QVector2D::x) {
            v.setX(val);
        } else {
            v.setY(val);
        }
    };

    auto getGraphPoints = [&]() {
        return (get == &QVector2D::x) ?
            mGraphX.getPoints() :
            mGraphY.getPoints();
    };

    const f32 oldP0 = (mProps.initScale.*get)();
    const f32 oldP1 = oldP0 + (mProps.diffScale21.*get)();
    const f32 oldP2 = oldP1;
    const f32 oldP3 = oldP2 + (mProps.diffScale32.*get)();

    switch (pointIndex) {
    case 0:
        set(mProps.initScale, point.value);
        set(mProps.diffScale21, oldP1 - point.value);
        break;
    case 1:
    {
        set(mProps.diffScale21, point.value - oldP0);

        const f32 sec1 = getGraphPoints()[1].position;
        const f32 sec2 = getGraphPoints()[2].position;

        // Disable if handles overlap
        if (std::abs(sec1 - sec2) < std::numeric_limits<f32>::epsilon()) {
            mProps.scaleSection1 = -127;
        } else {
            mProps.scaleSection1 = static_cast<s32>(point.position);
        }
        break;
    }
    case 2:
    {
        set(mProps.diffScale21, point.value - oldP0);
        set(mProps.diffScale32, oldP3 - point.value);

        const f32 sec1 = getGraphPoints()[1].position;
        const f32 sec2 = getGraphPoints()[2].position;

        // Disable if handles overlap
        if (std::abs(sec1 - sec2) < std::numeric_limits<f32>::epsilon()) {
            mProps.scaleSection1 = -127;
            mProps.scaleSection2 = static_cast<s32>(point.position);
        } else {
            mProps.scaleSection2 = static_cast<s32>(point.position);
        }
        break;
    }
    case 3:
        set(mProps.diffScale32, point.value - oldP1);
        break;
    }

    emit propertiesUpdated(mProps);
    updateGraphs();
}

void scalePropertiesWidget::updateGraphs() {
    auto updateGraph = [this](AnimGraph& graph, f32 (QVector2D::*get)() const) {
        QSignalBlocker blocker(graph);

        const f32 p0 = (mProps.initScale.*get)();
        const f32 p1 = p0 + (mProps.diffScale21.*get)();
        const f32 p2 = p1;
        const f32 p3 = p2 + (mProps.diffScale32.*get)();

        const bool disabled = mProps.scaleSection1 == -127;

        const f32 sec1 = disabled ? static_cast<f32>(mProps.scaleSection2) : static_cast<f32>(mProps.scaleSection1);
        const f32 sec2 = static_cast<f32>(mProps.scaleSection2);

        AnimGraph::PointList points = {
            { 0.0f, p0, AnimGraph::HandleType::Locked },
            { sec1, p1, AnimGraph::HandleType::HoldStart },
            { sec2, p2, AnimGraph::HandleType::HoldEnd },
            { 100.0f, p3, AnimGraph::HandleType::Locked }
        };
        graph.setControlPoints(points);
    };

    updateGraph(mGraphX, &QVector2D::x);
    updateGraph(mGraphY, &QVector2D::y);
}

void scalePropertiesWidget::setProperties(const Ptcl::ScaleProperties& properties) {
    mProps = properties;

    updateGraphs();

    QSignalBlocker blockerRand(mRandSpinbox);
    mRandSpinbox.setValue(mProps.scaleRand);

    update();
}
