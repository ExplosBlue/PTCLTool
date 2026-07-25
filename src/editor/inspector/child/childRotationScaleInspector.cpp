#include "editor/inspector/child/childRotationScaleInspector.h"

#include "math/util.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
#include <QVBoxLayout>

#include <cmath>


namespace PtclEditor {


// ========================================================================== //


static const std::array rotTypeOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::RotType>{ Ptcl::RotType::None,   "None",     "No rotation." },
    EnumOption<Ptcl::RotType>{ Ptcl::RotType::RotX,   "X-Axis",   "Rotates around the X-axis." },
    EnumOption<Ptcl::RotType>{ Ptcl::RotType::RotY,   "Y-Axis",   "Rotates around the Y-axis." },
    EnumOption<Ptcl::RotType>{ Ptcl::RotType::RotZ,   "Z-Axis",   "Rotates around the Z-axis." },
    EnumOption<Ptcl::RotType>{ Ptcl::RotType::RotXYZ, "XYZ-Axis", "Rotates around all three axes." },
};


ChildRotationScaleInspector::ChildRotationScaleInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

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

    constexpr s32 forcedPad = 56;
    mGraphX.setFixedLeftPadding(forcedPad);
    mGraphY.setFixedLeftPadding(forcedPad);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    // Rotation section
    addSectionHeader(mainLayout, "Rotation", this);

    mInheritRotCheckBox.setText("Inherit Parent Rotation");
    mInheritRotCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mainLayout->addRow("Inherit:", &mInheritRotCheckBox);
    mInheritRotCheckBox.setToolTip("Copies the parent's rotation.");

    mRotTypeComboBox.setOptions(rotTypeOptions);
    mRotTypeComboBox.setDescription("Which axis the particle rotates around.");
    mainLayout->addRow("Rotation Type:", &mRotTypeComboBox);
    mainLayout->addRow("Initial Rotation:", &mInitRotSpinBox);
    mInitRotSpinBox.setDecimals(2);
    mInitRotSpinBox.setSuffix(" °");
    mInitRotSpinBox.setToolTip("Starting rotation angle.");
    mainLayout->addRow("Initial Rotation Variation:", &mInitRotRandSpinBox);
    mInitRotRandSpinBox.setDecimals(2);
    mInitRotRandSpinBox.setSuffix(" °");
    mInitRotRandSpinBox.setToolTip("Random variation added to the starting rotation.");
    mainLayout->addRow("Rotation Speed:", &mRotVelSpinBox);
    mRotVelSpinBox.setDecimals(2);
    mRotVelSpinBox.setSuffix(" °");
    mRotVelSpinBox.setToolTip("How fast the particle rotates per frame.");
    mainLayout->addRow("Rotation Speed Variation:", &mRotVelRandSpinBox);
    mRotVelRandSpinBox.setDecimals(2);
    mRotVelRandSpinBox.setSuffix(" °");
    mRotVelRandSpinBox.setToolTip("Random variation added to the rotation speed.");
    mainLayout->addRow("Rotation Pivot Offset:", &mRotBasisSpinBox);
    mRotBasisSpinBox.setDecimals(2);
    mRotBasisSpinBox.setToolTip("Offset that shifts the center of rotation.");

    // Scale section
    addSectionHeader(mainLayout, "Scale", this);

    mInheritScaleCheckBox.setText("Inherit Parent Scale");
    mInheritScaleCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mainLayout->addRow("Inherit:", &mInheritScaleCheckBox);
    mInheritScaleCheckBox.setToolTip("When enabled, the initial scale is inherited from the parent particle's scale.");

    mInheritRateSpinBox.setRange(0.0, 100.0);
    mInheritRateSpinBox.setSuffix("%");
    mInheritRateSpinBox.setSingleStep(1.0);
    mInheritRateSpinBox.setDecimals(0);
    mainLayout->addRow("Inherit Rate:", &mInheritRateSpinBox);
    mInheritRateSpinBox.setToolTip("How much of the parent's scale is inherited.");

    mainLayout->addRow(&mGraphX);
    mainLayout->addRow(&mGraphY);

    // Position section
    addSectionHeader(mainLayout, "Position", this);

    mInitPosRandSpinBox.setRange(0.0, std::numeric_limits<f32>::max());
    mInitPosRandSpinBox.setDecimals(2);
    mInitPosRandSpinBox.setToolTip("Maximum distance from the parent particle at which each child can spawn.");
    mainLayout->addRow("Spawn Offset Variation:", &mInitPosRandSpinBox);

    // Overlay for connector lines between graphs
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

void ChildRotationScaleInspector::resizeEvent(QResizeEvent* event) {
    InspectorWidgetBase::resizeEvent(event);
    mOverlay->setGeometry(rect());
}

bool ChildRotationScaleInspector::eventFilter(QObject* obj, QEvent* event) {
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

        return true;
    }
    return InspectorWidgetBase::eventFilter(obj, event);
}

void ChildRotationScaleInspector::setupConnections() {
    auto deg2idxVec = [](const Math::Vector3f& v) {
        Math::Vector3i result {
            Math::Util::deg2idx(v.getX()),
            Math::Util::deg2idx(v.getY()),
            Math::Util::deg2idx(v.getZ())
        };
        return result;
    };

    // Rotation connections
    connect(&mInheritRotCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Child Inherit Rotation",
            "ToggleChildInheritRot",
            &Ptcl::Emitter::isChildInheritRotation,
            &Ptcl::Emitter::setChildInheritRotation,
            checked
        );
    });

    connect(&mRotTypeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mRotTypeComboBox.currentEnum();
        setEmitterProperty(
            "Set Child Rotation Type",
            "SetChildRotationType",
            &Ptcl::Emitter::childRotationType,
            &Ptcl::Emitter::setChildRotationType,
            type
        );
    });

    connect(&mInitRotSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto rot = deg2idxVec(mInitRotSpinBox.getVector());
        setEmitterProperty(
            "Set Child Initial Rotation",
            "SetChildInitRotation",
            &Ptcl::Emitter::childInitialRotation,
            &Ptcl::Emitter::setChildInitialRotation,
            rot
        );
    });

    connect(&mInitRotRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto rot = deg2idxVec(mInitRotRandSpinBox.getVector());
        setEmitterProperty(
            "Set Child Initial Rotation Variation",
            "SetChildInitRotationRand",
            &Ptcl::Emitter::childInitialRotationRandom,
            &Ptcl::Emitter::setChildInitialRotationRandom,
            rot
        );
    });

    connect(&mRotVelSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto speed = deg2idxVec(mRotVelSpinBox.getVector());
        setEmitterProperty(
            "Set Child Rotation Speed",
            "SetChildRotVel",
            &Ptcl::Emitter::childRotationVelocity,
            &Ptcl::Emitter::setChildRotationVelocity,
            speed
        );
    });

    connect(&mRotVelRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto vel = deg2idxVec(mRotVelRandSpinBox.getVector());
        setEmitterProperty(
            "Set Child Rotation Speed Variation",
            "SetChildRotVelRand",
            &Ptcl::Emitter::childRotationVelocityRandom,
            &Ptcl::Emitter::setChildRotationVelocityRandom,
            vel
        );
    });

    connect(&mRotBasisSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto pivot = mRotBasisSpinBox.getVector();
        setEmitterProperty(
            "Set Child Rotation Pivot",
            "SetChildRotBasis",
            &Ptcl::Emitter::childRotationBasis,
            &Ptcl::Emitter::setChildRotationBasis,
            pivot
        );
    });

    // Scale connections
    connect(&mInheritScaleCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        mInheritRateSpinBox.setEnabled(checked);
        setEmitterProperty(
            "Toggle Child Inherit Scale",
            "ToggleChildInheritScale",
            &Ptcl::Emitter::isChildInheritScale,
            &Ptcl::Emitter::setChildInheritScale,
            checked
        );
        updateGraphs();
    });

    connect(&mInheritRateSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Child Scale Inherit Rate",
            "SetChildScaleInheritRate",
            &Ptcl::Emitter::childScaleInheritRate,
            &Ptcl::Emitter::setChildScaleInheritRate,
            static_cast<f32>(value / 100.0)
        );
        updateGraphs();
    });

    connect(&mGraphX, &AnimGraph::pointEdited, this, [this](s32 pointIndex, const AnimGraph::GraphPoint& point) {
        updateAnimPoint(pointIndex, point, &Math::Vector2f::getX);
    });

    connect(&mGraphY, &AnimGraph::pointEdited, this, [this](s32 pointIndex, const AnimGraph::GraphPoint& point) {
        updateAnimPoint(pointIndex, point, &Math::Vector2f::getY);
    });

    // Position connections
    connect(&mInitPosRandSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Child Initial Position Random",
            "SetChildInitPosRand",
            &Ptcl::Emitter::childInitalPositionRand,
            &Ptcl::Emitter::setChildInitialPositionRand,
            static_cast<f32>(value)
        );
    });
}

void ChildRotationScaleInspector::updateAnimPoint(s32 pointIndex, const AnimGraph::GraphPoint& point, f32 (Math::Vector2f::*get)() const) {
    auto state = mEmitter->childScaleState();

    auto setVec = [&](Math::Vector2f& v, f32 val) {
        if (get == &Math::Vector2f::getX) {
            v.setX(val);
        } else {
            v.setY(val);
        }
    };

    auto getVec = [&](const Math::Vector2f& v) -> f32 {
        return (get == &Math::Vector2f::getX) ? v.getX() : v.getY();
    };

    const s32 childLife = mEmitter->childLife();
    const bool inherited = mEmitter->isChildInheritScale();

    switch (pointIndex) {
    case 0:
        if (!inherited) {
            setVec(state.scale, point.value);
        }
        break;
    case 1:
    {
        const f32 startFramePct = point.position;
        state.startFrame = (childLife > 0)
            ? static_cast<s32>(lround((startFramePct / 100.0) * static_cast<f64>(childLife)))
            : 0;
        if (!inherited) {
            setVec(state.scale, point.value);
        }
        break;
    }
    case 2:
    {
        if (inherited) {
            setVec(state.scaleTarget, point.value);
        } else {
            const f32 currentScale = getVec(state.scale);
            if (currentScale != 0.0f) {
                setVec(state.scaleTarget, point.value / currentScale);
            }
        }
        break;
    }
    }

    const QString axis = (get == &Math::Vector2f::getX) ? "X" : "Y";

    QString handleName;
    switch (pointIndex) {
        case 0: handleName = "Start"; break;
        case 1: handleName = "StartFrame"; break;
        case 2: handleName = "End"; break;
    }

    QString key;
    QString label;
    if (pointIndex == 1) {
        label = QString("Move Scale %1").arg(handleName);
        key = QString("ChildScale_%1").arg(pointIndex);
    } else {
        label = QString("Move Scale %1 (%2)").arg(handleName, axis);
        key = QString("ChildScale_%1_%2").arg(pointIndex).arg(axis);
    }

    setEmitterProperty(
        label,
        key,
        &Ptcl::Emitter::childScaleState,
        &Ptcl::Emitter::setChildScaleState,
        state
    );

    updateGraphs();
}

void ChildRotationScaleInspector::updateGraphs() {
    const s32 childLife = mEmitter->childLife();
    const bool inherited = mEmitter->isChildInheritScale();
    const Math::Vector2f& scale = mEmitter->childScale();
    const Math::Vector2f& scaleTarget = mEmitter->childScaleTarget();
    const s32 startFrame = mEmitter->childScaleStartFrame();

    const f32 startFramePct = (childLife > 0)
        ? (static_cast<f32>(startFrame) / static_cast<f32>(childLife)) * 100.0f
        : 0.0f;

    auto updateGraph = [&](AnimGraph& graph, f32 (Math::Vector2f::*get)() const) {
        QSignalBlocker blocker(graph);

        const f32 p0 = inherited ? 1.0f : (scale.*get)();
        const f32 p2 = inherited
            ? (scaleTarget.*get)()
            : p0 * (scaleTarget.*get)();

        const auto holdStartType = inherited
            ? AnimGraph::HandleType::InheritedHoldStart
            : AnimGraph::HandleType::HoldStart;
        const auto holdEndType = inherited
            ? AnimGraph::HandleType::InheritedHoldEnd
            : AnimGraph::HandleType::HoldEnd;

        AnimGraph::PointList points = {
            { 0.0f,          p0, holdStartType },
            { startFramePct, p0, holdEndType },
            { 100.0f,        p2, AnimGraph::HandleType::Locked }
        };
        graph.setControlPoints(points);
    };

    updateGraph(mGraphX, &Math::Vector2f::getX);
    updateGraph(mGraphY, &Math::Vector2f::getY);

    if (inherited) {
        mGraphX.setVerticalAxisLabel("Scale Multiplier X");
        mGraphY.setVerticalAxisLabel("Scale Multiplier Y");
        mGraphX.setTickStepSize(0.25f);
        mGraphY.setTickStepSize(0.25f);
    } else {
        mGraphX.setVerticalAxisLabel("Scale X");
        mGraphY.setVerticalAxisLabel("Scale Y");
        mGraphX.setTickStepSize(1.0f);
        mGraphY.setTickStepSize(1.0f);
    }

    mGraphX.zoomToFit();
    mGraphY.zoomToFit();

    if (inherited) {
        mGraphX.setHandleTooltips({
            "Scale Hold Start.\nLocked to 0% lifetime.\nValue is a multiplier relative to the inherited scale.",
            "Scale Hold End.\nValue is synced with the Scale Hold Start handle.\nValue is a multiplier relative to the inherited scale.",
            "Scale Target.\nLocked to 100% lifetime.\nValue is a multiplier relative to the inherited scale.",
        });
        mGraphY.setHandleTooltips({
            "Scale Hold Start.\nLocked to 0% lifetime.\nValue is a multiplier relative to the inherited scale.",
            "Scale Hold End.\nValue is synced with the Scale Hold Start handle.\nValue is a multiplier relative to the inherited scale.",
            "Scale Target.\nLocked to 100% lifetime.\nValue is a multiplier relative to the inherited scale.",
        });
    } else {
        mGraphX.setHandleTooltips({
            "Scale Hold Start.\nLocked to 0% lifetime.",
            "Scale Hold End.\nValue is synced with the Scale Hold Start handle.",
            "Scale Target.\nLocked to 100% lifetime.",
        });
        mGraphY.setHandleTooltips({
            "Scale Hold Start.\nLocked to 0% lifetime.",
            "Scale Hold End.\nValue is synced with the Scale Hold Start handle.",
            "Scale Target.\nLocked to 100% lifetime.",
        });
    }

    mOverlay->update();
}

void ChildRotationScaleInspector::populateProperties() {
    QSignalBlocker b1(mRotTypeComboBox);
    QSignalBlocker b2(mInitRotSpinBox);
    QSignalBlocker b3(mInitRotRandSpinBox);
    QSignalBlocker b4(mRotVelSpinBox);
    QSignalBlocker b5(mRotVelRandSpinBox);
    QSignalBlocker b6(mRotBasisSpinBox);
    QSignalBlocker b7(mInheritRotCheckBox);
    QSignalBlocker b8(mInheritRateSpinBox);
    QSignalBlocker b9(mInheritScaleCheckBox);
    QSignalBlocker b10(mInitPosRandSpinBox);

    auto idx2degVec = [](const Math::Vector3i& v) {
        return Math::Vector3f {
            Math::Util::to180(Math::Util::idx2deg(v.getX())),
            Math::Util::to180(Math::Util::idx2deg(v.getY())),
            Math::Util::to180(Math::Util::idx2deg(v.getZ()))
        };
    };

    mRotTypeComboBox.setCurrentEnum(mEmitter->childRotationType());
    mInitRotSpinBox.setVector(idx2degVec(mEmitter->childInitialRotation()));
    mInitRotRandSpinBox.setVector(idx2degVec(mEmitter->childInitialRotationRandom()));
    mRotVelSpinBox.setVector(idx2degVec(mEmitter->childRotationVelocity()));
    mRotVelRandSpinBox.setVector(idx2degVec(mEmitter->childRotationVelocityRandom()));
    mRotBasisSpinBox.setVector(mEmitter->childRotationBasis());
    mInheritRotCheckBox.setChecked(mEmitter->isChildInheritRotation());

    updateAxis();

    // Scale properties
    const bool inherited = mEmitter->isChildInheritScale();
    mInheritScaleCheckBox.setChecked(inherited);
    mInheritRateSpinBox.setEnabled(inherited);
    mInheritRateSpinBox.setValue(mEmitter->childScaleInheritRate() * 100.0);

    const bool emitterChanged = (mEmitter != mLastEmitter);
    mLastEmitter = mEmitter;

    updateGraphs();

    if (emitterChanged) {
        mGraphX.zoomToFit();
        mGraphY.zoomToFit();
    }

    // Position properties
    mInitPosRandSpinBox.setValue(mEmitter->childInitalPositionRand());

    update();
}

void ChildRotationScaleInspector::updateAxis() {
    using Axis = VectorSpinBoxBase::Axis;

    switch (mEmitter->childRotationType()) {
    case Ptcl::RotType::None:
        mInitRotSpinBox.setEnabledAxis(Axis::None);
        mInitRotRandSpinBox.setEnabledAxis(Axis::None);
        mRotVelSpinBox.setEnabledAxis(Axis::None);
        mRotVelRandSpinBox.setEnabledAxis(Axis::None);
        break;
    case Ptcl::RotType::RotX:
        mInitRotSpinBox.setEnabledAxis(Axis::X);
        mInitRotRandSpinBox.setEnabledAxis(Axis::X);
        mRotVelSpinBox.setEnabledAxis(Axis::X);
        mRotVelRandSpinBox.setEnabledAxis(Axis::X);
        break;
    case Ptcl::RotType::RotY:
        mInitRotSpinBox.setEnabledAxis(Axis::Y);
        mInitRotRandSpinBox.setEnabledAxis(Axis::Y);
        mRotVelSpinBox.setEnabledAxis(Axis::Y);
        mRotVelRandSpinBox.setEnabledAxis(Axis::Y);
        break;
    case Ptcl::RotType::RotZ:
        mInitRotSpinBox.setEnabledAxis(Axis::Z);
        mInitRotRandSpinBox.setEnabledAxis(Axis::Z);
        mRotVelSpinBox.setEnabledAxis(Axis::Z);
        mRotVelRandSpinBox.setEnabledAxis(Axis::Z);
        break;
    case Ptcl::RotType::RotXYZ:
        mInitRotSpinBox.setEnabledAxis(Axis::XYZ);
        mInitRotRandSpinBox.setEnabledAxis(Axis::XYZ);
        mRotVelSpinBox.setEnabledAxis(Axis::XYZ);
        mRotVelRandSpinBox.setEnabledAxis(Axis::XYZ);
        break;
    default:
        break;
    }
}


// ========================================================================== //


} // namespace PtclEditor
