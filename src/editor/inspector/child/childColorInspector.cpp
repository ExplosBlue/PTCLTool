#include "editor/inspector/child/childColorInspector.h"

#include <QFormLayout>

#include <cmath>


namespace PtclEditor {


// ==========================================================================//


ChildColorInspector::ChildColorInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    // Color Section
    addSectionHeader(mainLayout, "Color", this);

    mInheritColorCheckBox.setText("Inherit Parent Color");
    mInheritColorCheckBox.setToolTip(
        "When enabled, child particles inherit the parent particle's current "
        "computed color instead of using the Secondary Color value set below."
    );
    mInheritColorCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mColor0Widget.setToolTip("Base RGBA color (0.0-1.0) applied to child particles.");
    mColor1Widget.enableAlpha(false);

    mainLayout->addRow("Color:", &mInheritColorCheckBox);
    mainLayout->addRow("Primary Color:", &mColor1Widget);
    mainLayout->addRow("Secondary Color:", &mColor0Widget);

    // Alpha Section
    addSectionHeader(mainLayout, "Alpha", this);

    mAlphaGraph.setLineColor(QColor{137, 214, 1});
    mAlphaGraph.setTickStepSize(0.1f);
    mAlphaGraph.setValueBounds(0.0f, 1.0f);
    mAlphaGraph.setValueRange(0.0f, 1.0f);
    mAlphaGraph.setVerticalAxisLabel("Alpha");
    mAlphaGraph.setPosLabel("Life");
    mAlphaGraph.setValLabel("Alpha");

    mInheritAlphaCheckBox.setText("Inherit Parent Alpha");
    mInheritAlphaCheckBox.setToolTip(
        "When enabled, the hold alpha value is overridden by the parent particle's current alpha at spawn time.");
    mInheritAlphaCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    mainLayout->addRow("Alpha:", &mInheritAlphaCheckBox);
    mainLayout->addRow(&mAlphaGraph);

    setLayout(mainLayout);
    setupConnections();
}

void ChildColorInspector::setupConnections() {
    // Inherit Color
    connect(&mInheritColorCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        mColor0Widget.setDisabled(checked);

        setEmitterProperty(
            "Toggle Child Inherit Parent Color",
            "ToggleChildInheritColor",
            &Ptcl::Emitter::isChildInheritParentColor,
            &Ptcl::Emitter::setChildInheritParentColor,
            checked
        );
    });

    // Color 0
    connect(&mColor0Widget, &RGBAColorWidget::colorChanged, this, [this]() {
        const auto newColor = mColor0Widget.color();
        setEmitterProperty(
            "Set Child Secondary Color",
            "SetChildSecondaryColor",
            &Ptcl::Emitter::childSecondaryColor,
            &Ptcl::Emitter::setChildSecondaryColor,
            newColor
        );
    });

    // Color 1
    connect(&mColor1Widget, &RGBAColorWidget::colorChanged, this, [this]() {
        const auto color = mColor1Widget.color();
        setEmitterProperty(
            "Set Child Primary Color",
            "SetChildPrimaryColor",
            &Ptcl::Emitter::childPrimaryColor,
            &Ptcl::Emitter::setChildPrimaryColor,
            color
        );
    });

    // Inherit Alpha
    connect(&mInheritAlphaCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Child Inherit Alpha",
            "ToggleChildInheritAlpha",
            &Ptcl::Emitter::isChildInheritAlpha,
            &Ptcl::Emitter::setChildInheritAlpha,
            checked
        );
        updateAlphaGraph();
    });

    // Alpha Graph
    connect(&mAlphaGraph, &AnimGraph::pointEdited, this, &ChildColorInspector::handleAlphaPointEdited);
}

void ChildColorInspector::populateProperties() {
    QSignalBlocker b1(mColor0Widget);
    QSignalBlocker b2(mColor1Widget);
    QSignalBlocker b3(mInheritColorCheckBox);
    QSignalBlocker b4(mAlphaGraph);
    QSignalBlocker b5(mInheritAlphaCheckBox);

    const bool inheritParentColor = mEmitter->isChildInheritParentColor();
    const bool inheritParentAlpha = mEmitter->isChildInheritAlpha();

    mColor0Widget.setColor(mEmitter->childSecondaryColor());
    mColor0Widget.setDisabled(inheritParentColor);

    mColor1Widget.setColor(mEmitter->childPrimaryColor());
    mInheritColorCheckBox.setChecked(inheritParentColor);

    updateAlphaGraph();

    mInheritAlphaCheckBox.setChecked(inheritParentAlpha);
}

void ChildColorInspector::updateAlphaGraph() {
    const s32 childLife = mEmitter->childLife();
    const f32 initAlpha = mEmitter->childAlphaInit();
    const f32 alpha = mEmitter->childAlpha();
    const f32 targetAlpha = mEmitter->childAlphaTarget();
    const s32 baseFrame = mEmitter->childAlphaBaseFrame();
    const s32 startFrame = mEmitter->childAlphaStartFrame();
    const bool inherited = mEmitter->isChildInheritAlpha();

    const f32 baseFramePct = (childLife > 0)
        ? (static_cast<f32>(baseFrame) / static_cast<f32>(childLife)) * 100.0f
        : 0.0f;
    const f32 startFramePct = (childLife > 0)
        ? (static_cast<f32>(startFrame) / static_cast<f32>(childLife)) * 100.0f
        : 100.0f;

    const auto holdType = inherited
        ? AnimGraph::HandleType::InheritedHoldStart
        : AnimGraph::HandleType::HoldStart;

    const f32 holdAlpha = inherited ? 1.0f : alpha;

    AnimGraph::PointList points = {
        { 0.0f,          initAlpha, AnimGraph::HandleType::Locked },
        { baseFramePct,  holdAlpha, holdType },
        { startFramePct, holdAlpha, inherited ? AnimGraph::HandleType::InheritedHoldEnd : AnimGraph::HandleType::HoldEnd },
        { 100.0f,        targetAlpha, AnimGraph::HandleType::Locked }
    };

    mAlphaGraph.setValueRange(0.0f, 1.0f);
    mAlphaGraph.setValueBounds(0.0f, 1.0f);
    mAlphaGraph.setValueDisplayScale(inherited ? 100.0f : 1.0f);
    mAlphaGraph.setVerticalAxisLabel(inherited ? "Parent Alpha (%)" : "Alpha");

    mAlphaGraph.setControlPoints(points);

    if (inherited) {
        mAlphaGraph.setHandleTooltips({
            "Alpha Fade In.\nLocked to 0% lifetime.",
            "Alpha Hold Start.\nValue is locked to 100% Parent Alpha.",
            "Alpha Hold End.\nValue is locked to 100% Parent Alpha.",
            "Alpha Fade Out.\nLocked to 100% lifetime.",
        });
    } else {
        mAlphaGraph.setHandleTooltips({
            "Alpha Fade In.\nLocked to 0% lifetime.",
            "Alpha Hold Start.\nValue is synced with the Alpha Hold End handle.",
            "Alpha Hold End.\nValue is synced with the Alpha Hold Start handle.",
            "Alpha Fade Out.\nLocked to 100% lifetime.",
        });
    }
}

void ChildColorInspector::handleAlphaPointEdited(s32 pointIndex, const AnimGraph::GraphPoint& point) {
    const s32 childLife = mEmitter->childLife();

    const auto& pts = mAlphaGraph.getPoints();
    const s32 graphBaseFrame = (childLife > 0)
        ? static_cast<s32>(lround((pts[1].position / 100.0) * static_cast<f64>(childLife)))
        : 0;
    const s32 graphStartFrame = (childLife > 0)
        ? static_cast<s32>(lround((pts[2].position / 100.0) * static_cast<f64>(childLife)))
        : 0;

    auto state = mEmitter->childAlphaState();

    const QString key = QString("ChildAlpha_%1").arg(pointIndex);
    QString label;
    switch (pointIndex) {
    case 0:
        state.initAlpha = point.value;
        label = "Move Alpha Fade In";
        break;
    case 1:
        state.alpha = point.value;
        state.baseFrame = graphBaseFrame;
        state.startFrame = graphStartFrame;
        label = "Move Alpha Hold Start";
        break;
    case 2:
        state.alpha = point.value;
        state.baseFrame = graphBaseFrame;
        state.startFrame = graphStartFrame;
        label = "Move Alpha Hold End";
        break;
    case 3:
        state.targetAlpha = point.value;
        label = "Move Alpha Fade Out";
        break;
    }

    setEmitterProperty(
        label,
        key,
        &Ptcl::Emitter::childAlphaState,
        &Ptcl::Emitter::setChildAlphaState,
        state
    );
}


// ==========================================================================//


} // namespace PtclEditor
