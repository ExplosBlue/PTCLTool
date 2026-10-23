#include "editor/viewport/viewportDebugOverlay.h"

#include "math/util.h"
#include "ptcl/engine/emitterCalcTables.h"
#include "ptcl/ptclDocument.h"

#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include <utility>
#include <vector>


namespace PtclTool {


// ========================================================================== //


namespace {


constexpr s32 cShapeArcSegments = 96;
constexpr s32 cShapeMeridianSegments = 32;
constexpr f32 cShapeFullTurn = 2.0f * std::numbers::pi_v<f32>;
constexpr f32 cShapeAngleTolerance = 1.0e-4f;
constexpr f32 cMinClipW = 1.0e-5f;
constexpr f32 cShapePointSize = 5.0f;
constexpr f32 cShapeLineWidth = 1.8f;
constexpr f32 cSelectedShapeLineWidth = 2.4f;
constexpr f32 cShapeInactiveLineWidth = 1.2f;
constexpr s32 cMaxDivisionMarkers = 512;
constexpr std::array<s32, 8> cSameDivideSphereCounts{2, 3, 4, 6, 8, 12, 20, 32};

struct ShapeSegment {
    Math::Vector3f start;
    Math::Vector3f end;
    bool active{true};
};

using ShapeSegments = std::vector<ShapeSegment>;
using ShapePoints = std::vector<Math::Vector3f>;

struct ShapeArc {
    f32 start{0.0f};
    f32 sweep{cShapeFullTurn};

    bool isFull() const {
        return sweep >= cShapeFullTurn - cShapeAngleTolerance;
    }
};

f32 shapeIndexToRadians(u32 index) {
    const f32 angle = Math::Util::idx2rad(static_cast<s32>(index));
    return angle < 0.0f ? angle + cShapeFullTurn : angle;
}

ShapeArc shapeArc(const Ptcl::Emitter& emitter) {
    const u32 startIndex = static_cast<u32>(emitter.volumeArcWidth());
    const u32 sweepIndex = static_cast<u32>(emitter.volumeArcStart());

    ShapeArc arc{};
    arc.start = shapeIndexToRadians(startIndex);
    if (sweepIndex != 0) {
        arc.sweep = shapeIndexToRadians(sweepIndex);
    }
    return arc;
}

ShapeArc divisionCircleArc(const Ptcl::Emitter& emitter) {
    ShapeArc arc{};
    arc.start = shapeIndexToRadians(static_cast<u32>(emitter.volumeArcWidth()));
    arc.sweep = 0.0f;

    const s32 divisionCount = std::max(0, emitter.emitRate());
    if (divisionCount > 1) {
        const f32 step = shapeIndexToRadians(static_cast<u32>(emitter.volumeArcStart()));
        const f32 coverage = step * static_cast<f32>(divisionCount);
        arc.sweep = (coverage >= cShapeFullTurn - cShapeAngleTolerance)
            ? cShapeFullTurn
            : step * static_cast<f32>(divisionCount - 1);
    }

    return arc;
}

bool shapeAngleInArc(f32 angle, const ShapeArc& arc) {
    if (arc.isFull()) {
        return true;
    }
    if (arc.sweep <= 0.0f) {
        return false;
    }

    f32 delta = std::fmod(angle - arc.start, cShapeFullTurn);
    if (delta < 0.0f) {
        delta += cShapeFullTurn;
    }

    return delta <= arc.sweep + cShapeAngleTolerance;
}

template <typename PointAt>
void appendShapeArc(ShapeSegments& segments, s32 count, PointAt&& pointAt, bool active) {
    if (count < 2) {
        return;
    }

    Math::Vector3f previous = pointAt(0.0f);
    for (s32 i = 1; i <= count; ++i) {
        const f32 t = static_cast<f32>(i) / static_cast<f32>(count);
        const Math::Vector3f current = pointAt(t);
        segments.push_back({previous, current, active});
        previous = current;
    }
}

s32 shapeArcSegmentCount(f32 sweep) {
    const f32 clampedSweep = std::clamp(sweep, 0.0f, cShapeFullTurn);
    return std::max(2, static_cast<s32>(std::ceil(
        static_cast<f32>(cShapeArcSegments) * clampedSweep / cShapeFullTurn)));
}

template <typename PointAt>
void appendShapeRing(ShapeSegments& segments, PointAt&& pointAt, const ShapeArc& activeArc) {
    if (activeArc.isFull()) {
        appendShapeArc(segments, cShapeArcSegments, [&](f32 t) {
            return pointAt(cShapeFullTurn * t);
        }, true);
        return;
    }

    const f32 activeSweep = std::clamp(activeArc.sweep, 0.0f, cShapeFullTurn);
    if (activeSweep > 0.0f) {
        appendShapeArc(segments, shapeArcSegmentCount(activeSweep), [&](f32 t) {
            return pointAt(activeArc.start + activeSweep * t);
        }, true);
    }

    const f32 inactiveSweep = cShapeFullTurn - activeSweep;
    if (inactiveSweep > 0.0f) {
        const f32 inactiveStart = activeArc.start + activeSweep;
        appendShapeArc(segments, shapeArcSegmentCount(inactiveSweep), [&](f32 t) {
            return pointAt(inactiveStart + inactiveSweep * t);
        }, false);
    }
}

void appendShapeLine(ShapeSegments& segments, const Math::Vector3f& start, const Math::Vector3f& end, bool active = true) {
    segments.push_back({start, end, active});
}

void appendShapeMeridian(ShapeSegments& segments, const Math::Vector3f& radius, f32 azimuth, bool active) {
    const f32 sinAzimuth = std::sin(azimuth);
    const f32 cosAzimuth = std::cos(azimuth);

    appendShapeArc(segments, cShapeMeridianSegments, [&](f32 t) {
        const f32 elevation = (t - 0.5f) * std::numbers::pi_v<f32>;
        const f32 sinElevation = std::sin(elevation);
        const f32 cosElevation = std::cos(elevation);
        return Math::Vector3f{
            sinAzimuth * cosElevation * radius.x,
            -sinElevation * radius.y,
            cosAzimuth * cosElevation * radius.z,
        };
    }, active);
}

void buildPointShape(ShapeSegments& segments) {
    const f32 size = cShapePointSize;
    appendShapeLine(segments, {-size, 0.0f, 0.0f}, {size, 0.0f, 0.0f});
    appendShapeLine(segments, {0.0f, -size, 0.0f}, {0.0f, size, 0.0f});
    appendShapeLine(segments, {0.0f, 0.0f, -size}, {0.0f, 0.0f, size});
}

void buildCircleShape(const Ptcl::Emitter& emitter, ShapeSegments& segments, const ShapeArc& activeArc) {
    const auto& radius = emitter.volumeRadius();

    appendShapeRing(segments, [&](f32 angle) {
        return Math::Vector3f{
            std::sin(angle) * radius.x,
            0.0f,
            std::cos(angle) * radius.z,
        };
    }, activeArc);
}

void buildSphereShape(const Ptcl::Emitter& emitter, ShapeSegments& segments, const ShapeArc& activeArc) {
    const auto& radius = emitter.volumeRadius();
    constexpr f32 cHalfPi = std::numbers::pi_v<f32> * 0.5f;

    constexpr std::array<f32, 5> cLatitudeFactors{
        -0.75f,
        -0.375f,
        0.0f,
        0.375f,
        0.75f,
    };

    for (const f32 latitudeFactor : cLatitudeFactors) {
        const f32 elevation = latitudeFactor * cHalfPi;
        const f32 sinElevation = std::sin(elevation);
        const f32 cosElevation = std::cos(elevation);

        appendShapeRing(segments, [&](f32 angle) {
            return Math::Vector3f{
                std::sin(angle) * cosElevation * radius.x,
                -sinElevation * radius.y,
                std::cos(angle) * cosElevation * radius.z,
            };
        }, activeArc);
    }

    constexpr s32 cMeridianAzimuthCount = 4;
    for (s32 i = 0; i < cMeridianAzimuthCount; ++i) {
        const f32 azimuth = (static_cast<f32>(i) / static_cast<f32>(cMeridianAzimuthCount)) * cShapeFullTurn;
        appendShapeMeridian(segments, radius, azimuth, shapeAngleInArc(azimuth, activeArc));
    }

    if (!activeArc.isFull() && activeArc.sweep > 0.0f) {
        appendShapeMeridian(segments, radius, activeArc.start, true);
        appendShapeMeridian(segments, radius, activeArc.start + activeArc.sweep, true);
    }
}

void buildCylinderShape(const Ptcl::Emitter& emitter, ShapeSegments& segments, const ShapeArc& activeArc) {
    const auto& radius = emitter.volumeRadius();
    const f32 halfHeight = radius.y;

    for (const f32 y : { -halfHeight, halfHeight }) {
        appendShapeRing(segments, [&](f32 angle) {
            return Math::Vector3f{
                std::sin(angle) * radius.x,
                y,
                std::cos(angle) * radius.z,
            };
        }, activeArc);
    }

    constexpr s32 cSideCount = 4;
    for (s32 i = 0; i < cSideCount; ++i) {
        const f32 angle = (static_cast<f32>(i) / static_cast<f32>(cSideCount)) * cShapeFullTurn;
        const f32 x = std::sin(angle) * radius.x;
        const f32 z = std::cos(angle) * radius.z;
        appendShapeLine(segments, {x, -halfHeight, z}, {x, halfHeight, z}, shapeAngleInArc(angle, activeArc));
    }

    if (!activeArc.isFull() && activeArc.sweep > 0.0f) {
        for (const f32 angle : { activeArc.start, activeArc.start + activeArc.sweep }) {
            const f32 x = std::sin(angle) * radius.x;
            const f32 z = std::cos(angle) * radius.z;
            appendShapeLine(segments, {x, -halfHeight, z}, {x, halfHeight, z}, true);
        }
    }
}

void buildBoxShape(const Ptcl::Emitter& emitter, ShapeSegments& segments) {
    const auto& radius = emitter.volumeRadius();
    const std::array<Math::Vector3f, 8> corners{{
        {-radius.x, -radius.y, -radius.z},
        { radius.x, -radius.y, -radius.z},
        { radius.x,  radius.y, -radius.z},
        {-radius.x,  radius.y, -radius.z},
        {-radius.x, -radius.y,  radius.z},
        { radius.x, -radius.y,  radius.z},
        { radius.x,  radius.y,  radius.z},
        {-radius.x,  radius.y,  radius.z},
    }};

    static constexpr std::array<std::pair<s32, s32>, 12> cEdges{{
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7},
    }};

    for (const auto& [start, end] : cEdges) {
        appendShapeLine(segments, corners[start], corners[end]);
    }
}

void buildRectangleShape(const Ptcl::Emitter& emitter, ShapeSegments& segments) {
    const auto& radius = emitter.volumeRadius();
    const std::array<Math::Vector3f, 4> corners{{
        {-radius.x, 0.0f, -radius.z},
        { radius.x, 0.0f, -radius.z},
        { radius.x, 0.0f,  radius.z},
        {-radius.x, 0.0f,  radius.z},
    }};

    for (s32 i = 0; i < 4; ++i) {
        appendShapeLine(segments, corners[i], corners[(i + 1) % 4]);
    }
}

void buildLineShape(const Ptcl::Emitter& emitter, ShapeSegments& segments) {
    const auto& radius = emitter.volumeRadius();
    appendShapeLine(segments, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, radius.z});
}

s32 sampledDivisionIndex(s32 markerIndex, s32 markerCount, s32 divisionCount) {
    if (markerCount <= 1 || divisionCount <= 1) {
        return 0;
    }

    return static_cast<s32>(
        static_cast<s64>(markerIndex) * static_cast<s64>(divisionCount - 1)
        / static_cast<s64>(markerCount - 1)
    );
}

void buildEmissionDivisionPoints(const Ptcl::Emitter& emitter, ShapePoints& points) {
    const auto& radius = emitter.volumeRadius();

    const s32 divisionCount = std::max(0, emitter.emitRate());
    const s32 markerCount = std::min(divisionCount, cMaxDivisionMarkers);

    switch (emitter.volumeType()) {
    case Ptcl::VolumeType::CircleSameDiv: {
        const u32 baseAngle = static_cast<u32>(emitter.volumeArcWidth());
        const u32 stepAngle = static_cast<u32>(emitter.volumeArcStart());

        for (s32 markerIndex = 0; markerIndex < markerCount; ++markerIndex) {
            const s32 divisionIndex = sampledDivisionIndex(markerIndex, markerCount, divisionCount);
            const u32 angleIndex = baseAngle + stepAngle * static_cast<u32>(divisionIndex);
            const f32 angle = shapeIndexToRadians(angleIndex);

            points.push_back({
                std::sin(angle) * radius.x,
                0.0f,
                std::cos(angle) * radius.z,
            });
        }
        break;
    }
    case Ptcl::VolumeType::SphereSameDiv: {
        const s32 tableIndex = std::clamp(static_cast<s32>(emitter.volumeTblIndex()), 0, 7);
        const Math::Vector3f* directions = Ptcl::Engine::sSameDivideSphereTable[tableIndex];
        const s32 count = std::min(divisionCount, cSameDivideSphereCounts[tableIndex]);

        for (s32 i = 0; i < count; ++i) {
            const Math::Vector3f& direction = directions[i];
            points.push_back({
                direction.x * radius.x,
                direction.y * radius.y,
                direction.z * radius.z,
            });
        }
        break;
    }
    case Ptcl::VolumeType::LineSameDiv:
        for (s32 markerIndex = 0; markerIndex < markerCount; ++markerIndex) {
            const s32 divisionIndex = sampledDivisionIndex(markerIndex, markerCount, divisionCount);
            const f32 t = (divisionCount <= 1)
                ? 0.5f
                : static_cast<f32>(divisionIndex) / static_cast<f32>(divisionCount - 1);

            points.push_back({0.0f, 0.0f, t * radius.z});
        }
        break;
    default:
        break;
    }
}

void buildEmissionShapeSegments(const Ptcl::Emitter& emitter, ShapeSegments& segments) {
    switch (emitter.volumeType()) {
    case Ptcl::VolumeType::Point:
        buildPointShape(segments);
        break;
    case Ptcl::VolumeType::Circle:
        buildCircleShape(emitter, segments, shapeArc(emitter));
        break;
    case Ptcl::VolumeType::CircleSameDiv:
        buildCircleShape(emitter, segments, divisionCircleArc(emitter));
        break;
    case Ptcl::VolumeType::CircleFill:
        buildCircleShape(emitter, segments, shapeArc(emitter));
        break;
    case Ptcl::VolumeType::Sphere:
        buildSphereShape(emitter, segments, shapeArc(emitter));
        break;
    case Ptcl::VolumeType::SphereSameDiv:
        buildSphereShape(emitter, segments, ShapeArc{});
        break;
    case Ptcl::VolumeType::SphereFill:
        buildSphereShape(emitter, segments, shapeArc(emitter));
        break;
    case Ptcl::VolumeType::Cylinder:
        buildCylinderShape(emitter, segments, shapeArc(emitter));
        break;
    case Ptcl::VolumeType::CylinderFill:
        buildCylinderShape(emitter, segments, shapeArc(emitter));
        break;
    case Ptcl::VolumeType::Box:
    case Ptcl::VolumeType::BoxFill:
        buildBoxShape(emitter, segments);
        break;
    case Ptcl::VolumeType::Line:
    case Ptcl::VolumeType::LineSameDiv:
        buildLineShape(emitter, segments);
        break;
    case Ptcl::VolumeType::Rectangle:
        buildRectangleShape(emitter, segments);
        break;
    }
}

bool projectShapePoint(const Math::Vector3f& point, const Math::Matrix44f& viewProjection, const QSize& viewportSize, QPointF& screenPoint) {

    const f32 clipX =
        viewProjection(0, 0) * point.x +
        viewProjection(0, 1) * point.y +
        viewProjection(0, 2) * point.z +
        viewProjection(0, 3);

    const f32 clipY =
        viewProjection(1, 0) * point.x +
        viewProjection(1, 1) * point.y +
        viewProjection(1, 2) * point.z +
        viewProjection(1, 3);

    const f32 clipW =
        viewProjection(3, 0) * point.x +
        viewProjection(3, 1) * point.y +
        viewProjection(3, 2) * point.z +
        viewProjection(3, 3);

    if (!(clipW > cMinClipW)) {
        return false;
    }

    const f32 inverseW = 1.0f / clipW;
    const f32 ndcX = clipX * inverseW;
    const f32 ndcY = clipY * inverseW;
    if (!std::isfinite(ndcX) || !std::isfinite(ndcY)) {
        return false;
    }

    screenPoint = {
        (ndcX * 0.5f + 0.5f) * static_cast<f32>(viewportSize.width()),
        (0.5f - ndcY * 0.5f) * static_cast<f32>(viewportSize.height()),
    };
    return true;
}


} // namespace


// ========================================================================== //


ViewportDebugOverlay::ViewportDebugOverlay(QObject* parent) :
    QObject{parent} {
}

void ViewportDebugOverlay::setEmissionShapesVisible(bool visible) {
    if (mEmissionShapesVisible == visible) {
        return;
    }

    mEmissionShapesVisible = visible;
    emit emissionShapesVisibleChanged(visible);
    emit changed();
}

void ViewportDebugOverlay::setDebugSelectedOnly(bool selectedOnly) {
    if (mDebugSelectedOnly == selectedOnly) {
        return;
    }

    mDebugSelectedOnly = selectedOnly;
    emit debugSelectedOnlyChanged(selectedOnly);
    emit changed();
}

void ViewportDebugOverlay::draw(QPainter& painter, const Context& context) const {
    if (!mEmissionShapesVisible || context.emitterSet == nullptr || context.viewProjection == nullptr) {
        return;
    }

    const Ptcl::EmitterSet* set = context.emitterSet;
    if (set->emitterCount() == 0 || context.viewportSize.isEmpty()) {
        return;
    }

    const QSize viewportSize = context.viewportSize;
    const Math::Matrix44f& viewProjection = *context.viewProjection;
    const bool hasSelectedEmitter = context.selectedEmitterIndex >= 0;

    if (mDebugSelectedOnly && !hasSelectedEmitter && !context.hasSelection) {
        return;
    }

    ShapeSegments segments;
    segments.reserve(2048);

    ShapePoints divisionPoints;
    divisionPoints.reserve(cMaxDivisionMarkers);

    std::vector<QPointF> divisionMarkers;
    divisionMarkers.reserve(cMaxDivisionMarkers);

    std::vector<QPointF> selectedDivisionMarkers;
    selectedDivisionMarkers.reserve(cMaxDivisionMarkers);

    QPainterPath activePath;
    QPainterPath inactivePath;
    QPainterPath selectedActivePath;
    QPainterPath selectedInactivePath;

    const auto& emitters = set->emitters();
    const s32 emitterCount = static_cast<s32>(emitters.size());
    for (s32 emitterIndex = 0; emitterIndex < emitterCount; ++emitterIndex) {
        const auto& emitter = emitters[emitterIndex];
        if (emitter == nullptr) {
            continue;
        }

        const bool selected = hasSelectedEmitter && emitterIndex == context.selectedEmitterIndex;
        if (mDebugSelectedOnly && hasSelectedEmitter && !selected) {
            continue;
        }

        QPainterPath& targetActivePath = selected ? selectedActivePath : activePath;
        QPainterPath& targetInactivePath = selected ? selectedInactivePath : inactivePath;
        std::vector<QPointF>& targetMarkers = selected ? selectedDivisionMarkers : divisionMarkers;

        segments.clear();
        buildEmissionShapeSegments(*emitter, segments);

        divisionPoints.clear();
        buildEmissionDivisionPoints(*emitter, divisionPoints);

        const Math::Matrix34f& transform = emitter->transformSRT();
        for (const ShapeSegment& segment : segments) {
            const Math::Vector3f worldStart = Math::MatrixCalc::multiply(transform, segment.start);
            const Math::Vector3f worldEnd = Math::MatrixCalc::multiply(transform, segment.end);

            QPointF screenStart;
            QPointF screenEnd;
            if (!projectShapePoint(worldStart, viewProjection, viewportSize, screenStart)) {
                continue;
            }
            if (!projectShapePoint(worldEnd, viewProjection, viewportSize, screenEnd)) {
                continue;
            }

            QPainterPath& targetPath = segment.active ? targetActivePath : targetInactivePath;
            if (targetPath.isEmpty() || targetPath.currentPosition() != screenStart) {
                targetPath.moveTo(screenStart);
            }
            targetPath.lineTo(screenEnd);
        }

        for (const Math::Vector3f& point : divisionPoints) {
            const Math::Vector3f worldPoint = Math::MatrixCalc::multiply(transform, point);

            QPointF screenPoint;
            if (projectShapePoint(worldPoint, viewProjection, viewportSize, screenPoint)) {
                targetMarkers.push_back(screenPoint);
            }
        }
    }

    if (activePath.isEmpty() && inactivePath.isEmpty()
        && selectedActivePath.isEmpty() && selectedInactivePath.isEmpty()
        && divisionMarkers.empty() && selectedDivisionMarkers.empty()) {
        return;
    }

    painter.save();
    painter.setBrush(Qt::NoBrush);

    const QColor shapeColor(120, 220, 255, 225);
    const QColor selectedShapeColor(255, 186, 96, 255);
    const QColor inactiveShapeColor(120, 220, 255, 80);
    const QColor selectedInactiveShapeColor(255, 186, 96, 110);
    const QColor divisionColor(120, 220, 255, 245);
    const QColor selectedDivisionColor(255, 190, 96, 255);

    const auto drawShapePath = [&painter](const QPainterPath& shapePath, const QColor& color, f32 width, Qt::PenStyle style) {
        QPen pen(color);
        pen.setWidthF(width);
        pen.setStyle(style);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(shapePath);
    };

    const auto drawDivisionMarkers = [&painter](const std::vector<QPointF>& markers, const QColor& color, f32 radius) {
        if (markers.empty()) {
            return;
        }

        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        for (const QPointF& marker : markers) {
            painter.drawEllipse(marker, radius, radius);
        }
    };

    if (!inactivePath.isEmpty()) {
        drawShapePath(inactivePath, inactiveShapeColor, cShapeInactiveLineWidth, Qt::DotLine);
    }
    if (!selectedInactivePath.isEmpty()) {
        drawShapePath(selectedInactivePath, selectedInactiveShapeColor, cShapeInactiveLineWidth, Qt::DotLine);
    }
    if (!activePath.isEmpty()) {
        drawShapePath(activePath, shapeColor, cShapeLineWidth, Qt::SolidLine);
    }
    drawDivisionMarkers(divisionMarkers, divisionColor, 2.4f);

    if (!selectedActivePath.isEmpty()) {
        drawShapePath(selectedActivePath, selectedShapeColor, cSelectedShapeLineWidth, Qt::SolidLine);
    }
    drawDivisionMarkers(selectedDivisionMarkers, selectedDivisionColor, 3.0f);

    painter.restore();
}


// ========================================================================== //


} // namespace PtclTool
