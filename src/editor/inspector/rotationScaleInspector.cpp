#include "editor/inspector/rotationScaleInspector.h"

#include "math/util.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
#include <QVBoxLayout>


namespace PtclEditor {


// ========================================================================== //


static const std::array rotTypeOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::RotType>{ Ptcl::RotType::None,   "None",    "No rotation." },
    EnumOption<Ptcl::RotType>{ Ptcl::RotType::RotX,   "X-Axis",  "Rotates around the X-axis." },
    EnumOption<Ptcl::RotType>{ Ptcl::RotType::RotY,   "Y-Axis",  "Rotates around the Y-axis." },
    EnumOption<Ptcl::RotType>{ Ptcl::RotType::RotZ,   "Z-Axis",  "Rotates around the Z-axis." },
    EnumOption<Ptcl::RotType>{ Ptcl::RotType::RotXYZ, "XYZ-Axis", "Rotates around all three axes." },
};


RotationScaleInspector::RotationScaleInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    // TODO: Move these somewhere else
    static constexpr QColor sColorAxisX = { 238, 51, 79 };
    static constexpr QColor sColorAxisY = { 42, 125, 212 };

    mGraphX.setLineColor(sColorAxisX);
    mGraphX.setVerticalAxisLabel("Scale X");
    mGraphX.setPosLabel("Life");
    mGraphX.setValLabel("Scale");
    mGraphX.setToolTip("Scale X over the particle's lifetime.");
    mGraphY.setLineColor(sColorAxisY);
    mGraphY.setVerticalAxisLabel("Scale Y");
    mGraphY.setPosLabel("Life");
    mGraphY.setValLabel("Scale");
    mGraphY.setToolTip("Scale Y over the particle's lifetime.");

    // Force identical left padding so content rects align and
    // connector lines drawn at the same percentage position match.
    constexpr s32 forcedPad = 48;
    mGraphX.setFixedLeftPadding(forcedPad);
    mGraphY.setFixedLeftPadding(forcedPad);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    // Rotation section
    addSectionHeader(mainLayout, "Rotation", this);

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

    mainLayout->addRow(&mGraphX);
    mainLayout->addRow(&mGraphY);
    mScaleRandSpinbox.setRange(0.0, 100.0);
    mScaleRandSpinbox.setSuffix("%");
    mScaleRandSpinbox.setSingleStep(1.0);
    mScaleRandSpinbox.setDecimals(0);
    mainLayout->addRow("Scale Variation:", &mScaleRandSpinbox);
    mScaleRandSpinbox.setToolTip("Random variation in scale when particles are spawned.");

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

void RotationScaleInspector::resizeEvent(QResizeEvent* event) {
    InspectorWidgetBase::resizeEvent(event);
    mOverlay->setGeometry(rect());
}

bool RotationScaleInspector::eventFilter(QObject* obj, QEvent* event) {
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

void RotationScaleInspector::setupConnections() {
    // Rotation connections
    connect(&mRotTypeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mRotTypeComboBox.currentEnum();

        setEmitterProperty(
            "Set Rotation type",
            "SetRotType",
            &Ptcl::Emitter::rotationType,
            &Ptcl::Emitter::setRotationType,
            type
        );
    });

    auto deg2idxVec = [](const Math::Vector3f& v) {
        Math::Vector3i result {
            Math::Util::deg2idx(v.getX()),
            Math::Util::deg2idx(v.getY()),
            Math::Util::deg2idx(v.getZ())
        };
        return result;
    };

    connect(&mInitRotSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto rotation = deg2idxVec(mInitRotSpinBox.getVector());

        setEmitterProperty(
            "Set Initial Rotation",
            "SetInitRot",
            &Ptcl::Emitter::initialRotation,
            &Ptcl::Emitter::setInitialRotation,
            rotation
        );
    });

    connect(&mInitRotRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto rand = deg2idxVec(mInitRotRandSpinBox.getVector());

        setEmitterProperty(
            "Set Initial Rotation Variation",
            "SetInitRotRand",
            &Ptcl::Emitter::initialRotationRandom,
            &Ptcl::Emitter::setInitialRotationRandom,
            rand
        );
    });

    connect(&mRotVelSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto velocity = deg2idxVec(mRotVelSpinBox.getVector());

        setEmitterProperty(
            "Set Rotation Speed",
            "SetRotVel",
            &Ptcl::Emitter::rotationVelocity,
            &Ptcl::Emitter::setRotationVelocity,
            velocity
        );
    });

    connect(&mRotVelRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto rand = deg2idxVec(mRotVelRandSpinBox.getVector());

        setEmitterProperty(
            "Set Rotation Speed Variation",
            "SetRotVelRand",
            &Ptcl::Emitter::rotationVelocityRandom,
            &Ptcl::Emitter::setRotationVelocityRandom,
            rand
        );
    });

    connect(&mRotBasisSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto basis = mRotBasisSpinBox.getVector();

        setEmitterProperty(
            "Set Rotation Pivot",
            "SetRotBasis",
            &Ptcl::Emitter::rotationBasis,
            &Ptcl::Emitter::setRotationBasis,
            basis
        );
    });

    // Scale connections
    connect(&mGraphX, &AnimGraph::pointEdited, this, [this](s32 pointIndex, const AnimGraph::GraphPoint& point) {
        updateAnimPoint(pointIndex, point, &Math::Vector2f::getX);
    });

    connect(&mGraphY, &AnimGraph::pointEdited, this, [this](s32 pointIndex, const AnimGraph::GraphPoint& point) {
        updateAnimPoint(pointIndex, point, &Math::Vector2f::getY);
    });

    connect(&mScaleRandSpinbox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Scale Variation",
            "SetScaleRand",
            &Ptcl::Emitter::scaleRand,
            &Ptcl::Emitter::setScaleRand,
            static_cast<f32>(value / 100.0)
         );
    });
}

void RotationScaleInspector::updateAnimPoint(s32 pointIndex, const AnimGraph::GraphPoint& point, f32 (Math::Vector2f::*get)() const) {
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

void RotationScaleInspector::updateGraphs() {
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


void RotationScaleInspector::populateProperties() {
    QSignalBlocker b1(mRotTypeComboBox);
    QSignalBlocker b2(mInitRotSpinBox);
    QSignalBlocker b3(mInitRotRandSpinBox);
    QSignalBlocker b4(mRotVelSpinBox);
    QSignalBlocker b5(mRotVelRandSpinBox);
    QSignalBlocker b6(mRotBasisSpinBox);
    QSignalBlocker b7(mScaleRandSpinbox);

    auto idx2degVec = [](const Math::Vector3i& v) {
        return Math::Vector3f {
            Math::Util::to180(Math::Util::idx2deg(v.getX())),
            Math::Util::to180(Math::Util::idx2deg(v.getY())),
            Math::Util::to180(Math::Util::idx2deg(v.getZ()))
        };
    };

    mRotTypeComboBox.setCurrentEnum(mEmitter->rotationType());
    mInitRotSpinBox.setVector(idx2degVec(mEmitter->initialRotation()));
    mInitRotRandSpinBox.setVector(idx2degVec(mEmitter->initialRotationRandom()));
    mRotVelSpinBox.setVector(idx2degVec(mEmitter->rotationVelocity()));
    mRotVelRandSpinBox.setVector(idx2degVec(mEmitter->rotationVelocityRandom()));
    mRotBasisSpinBox.setVector(mEmitter->rotationBasis());

    updateAxis();

    // Scale properties
    const bool emitterChanged = (mEmitter != mLastEmitter);
    mLastEmitter = mEmitter;

    updateGraphs();

    if (emitterChanged) {
        mGraphX.zoomToFit();
        mGraphY.zoomToFit();
    }

    QSignalBlocker blockerRand(mScaleRandSpinbox);
    mScaleRandSpinbox.setValue(mEmitter->scaleRand() * 100.0);

    update();
}

void RotationScaleInspector::updateAxis() {
    using Axis = VectorSpinBoxBase::Axis;

    switch (mEmitter->rotationType()) {
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
