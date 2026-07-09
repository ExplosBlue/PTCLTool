#include "editor/inspector/alphaAnimInspector.h"

#include <QLabel>
#include <QVBoxLayout>


namespace PtclEditor {


// ========================================================================== //


AlphaAnimInspector::AlphaAnimInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    mGraphA.setLineColor(QColor{137, 214, 1});
    mGraphA.setTickStepSize(0.1f);
    mGraphA.setValueBounds(0.0f, 1.0f);
    mGraphA.setValueRange(0.0f, 1.0f);
    mGraphA.setVerticalAxisLabel("Alpha");
    mGraphA.setPosLabel("Life");
    mGraphA.setValLabel("Alpha");

    auto* mainLayout = new QVBoxLayout(this);
    auto* header = new QLabel("Alpha Animation", this);
    header->setStyleSheet("font-weight: bold;");
    mainLayout->addWidget(header);
    mainLayout->addWidget(&mGraphA);

    connect(&mGraphA, &AnimGraph::pointEdited, this, [this](s32 pointIndex, const AnimGraph::GraphPoint& point) {
        updateAnimPoint(pointIndex, point);
    });
}

void AlphaAnimInspector::updateAnimPoint(s32 pointIndex, const AnimGraph::GraphPoint& point) {
    auto anim = mEmitter->alphaAnim();

    const f32 oldP0 = anim.initAlpha;
    const f32 oldP1 = oldP0 + anim.diffAlpha21;
    const f32 oldP2 = oldP1;
    const f32 oldP3 = oldP2 + anim.diffAlpha32;

    auto updateAlphaSection = [&](s32* section) {
        anim.diffAlpha21 = point.value - oldP0;
        anim.diffAlpha32 = oldP3 - point.value;

        const f32 sec1 = mGraphA.getPoints()[1].position;
        const f32 sec2 = mGraphA.getPoints()[2].position;

        *section = (std::abs(sec2 - 100.0f) < std::numeric_limits<f32>::epsilon()) ? 128 : static_cast<s32>(point.position);
    };

    switch (pointIndex) {
    case 0:
        anim.initAlpha = point.value;
        anim.diffAlpha21 = oldP1 - point.value;
        break;
    case 1:
        updateAlphaSection(&anim.alphaSection1);
        {
            const f32 sec2Pos = mGraphA.getPoints()[2].position;
            anim.alphaSection2 = (std::abs(sec2Pos - 100.0f) < std::numeric_limits<f32>::epsilon())
                ? 128 : static_cast<s32>(sec2Pos);
        }
        break;
    case 2:
        updateAlphaSection(&anim.alphaSection2);
        anim.alphaSection1 = static_cast<s32>(mGraphA.getPoints()[1].position);
        break;
    case 3:
        anim.diffAlpha32 = point.value - oldP1;
        break;
    }

    const f32 newP0 = anim.initAlpha;
    const f32 newP1 = newP0 + anim.diffAlpha21;
    anim.isFlatStart = (newP0 == newP1);

    QString handleName;
    switch (pointIndex) {
    case 0: handleName = "Start"; break;
    case 1: handleName = "Section1"; break;
    case 2: handleName = "Section2"; break;
    case 3: handleName = "End"; break;
    }

    const auto label = QString("Move Alpha %1").arg(handleName);
    const auto key = QString("AlphaGraph_%1").arg(pointIndex);

    setEmitterProperty(
        label,
        key,
        &Ptcl::Emitter::alphaAnim,
        &Ptcl::Emitter::setAlphaAnim,
        anim
    );
}

void AlphaAnimInspector::populateProperties() {
    const bool emitterChanged = (mEmitter != mLastEmitter);
    mLastEmitter = mEmitter;

    QSignalBlocker blocker(mGraphA);

    const auto& anim = mEmitter->alphaAnim();

    const f32 p0 = anim.initAlpha;
    const f32 p1 = p0 + anim.diffAlpha21;
    const f32 p2 = p1;
    const f32 p3 = p2 + anim.diffAlpha32;

    const bool fullLife = anim.alphaSection2 == 128;

    const f32 sec1 = static_cast<f32>(anim.alphaSection1);
    const f32 sec2 = fullLife ? 100.0f : static_cast<f32>(anim.alphaSection2);

    AnimGraph::PointList points = {
        { 0.0f, p0, AnimGraph::HandleType::Locked },
        { sec1, p1, AnimGraph::HandleType::HoldStart },
        { sec2, p2, AnimGraph::HandleType::HoldEnd },
        { 100.0f, p3, AnimGraph::HandleType::Locked }
    };
    mGraphA.setControlPoints(points);

    if (emitterChanged) {
        mGraphA.zoomToFit();
    }

    update();
}


// ========================================================================== //


} // namespace PtclEditor
