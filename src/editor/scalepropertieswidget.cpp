#include "editor/scalepropertieswidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

scalePropertiesWidget::scalePropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    // TODO: Move these somewhere else
    static constexpr QColor sColorAxisX = { 238, 51, 79 };
    static constexpr QColor sColorAxisY = { 42, 125, 212 };
    static constexpr QColor sColorAxisZ = { 137, 214, 1 };

    mGraphX.setLineColor(sColorAxisX);
    mGraphY.setLineColor(sColorAxisY);

    // --- Layout Setup ---

    // Main vertical layout
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(5);

    // X graph row: label + graph
    auto* xRow = new QHBoxLayout();
    auto* xLabel = new QLabel("X:", this);
    xLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    xRow->addWidget(xLabel);
    xRow->addWidget(&mGraphX, 1); // stretch factor 1 to fill remaining space
    mainLayout->addLayout(xRow);

    // Y graph row: label + graph
    auto* yRow = new QHBoxLayout();
    auto* yLabel = new QLabel("Y:", this);
    yLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    yRow->addWidget(yLabel);
    yRow->addWidget(&mGraphY, 1); // stretch factor 1 to fill remaining space
    mainLayout->addLayout(yRow);
}

void scalePropertiesWidget::setAnim(const Ptcl::ScaleAnim& scaleAnim) {
    mAnim = scaleAnim;

    // Update X Graph
    AnimGraph::PointList xPoints = {
        { 0.0f, mAnim.initScale.x() },
        { static_cast<f32>(mAnim.scaleSection1), mAnim.initScale.x() + mAnim.diffScale21.x() },
        { static_cast<f32>(mAnim.scaleSection2), mAnim.initScale.x() + mAnim.diffScale21.x() },
        { 100.0f, mAnim.initScale.x() + mAnim.diffScale21.x() + mAnim.diffScale32.x() }
    };
    mGraphX.setControlPoints(xPoints);

    // Update Y Graph
    AnimGraph::PointList yPoints = {
       { 0.0f, mAnim.initScale.y() },
       { static_cast<f32>(mAnim.scaleSection1), mAnim.initScale.y() + mAnim.diffScale21.y() },
       { static_cast<f32>(mAnim.scaleSection2), mAnim.initScale.y() + mAnim.diffScale21.y() },
       { 100.0f, mAnim.initScale.y() + mAnim.diffScale21.y() + mAnim.diffScale32.y() }
    };
    mGraphY.setControlPoints(yPoints);

    update();
}
