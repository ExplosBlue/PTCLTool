#include "editor/viewport/viewportGizmo.h"

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPen>

#include <algorithm>
#include <array>
#include <cmath>


namespace PtclTool {


namespace {

constexpr int cGizmoSize = 108;
constexpr int cGizmoMargin = 12;
constexpr f32 cGizmoAxisLength = 31.0f;
constexpr f32 cGizmoPrimaryRadius = 10.5f;
constexpr f32 cGizmoSecondaryRadius = 7.0f;
constexpr f32 cGizmoNearScale = 1.12f;
constexpr f32 cGizmoFarScale = 0.82f;
constexpr f32 cGizmoHitPadding = 2.0f;
constexpr f32 cGizmoOverlapTolerance = 6.0f;

struct GizmoAxisInfo {
    Math::Vector3f direction;
    QColor color;
    const char* label;
    const char* viewName;
    ViewportCamera::ViewDirection view;
    const char* shortcut;
    bool primary;
};

const std::array<GizmoAxisInfo, 6>& gizmoAxes() {
    static const std::array<GizmoAxisInfo, 6> axes{{
        {
            {1.0f, 0.0f, 0.0f},
            QColor::fromRgbF(0.95f, 0.24f, 0.28f),
            "X", "Right", ViewportCamera::ViewDirection::Right, "Numpad 3", true
        },
        {
            {0.0f, 1.0f, 0.0f},
            QColor::fromRgbF(0.42f, 0.82f, 0.30f),
            "Y", "Top", ViewportCamera::ViewDirection::Top, "Numpad 7", true
        },
        {
            {0.0f, 0.0f, 1.0f},
            QColor::fromRgbF(0.25f, 0.50f, 0.98f),
            "Z", "Front", ViewportCamera::ViewDirection::Front, "Numpad 1", true
        },
        {
            {-1.0f, 0.0f, 0.0f},
            QColor::fromRgbF(0.95f, 0.24f, 0.28f),
            "-X", "Left", ViewportCamera::ViewDirection::Left, "Ctrl+Numpad 3", false
        },
        {
            {0.0f, -1.0f, 0.0f},
            QColor::fromRgbF(0.42f, 0.82f, 0.30f),
            "-Y", "Bottom", ViewportCamera::ViewDirection::Bottom, "Ctrl+Numpad 7", false
        },
        {
            {0.0f, 0.0f, -1.0f},
            QColor::fromRgbF(0.25f, 0.50f, 0.98f),
            "-Z", "Back", ViewportCamera::ViewDirection::Back, "Ctrl+Numpad 1", false
        },
    }};
    return axes;
}

QPointF gizmoCenter(const QRect& area) {
    return {
        static_cast<qreal>(area.x()) + static_cast<qreal>(area.width()) * 0.5,
        static_cast<qreal>(area.y()) + static_cast<qreal>(area.height()) * 0.5
    };
}

f32 gizmoDepthNearness(f32 depth) {
    return std::clamp((1.0f - depth) * 0.5f, 0.0f, 1.0f);
}

qreal gizmoRadius(const GizmoAxisInfo& info, f32 depth, bool aligned, bool hovered) {
    const qreal baseRadius = (info.primary || aligned)
        ? cGizmoPrimaryRadius
        : cGizmoSecondaryRadius;

    const qreal scale = cGizmoFarScale + (cGizmoNearScale - cGizmoFarScale) * gizmoDepthNearness(depth);
    return baseRadius * scale + (hovered ? 2.0 : 0.0);
}

qreal gizmoHitRadius(const GizmoAxisInfo& info, f32 depth, bool aligned) {
    return gizmoRadius(info, depth, aligned, false) + cGizmoHitPadding;
}


struct GizmoEndpoint {
    int axis{0};
    QPointF position{};
    f32 depth{0.0f};
};


struct GizmoHit {
    int axis{-1};
    f32 distance{0.0f};
    f32 depth{0.0f};
};


} // namespace


// ========================================================================== //


QRect ViewportGizmo::area(const QSize& widgetSize) const {
    return {
        widgetSize.width() - cGizmoSize - cGizmoMargin,
        cGizmoMargin,
        cGizmoSize,
        cGizmoSize
    };
}

bool ViewportGizmo::press(const QPoint& pos, const ViewportCamera& camera, const QSize& widgetSize) {
    if (!area(widgetSize).contains(pos)) {
        return false;
    }

    mHoverAxis = -1;
    mPressAxis = axisAt(pos, camera, widgetSize);
    mPressPos = pos;
    mLastMouse = pos;
    mPressActive = true;
    mDragging = false;
    return true;
}

ViewportGizmo::MoveResult ViewportGizmo::move(const QPoint& pos) {
    MoveResult result{};
    if (!mPressActive) {
        return result;
    }

    const QPoint totalDelta = pos - mPressPos;
    if (!mDragging && totalDelta.manhattanLength() > 4) {
        mDragging = true;
        result.startedDragging = true;
    }

    if (mDragging) {
        result.dragging = true;
        result.delta = pos - mLastMouse;
    }

    mLastMouse = pos;
    return result;
}

ViewportGizmo::ReleaseResult ViewportGizmo::release() {
    ReleaseResult result{};
    if (!mPressActive) {
        return result;
    }

    result.handled = true;

    const bool wasDragging = mDragging;
    const int pressedAxis = mPressAxis;

    mPressActive = false;
    mDragging = false;
    mPressAxis = -1;

    if (!wasDragging && pressedAxis >= 0) {
        result.hasView = true;
        result.view = gizmoAxes()[pressedAxis].view;
    }

    return result;
}

QPointF ViewportGizmo::axisEndpoint(
    int axis,
    const ViewportCamera& camera,
    const QRect& area
) const {
    const auto& axes = gizmoAxes();
    if (axis < 0 || axis >= static_cast<int>(axes.size())) {
        return gizmoCenter(area);
    }

    const auto& info = axes[axis];
    const auto& basis = camera.viewBasis();
    const QPointF center = gizmoCenter(area);
    const f32 screenX = Math::VectorCalc::dot(info.direction, basis.right);
    const f32 screenY = Math::VectorCalc::dot(info.direction, basis.up);

    return {
        center.x() + static_cast<qreal>(screenX) * cGizmoAxisLength,
        center.y() - static_cast<qreal>(screenY) * cGizmoAxisLength
    };
}

s32 ViewportGizmo::axisAt(const QPoint& pos, const ViewportCamera& camera, const QSize& widgetSize) const {
    const QRect area = this->area(widgetSize);
    if (!area.contains(pos)) {
        return -1;
    }

    const auto& axes = gizmoAxes();
    const auto& basis = camera.viewBasis();
    std::array<GizmoHit, 6> hits{};
    s32 hitCount = 0;

    for (s32 axis = 0; axis < static_cast<int>(axes.size()); ++axis) {
        const auto& info = axes[axis];
        const f32 screenX = Math::VectorCalc::dot(info.direction, basis.right);
        const f32 screenY = Math::VectorCalc::dot(info.direction, basis.up);

        const bool aligned = (screenX * screenX + screenY * screenY) < 1.0e-4f;
        const f32 depth = Math::VectorCalc::dot(info.direction, basis.forward);

        if (aligned && depth > 0.0f) {
            continue;
        }

        const QPointF endpoint = axisEndpoint(axis, camera, area);
        const f32 deltaX = static_cast<f32>(endpoint.x() - static_cast<qreal>(pos.x()));
        const f32 deltaY = static_cast<f32>(endpoint.y() - static_cast<qreal>(pos.y()));
        const f32 distance = std::sqrt(deltaX * deltaX + deltaY * deltaY);
        const f32 hitRadius = gizmoHitRadius(info, depth, aligned);

        if (distance <= hitRadius) {
            hits[hitCount++] = {axis, distance, depth};
        }
    }

    if (hitCount == 0) {
        return -1;
    }

    s32 nearest = 0;
    for (s32 i = 1; i < hitCount; ++i) {
        if (hits[i].distance < hits[nearest].distance) {
            nearest = i;
        }
    }

    s32 chosen = nearest;
    for (s32 i = 0; i < hitCount; ++i) {
        if (hits[i].distance <= hits[nearest].distance + cGizmoOverlapTolerance && hits[i].depth < hits[chosen].depth) {
            chosen = i;
        }
    }

    return hits[chosen].axis;
}

QString ViewportGizmo::toolTip(s32 axis) const {
    const auto& axes = gizmoAxes();
    if (axis < 0 || axis >= static_cast<int>(axes.size())) {
        return {};
    }

    const auto& info = axes[axis];
    return QStringLiteral("Click: %1 view\nShortcut: %2\nDrag: Rotate the view")
        .arg(QString::fromLatin1(info.viewName), QString::fromLatin1(info.shortcut));
}

void ViewportGizmo::draw(QPainter& painter, const ViewportCamera& camera, const QSize& widgetSize) const {
    const QRect area = this->area(widgetSize);
    if (area.width() < 32 || area.height() < 32) {
        return;
    }

    const QPointF center = gizmoCenter(area);
    const auto& basis = camera.viewBasis();
    const auto& axes = gizmoAxes();

    if (mHoverAxis >= 0 || mPressActive) {
        painter.setPen(QPen(QColor(255, 255, 255, 28), 1.0));
        painter.setBrush(QColor(8, 10, 24, 55));
        painter.drawEllipse(center, 47.0, 47.0);
    }

    for (s32 axis = 0; axis < static_cast<int>(axes.size()); ++axis) {
        const auto& info = axes[axis];
        if (!info.primary) {
            continue;
        }

        const f32 depth = Math::VectorCalc::dot(info.direction, basis.forward);
        QColor color = info.color;
        color.setAlphaF(0.42f + 0.48f * std::clamp(-depth, 0.0f, 1.0f));
        painter.setPen(QPen(color, 1.5, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(center, axisEndpoint(axis, camera, area));
    }

    for (s32 axis = 0; axis < static_cast<int>(axes.size()); ++axis) {
        const auto& info = axes[axis];
        if (info.primary) {
            continue;
        }

        const f32 depth = Math::VectorCalc::dot(info.direction, basis.forward);
        QColor color = info.color;
        color.setAlphaF(0.16f + 0.18f * std::clamp(-depth, 0.0f, 1.0f));
        painter.setPen(QPen(color, 1.0, Qt::DotLine, Qt::RoundCap));
        painter.drawLine(center, axisEndpoint(axis, camera, area));
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(220, 220, 230, 180));
    painter.drawEllipse(center, 2.0, 2.0);

    std::array<GizmoEndpoint, 6> endpoints{};
    for (s32 axis = 0; axis < static_cast<int>(axes.size()); ++axis) {
        endpoints[axis] = {
            axis,
            axisEndpoint(axis, camera, area),
            Math::VectorCalc::dot(axes[axis].direction, basis.forward)
        };
    }

    std::sort(endpoints.begin(), endpoints.end(),
        [](const GizmoEndpoint& lhs, const GizmoEndpoint& rhs) {
            return lhs.depth > rhs.depth;
        }
    );

    for (const GizmoEndpoint& endpoint : endpoints) {
        const auto& info = axes[endpoint.axis];
        const f32 screenX = Math::VectorCalc::dot(info.direction, basis.right);
        const f32 screenY = Math::VectorCalc::dot(info.direction, basis.up);
        const bool aligned = (screenX * screenX + screenY * screenY) < 1.0e-4f;
        if (aligned && endpoint.depth > 0.0f) {
            continue;
        }

        const bool hovered = (endpoint.axis == mHoverAxis);
        const bool hasLabel = info.primary || hovered || aligned;
        const bool strong = info.primary || aligned;
        const qreal radius = gizmoRadius(info, endpoint.depth, aligned, hovered);
        const f32 nearness = hovered ? 1.0f : gizmoDepthNearness(endpoint.depth);

        QColor base = info.color;
        f32 hue = 0.0f;
        f32 saturation = 0.0f;
        f32 value = 0.0f;
        base.getHsvF(&hue, &saturation, &value);

        QColor fill;
        fill.setHsvF(
            hue,
            saturation * (0.55 + 0.45 * nearness),
            value * (0.68 + 0.32 * nearness),
            hovered ? 0.95 : (strong ? 0.58 + 0.42 * nearness : 0.26 + 0.56 * nearness)
        );

        if (hovered) {
            QColor halo = info.color;
            halo.setAlphaF(0.22f + 0.10f * nearness);
            painter.setPen(Qt::NoPen);
            painter.setBrush(halo);
            painter.drawEllipse(endpoint.position, radius + 4.0, radius + 4.0);
        }

        QColor outline;
        outline.setHsvF(
            hue,
            saturation * (0.65 + 0.35 * nearness),
            value * (0.80 + 0.20 * nearness),
            hovered ? 1.0 : (strong ? 0.55 + 0.40 * nearness : 0.35 + 0.45 * nearness)
        );
        painter.setPen(QPen(outline, hovered ? 1.6 : 1.0));
        painter.setBrush(fill);
        painter.drawEllipse(endpoint.position, radius, radius);

        if (hasLabel) {
            QFont labelFont = painter.font();
            labelFont.setBold(true);
            labelFont.setPointSizeF(strong ? 8.5 : 7.5);
            painter.setFont(labelFont);
            painter.setPen(QColor(
                255,
                255,
                255,
                hovered ? 235 : static_cast<int>(std::lround(185.0f + 50.0f * nearness))
            ));
            painter.drawText(
                QRectF(
                    endpoint.position.x() - radius,
                    endpoint.position.y() - radius,
                    radius * 2.0,
                    radius * 2.0
                ),
                Qt::AlignCenter,
                QString::fromLatin1(info.label)
            );
        }
    }
}


// ========================================================================== //


} // namespace PtclTool
