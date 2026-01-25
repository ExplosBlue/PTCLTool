#include "editor/alphaPropertiesWidget.h"

#include <QFormLayout>


// ========================================================================== //


AlphaPropertiesWidget::AlphaPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    mGraphA.setLineColor(QColor{20, 40, 60});
    mGraphA.setTickStepSize(0.1f);
    mGraphA.setValueSnap(0.1f);
    mGraphA.setValueRange(0.0f, 1.0f);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Alpha Anim:", &mGraphA);

    connect(&mGraphA, &AnimGraph::pointEdited, this, [this](s32 pointIndex, const AnimGraph::GraphPoint& point) {
        const f32 oldP0 = mProps.initAlpha;
        const f32 oldP1 = oldP0 + mProps.diffAlpha21;
        const f32 oldP2 = oldP1;
        const f32 oldP3 = oldP2 + mProps.diffAlpha32;

        auto updateAlphaSection = [&](s32* section, bool isSection2 = false) {
            mProps.diffAlpha21 = point.value - oldP0;
            mProps.diffAlpha32 = oldP3 - point.value;

            const f32 sec1 = mGraphA.getPoints()[1].position;
            const f32 sec2 = mGraphA.getPoints()[2].position;

            if (std::abs(sec1 - sec2) < std::numeric_limits<f32>::epsilon()) {
                mProps.alphaSection1 = -127; // Disable section1 if handles overlap
                if (isSection2) { *section = static_cast<s32>(point.position); }
            } else {
                *section = (std::abs(sec2 - 100.0f) < std::numeric_limits<f32>::epsilon()) ? 128 : static_cast<s32>(point.position);
            }
        };

        switch (pointIndex) {
        case 0:
            mProps.initAlpha = point.value;
            mProps.diffAlpha21 = oldP1 - point.value;
            break;
        case 1:
            updateAlphaSection(&mProps.alphaSection1);
            break;
        case 2:
            updateAlphaSection(&mProps.alphaSection2, true);
            break;
        case 3:
            mProps.diffAlpha32 = point.value - oldP1;
            break;
        }

        emit propertiesUpdated(mProps);
    });
}

void AlphaPropertiesWidget::setProperties(const Ptcl::Emitter::AlphaProperties& properties) {
    mProps = properties;

    QSignalBlocker blocker(mGraphA);

    const f32 p0 = mProps.initAlpha;
    const f32 p1 = p0 + mProps.diffAlpha21;
    const f32 p2 = p1;
    const f32 p3 = p2 + mProps.diffAlpha32;

    const bool disabled = mProps.alphaSection1 == -127;
    const bool fullLife = mProps.alphaSection2 == 128;

    const f32 sec2 = fullLife ? 100.0f : static_cast<f32>(mProps.alphaSection2);
    const f32 sec1 = disabled ? sec2 : static_cast<f32>(mProps.alphaSection1);

    AnimGraph::PointList points = {
        { 0.0f, p0, AnimGraph::HandleType::Locked },
        { sec1, p1, AnimGraph::HandleType::HoldStart },
        { sec2, p2, AnimGraph::HandleType::HoldEnd },
        { 100.0f, p3, AnimGraph::HandleType::Locked }
    };
    mGraphA.setControlPoints(points);

    update();
}


// ========================================================================== //
