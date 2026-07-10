#include "editor/inspector/scaleAnimInspector.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
#include <QVBoxLayout>


namespace PtclEditor {


// ========================================================================== //


ScaleAnimInspector::ScaleAnimInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    // TODO: Move these somewhere else
    static constexpr QColor sColorAxisX = { 238, 51, 79 };
    static constexpr QColor sColorAxisY = { 42, 125, 212 };

    mGraphX.setLineColor(sColorAxisX);
    mGraphX.setVerticalAxisLabel("Scale X");
    mGraphX.setPosLabel("Life");
    mGraphX.setValLabel("Scale");
    mGraphY.setLineColor(sColorAxisY);
    mGraphY.setVerticalAxisLabel("Scale Y");
    mGraphY.setPosLabel("Life");
    mGraphY.setValLabel("Scale");

    // Force identical left padding so content rects align and
    // connector lines drawn at the same percentage position match.
    constexpr s32 forcedPad = 48;
    mGraphX.setFixedLeftPadding(forcedPad);
    mGraphY.setFixedLeftPadding(forcedPad);

    auto* mainLayout = new QVBoxLayout(this);
    // mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    auto* header = new QLabel("Scale Animation", this);
    header->setStyleSheet("font-weight: bold; padding: 4px 0;");
    mainLayout->addWidget(header);
    mainLayout->addWidget(&mGraphX);
    mainLayout->addWidget(&mGraphY);
    auto* randLayout = new QHBoxLayout;
    randLayout->addWidget(new QLabel("Scale Random:", this));
    randLayout->addWidget(&mRandSpinbox);
    mainLayout->addLayout(randLayout);

    // TODO: Check this is valid
    mRandSpinbox.setRange(0.0f, 1.0f);

    mOverlay = new QWidget(this);
    mOverlay->setAttribute(Qt::WA_TransparentForMouseEvents);
    mOverlay->setAttribute(Qt::WA_TranslucentBackground);
    mOverlay->setAttribute(Qt::WA_NoSystemBackground);
    mOverlay->installEventFilter(this);
    mOverlay->setGeometry(rect());
    mOverlay->show();
    mOverlay->raise();

    setupConnections();
}

void ScaleAnimInspector::resizeEvent(QResizeEvent* event) {
    InspectorWidgetBase::resizeEvent(event);
    mOverlay->setGeometry(rect());
}

bool ScaleAnimInspector::eventFilter(QObject* obj, QEvent* event) {
    if (obj == mOverlay && event->type() == QEvent::Paint) {
        QPainter p(mOverlay);
        const auto pal = mOverlay->palette();
        p.setPen(QPen(pal.color(QPalette::WindowText), 1, Qt::DashLine));

        auto drawLine = [&](s32 idx) {
            if (idx >= static_cast<s32>(mGraphX.getPoints().size()) ||
                idx >= static_cast<s32>(mGraphY.getPoints().size())) {
                return;
            }
            const QPointF px = mGraphX.handleVisualPos(idx);
            const QPointF py = mGraphY.handleVisualPos(idx);
            const QPoint sx = mOverlay->mapFromGlobal(mGraphX.mapToGlobal(px.toPoint()));
            const QPoint sy = mOverlay->mapFromGlobal(mGraphY.mapToGlobal(py.toPoint()));
            p.drawLine(sx, sy);
        };
        drawLine(1);
        drawLine(2);

        return true;
    }
    return InspectorWidgetBase::eventFilter(obj, event);
}

void ScaleAnimInspector::setupConnections() {
    connect(&mGraphX, &AnimGraph::pointEdited, this, [this](s32 pointIndex, const AnimGraph::GraphPoint& point) {
        updateAnimPoint(pointIndex, point, &Math::Vector2f::getX);
    });

    connect(&mGraphY, &AnimGraph::pointEdited, this, [this](s32 pointIndex, const AnimGraph::GraphPoint& point) {
        updateAnimPoint(pointIndex, point, &Math::Vector2f::getY);
    });

    connect(&mRandSpinbox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Scale Random",
            "SetScaleRand",
            &Ptcl::Emitter::scaleRand,
            &Ptcl::Emitter::setScaleRand,
            static_cast<f32>(value)
         );
    });
}

void ScaleAnimInspector::updateAnimPoint(s32 pointIndex, const AnimGraph::GraphPoint& point, f32 (Math::Vector2f::*get)() const) {
    auto anim = mEmitter->scaleAnim();

    auto set = [&](Math::Vector2f& v, f32 val) {
        if (get == &Math::Vector2f::getX) {
            v.setX(val);
        } else {
            v.setY(val);
        }
    };

    auto getGraphPoints = [&]() {
        return (get == &Math::Vector2f::getX) ?
            mGraphX.getPoints() :
            mGraphY.getPoints();
    };

    const f32 oldP0 = (anim.initScale.*get)();
    const f32 oldP1 = oldP0 + (anim.diffScale21.*get)();
    const f32 oldP2 = oldP1;
    const f32 oldP3 = oldP2 + (anim.diffScale32.*get)();

    auto updateScaleSection = [&](s32* section) {
        set(anim.diffScale21, point.value - oldP0);
        set(anim.diffScale32, oldP3 - point.value);

        *section = static_cast<s32>(point.position);
    };

    switch (pointIndex) {
    case 0:
        set(anim.initScale, point.value);
        set(anim.diffScale21, oldP1 - point.value);
        break;
    case 1:
        updateScaleSection(&anim.scaleSection1);
        anim.scaleSection2 = static_cast<s32>(getGraphPoints()[2].position);
        break;
    case 2:
        updateScaleSection(&anim.scaleSection2);
        anim.scaleSection1 = static_cast<s32>(getGraphPoints()[1].position);
        break;
    case 3:
        set(anim.diffScale32, point.value - oldP1);
        break;
    }

    const f32 newP0 = (anim.initScale.*get)();
    const f32 newP1 = newP0 + (anim.diffScale21.*get)();
    anim.isFlatStart = (newP0 == newP1);

    const QString axis = (get == &Math::Vector2f::getX) ? "X" : "Y";

    QString handleName;
    switch (pointIndex) {
        case 0: handleName = "Start"; break;
        case 1: handleName = "Section1"; break;
        case 2: handleName = "Section2"; break;
        case 3: handleName = "End"; break;
    }

    QString key;
    QString label;
    if (pointIndex == 1 || pointIndex == 2) {
        label = QString("Move Scale %1").arg(handleName);
        key = QString("ScaleGraph_%1").arg(pointIndex);
    } else {
        label = QString("Move Scale %1 (%2)").arg(handleName, axis);
        key = QString("ScaleAnim_%1_%2").arg(pointIndex).arg(axis);
    }

    setEmitterProperty(
        label,
        key,
        &Ptcl::Emitter::scaleAnim,
        &Ptcl::Emitter::setScaleAnim,
        anim
    );

    updateGraphs();
}

void ScaleAnimInspector::updateGraphs() {
    const auto& anim = mEmitter->scaleAnim();

    const f32 sec1 = static_cast<f32>(anim.scaleSection1);
    const f32 sec2 = static_cast<f32>(anim.scaleSection2);

    auto updateGraph = [&](AnimGraph& graph, f32 (Math::Vector2f::*get)() const) {
        QSignalBlocker blocker(graph);

        const f32 p0 = (anim.initScale.*get)();
        const f32 p1 = p0 + (anim.diffScale21.*get)();
        const f32 p2 = p1;
        const f32 p3 = p2 + (anim.diffScale32.*get)();

        AnimGraph::PointList points = {
            { 0.0f, p0, AnimGraph::HandleType::Locked },
            { sec1, p1, AnimGraph::HandleType::HoldStart },
            { sec2, p2, AnimGraph::HandleType::HoldEnd },
            { 100.0f, p3, AnimGraph::HandleType::Locked }
        };
        graph.setControlPoints(points);
    };

    updateGraph(mGraphX, &Math::Vector2f::getX);
    updateGraph(mGraphY, &Math::Vector2f::getY);

    mOverlay->update();
}

void ScaleAnimInspector::populateProperties() {
    const bool emitterChanged = (mEmitter != mLastEmitter);
    mLastEmitter = mEmitter;

    updateGraphs();

    if (emitterChanged) {
        mGraphX.zoomToFit();
        mGraphY.zoomToFit();
    }

    QSignalBlocker blockerRand(mRandSpinbox);
    mRandSpinbox.setValue(mEmitter->scaleRand());

    update();
}


// ========================================================================== //


} // namespace PtclEditor
