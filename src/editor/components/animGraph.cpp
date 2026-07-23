#include "editor/components/animGraph.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QVBoxLayout>
#include <limits>
#include <cmath>


// ========================================================================== //


AnimGraph::AnimGraph(QWidget* parent) :
    QWidget{parent} {
    setMinimumSize(300, 300);
    setMouseTracking(true);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Graph area
    layout->addStretch(1);

    // Control panel on the right
    mControlPanel = new QWidget(this);
    mControlPanel->setAutoFillBackground(true);
    mControlPanel->setFixedWidth(110);

    auto* panelLayout = new QVBoxLayout(mControlPanel);
    panelLayout->setContentsMargins(6, 6, 6, 6);

    mPosLabel = new QLabel("Pos:", mControlPanel);
    mPosLabel->setObjectName("Pos");
    mPositionSpin = new QDoubleSpinBox(mControlPanel);
    mPositionSpin->setRange(0.0, 100.0);
    mPositionSpin->setDecimals(2);
    mPositionSpin->setSuffix("%");
    mPositionSpin->setKeyboardTracking(false);

    mValLabel = new QLabel("Val:", mControlPanel);
    mValueSpin = new QDoubleSpinBox(mControlPanel);
    mValueSpin->setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mValueSpin->setDecimals(3);
    mValueSpin->setKeyboardTracking(false);

    panelLayout->addWidget(mPosLabel);
    panelLayout->addWidget(mPositionSpin);
    panelLayout->addWidget(mValLabel);
    panelLayout->addWidget(mValueSpin);

    auto* zoomLayout = new QHBoxLayout;
    mZoomInBtn = new QPushButton("+", mControlPanel);
    mZoomInBtn->setFixedSize(24, 22);
    mZoomInBtn->setToolTip("Zoom in");
    mZoomOutBtn = new QPushButton(QString::fromUtf8("\u2212"), mControlPanel);
    mZoomOutBtn->setFixedSize(24, 22);
    mZoomOutBtn->setToolTip("Zoom out");
    mZoomFitBtn = new QPushButton(QString::fromUtf8("\u2261"), mControlPanel);
    mZoomFitBtn->setFixedSize(24, 22);
    mZoomFitBtn->setToolTip("Fit all handles");
    zoomLayout->addWidget(mZoomInBtn);
    zoomLayout->addWidget(mZoomOutBtn);
    zoomLayout->addWidget(mZoomFitBtn);
    zoomLayout->addStretch();
    panelLayout->addLayout(zoomLayout);

    panelLayout->addStretch();

    layout->addWidget(mControlPanel);

    connect(mPositionSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double) {
        if (mSelectedIdx >= 0) {
            moveHandle(mSelectedIdx, static_cast<f32>(mPositionSpin->value()), static_cast<f32>(mValueSpin->value()));
            update();
        }
    });

    connect(mValueSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double) {
        if (mSelectedIdx >= 0) {
            moveHandle(mSelectedIdx, static_cast<f32>(mPositionSpin->value()), static_cast<f32>(mValueSpin->value()));
            update();
        }
    });

    connect(mZoomInBtn, &QPushButton::clicked, this, [this]() {
        const auto range = currentGraphRange();
        zoom(1.5f, (range.min + range.max) * 0.5f);
    });

    connect(mZoomOutBtn, &QPushButton::clicked, this, [this]() {
        const auto range = currentGraphRange();
        zoom(1.0f / 1.5f, (range.min + range.max) * 0.5f);
    });

    connect(mZoomFitBtn, &QPushButton::clicked, this, &AnimGraph::zoomToFit);

    updateControls();
}

void AnimGraph::setControlPoints(const PointList& points) {
    mPoints = points;
    update();
}

void AnimGraph::zoomToFit() {
    auto range = computeGraphRange();
    if (mHasValueBounds) {
        range.min = std::max(range.min, mValueBoundsMin);
        range.max = std::min(range.max, mValueBoundsMax);
        if (range.min >= range.max) {
            range.min = mValueBoundsMin;
            range.max = mValueBoundsMax;
        }
    }
    mZoomMin = range.min;
    mZoomMax = range.max;
    mHasZoom = true;
    update();
}

AnimGraph::GraphRect AnimGraph::computeGraphRect() const {
    const s32 panelW = mControlPanel->width();

    return {
        mPaddingLeft,
        sPaddingTop,
        width() - mPaddingLeft - sPaddingRight - panelW,
        height() - sPaddingTop - mPaddingBottom
    };
}

const AnimGraph::PointList& AnimGraph::getPoints() const {
    return mPoints;
}

void AnimGraph::setLineColor(const QColor& color) {
    mLineColor = color;
    update();
};

void AnimGraph::setValueRange(f32 min, f32 max) {
    if (min >= max) {
        return;
    }

    mCustomRange.min = min;
    mCustomRange.max = max;
    mCustomRange.tickStep = chooseTickStep(max - min);

    mHasCustomRange = true;
    update();
}

void AnimGraph::setValueBounds(f32 min, f32 max) {
    if (min >= max) {
        return;
    }

    mValueBoundsMin = min;
    mValueBoundsMax = max;
    mHasValueBounds = true;

    mValueSpin->setRange(static_cast<double>(min), static_cast<double>(max));

    update();
}

void AnimGraph::setTickStepSize(f32 stepSize) {
    mTickStepSize = stepSize;
}

void AnimGraph::setVerticalAxisLabel(const QString& label) {
    mVerticalAxisLabel = label;
    update();
}

void AnimGraph::setFixedLeftPadding(s32 pad) {
    mFixedLeftPad = pad;
    update();
}

void AnimGraph::setPosLabel(const QString& text) {
    if (mPosLabel) {
        mPosLabel->setText(text);
    }
}

void AnimGraph::setValLabel(const QString& text) {
    if (mValLabel) {
        mValLabel->setText(text);
    }
}

f32 AnimGraph::chooseTickStep(f32 range) const {
    constexpr s32 maxTicks = 10;

    f32 step = mTickStepSize;

    if (range / step <= static_cast<f32>(maxTicks)) {
        return step;
    }

    const f32 magnitude = std::pow(10.0f, std::floor(std::log10(range / static_cast<f32>(maxTicks))));

    for (f32 mult : {1.0f, 2.0f, 5.0f}) {
        f32 candidate = magnitude * mult;
        if (range / candidate <= static_cast<f32>(maxTicks)) {
            return std::max(candidate, step);
        }
    }

    return std::max(magnitude * 10.0f, step);
}


AnimGraph::GraphRange AnimGraph::computeGraphRange() const {
    f32 minV = std::numeric_limits<f32>::max();
    f32 maxV = std::numeric_limits<f32>::lowest();

    for (GraphPoint pt : mPoints) {
        minV = std::min(minV, pt.value);
        maxV = std::max(maxV, pt.value);
    }

    if (minV == maxV) {
        minV -= 1.0f;
        maxV += 1.0f;
    }

    // Padding
    const f32 padding = (maxV - minV) * 0.1f;
    const f32 paddedMin = minV - padding;
    const f32 paddedMax = maxV + padding;

    const f32 step = chooseTickStep(paddedMax - paddedMin);

    f32 snappedMin = std::floor(paddedMin / step) * step;
    f32 snappedMax = std::ceil (paddedMax / step) * step;

    return { snappedMin, snappedMax, step };
}

void AnimGraph::drawAxisLabels(QPainter& painter, const DrawContext& ctx) {
    const s32 w = ctx.contentRect.w;
    const s32 h = ctx.contentRect.h;
    const auto& range = ctx.range;
    constexpr s32 xDivs = 10;

    painter.save();

    QFont font = painter.font();
    font.setPointSize(font.pointSize() - 1);
    painter.setFont(font);

    const QFontMetrics fm(font);
    const s32 margin = 4;
    const s32 labelHeight = 16;

    painter.setPen(ctx.axisColor);

    const f32 widthF = static_cast<f32>(w);
    const f32 heightF = static_cast<f32>(h);

    for (s32 i = 0; i <= xDivs; ++i) {
        const f32 t = static_cast<f32>(i) / static_cast<f32>(xDivs);
        const s32 x = static_cast<s32>(t * widthF);
        const s32 value = static_cast<s32>(t * 100.0f);

        const QString text = QString::number(value);
        const s32 textW = fm.horizontalAdvance(text);

        painter.drawText(x - textW / 2, h + labelHeight - margin, text);
    }

    const f32 firstTick = std::ceil(range.min / range.tickStep) * range.tickStep;
    const s32 decimals = std::max(0, static_cast<s32>(-std::floor(std::log10(range.tickStep))));
    const s32 yTickCount = static_cast<s32>((range.max - firstTick) / range.tickStep) + 1;
    for (s32 ti = 0; ti < yTickCount; ++ti) {
        const f32 i = firstTick + static_cast<f32>(ti) * range.tickStep;
        const f32 t = (range.max - i) / (range.max - range.min);
        const s32 y = static_cast<s32>(t * heightF);

        const QString text = QString::number(i, 'f', decimals);
        const s32 textW = fm.horizontalAdvance(text);
        const s32 textH = fm.height();

        painter.drawText(-textW - margin, y + textH / 2, text);
    }

    painter.restore();
}

void AnimGraph::drawGrid(QPainter& painter, const DrawContext& ctx) {
    const s32 w = ctx.contentRect.w;
    const s32 h = ctx.contentRect.h;
    const auto& range = ctx.range;

    painter.save();

    constexpr s32 xMajorDivs = 10;
    constexpr s32 xMinorDivs = 3;

    const f32 widthF = static_cast<f32>(w);
    const f32 heightF = static_cast<f32>(h);

    // Background
    painter.fillRect(0, 0, w, h, ctx.gridBg);

    // X Minor Grid
    painter.setPen(QPen(ctx.gridMinor, 1));
    for (int i = 0; i <= xMajorDivs * xMinorDivs; ++i) {
        const f32 t = static_cast<f32>(i) / static_cast<f32>(xMajorDivs * xMinorDivs);
        const s32 x = static_cast<s32>(t * widthF);
        painter.drawLine(x, 0, x, h);
    }

    // X Major Grid
    painter.setPen(QPen(ctx.gridMajor, 1));
    for (int i = 0; i <= xMajorDivs; ++i) {
        const f32 t = static_cast<f32>(i) / static_cast<f32>(xMajorDivs);
        const s32 x = static_cast<s32>(t * widthF);
        painter.drawLine(x, 0, x, h);
    }

    // Y Major Grid
    painter.setPen(QPen(ctx.gridMajor, 1));
    const f32 firstTick = std::ceil(range.min / range.tickStep) * range.tickStep;
    const s32 yTickCount = static_cast<s32>((range.max - firstTick) / range.tickStep) + 1;
    for (s32 ti = 0; ti < yTickCount; ++ti) {
        const f32 i = firstTick + static_cast<f32>(ti) * range.tickStep;
        const f32 t = (range.max - i) / (range.max - range.min);
        const s32 y = static_cast<s32>(t * heightF);
        painter.drawLine(0, y, w, y);
    }

    painter.restore();
}

QList<s32> AnimGraph::hitTestAll(const QPoint& mousePos) const {
    const auto rect = computeGraphRect();
    const GraphRange range = currentGraphRange();
    const QPoint local = mousePos - QPoint(rect.x, rect.y);

    QList<s32> hits;
    for (size_t i = 0; i < mPoints.size(); ++i) {
        const auto& pt = mPoints[i];
        const QPoint center = mapToScreen(pt, rect.w, rect.h, range);

        if (getHitRect(center, pt.handleType).contains(local)) {
            hits.append(static_cast<s32>(i));
        }
    }

    // Check edge indicators
    for (const auto& ind : mEdgeIndicators) {
        if (QRect(ind.pos.x() - sHandleHitRadius, ind.pos.y() - sHandleHitRadius,
                  sHandleHitRadius * 2, sHandleHitRadius * 2).contains(local)) {
            hits.append(ind.handleIndex);
        }
    }
    return hits;
}

void AnimGraph::updateCursor(const QPoint& pos) {
    const auto rect = computeGraphRect();
    if (rect.contains(pos)) {
        auto hits = hitTestAll(pos);
        setCursor(hits.isEmpty() ? Qt::OpenHandCursor : Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
}

void AnimGraph::updateControls() {
    QSignalBlocker b1(mPositionSpin);
    QSignalBlocker b2(mValueSpin);

    if (mSelectedIdx >= 0 && mSelectedIdx < static_cast<s32>(mPoints.size())) {
        const auto& pt = mPoints[mSelectedIdx];
        mPositionSpin->setValue(pt.position);
        mValueSpin->setValue(pt.value);
        mPositionSpin->setDisabled(pt.handleType == HandleType::Locked);
        mValueSpin->setEnabled(true);
    } else {
        mPositionSpin->setValue(0);
        mValueSpin->setValue(0);
        mPositionSpin->setDisabled(true);
        mValueSpin->setDisabled(true);
    }
}

void AnimGraph::moveHandle(s32 handleIndex, f32 newPos, f32 newValue) {
    if (mHasValueBounds) {
        newValue = std::clamp(newValue, mValueBoundsMin, mValueBoundsMax);
    }

    // TODO: allow constraints to be defined per instance?
    switch (handleIndex) {
    case 0:
        mPoints[0].value = newValue;
        break;
    case 1:
        mPoints[1].position = newPos;
        mPoints[1].value = newValue;
        // Keep handle 1 and 2 in sync
        mPoints[2].value = newValue;
        break;
    case 2:
        mPoints[2].position = newPos;
        mPoints[2].value = newValue;
        // Keep handle 1 and 2 in sync
        mPoints[1].value = newValue;
        break;
    case 3:
        mPoints[3].value = newValue;
        break;
    }

    enforceOrdering(handleIndex);

    emit pointEdited(handleIndex, mPoints[handleIndex]);
}

void AnimGraph::ensureHandleVisible(s32 idx) {
    if (idx < 0 || idx >= static_cast<s32>(mPoints.size())) { return; }

    const auto rect = computeGraphRect();
    const GraphRange range = currentGraphRange();

    const f32 y = ((range.max - mPoints[idx].value) / (range.max - range.min))
                  * static_cast<f32>(rect.h);

    if (y < 0.0f || y > static_cast<f32>(rect.h)) {
        const f32 rangeSize = range.max - range.min;
        mZoomMin = mPoints[idx].value - rangeSize * 0.5f;
        mZoomMax = mPoints[idx].value + rangeSize * 0.5f;

        snapZoom();
        clampZoom();

        mHasZoom = true;
        update();
    }
}

void AnimGraph::enforceOrdering(s32 draggedIndex) {
    mPoints[0].position = 0.0f;
    mPoints[3].position = 100.0f;

    if (mPoints[1].position > mPoints[2].position) {
        if (draggedIndex == 1) {
            mPoints[2].position = mPoints[1].position;
        } else {
            mPoints[1].position = mPoints[2].position;
        }
    }

    mPoints[1].position = std::clamp(mPoints[1].position, 0.0f, 100.0f);
    mPoints[2].position = std::clamp(mPoints[2].position, 0.0f, 100.0f);
}

AnimGraph::GraphRange AnimGraph::currentGraphRange() const {
    if (mHasFrozenRange) {
        return mFrozenRange;
    }

    if (mHasZoom) {
        return { mZoomMin, mZoomMax, chooseTickStep(mZoomMax - mZoomMin) };
    }

    if (mHasCustomRange) {
        return mCustomRange;
    }

    const auto range = computeGraphRange();
    mZoomMin = range.min;
    mZoomMax = range.max;
    mHasZoom = true;
    return range;
}

void AnimGraph::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {
        return;
    }

    auto hits = hitTestAll(event->pos());
    if (hits.isEmpty()) {
        mSelectedIdx = -1;

        const auto rect = computeGraphRect();

        const QPoint pos = event->pos();
        if (pos.x() >= rect.x && pos.x() < rect.x + rect.w &&
            pos.y() >= rect.y && pos.y() < rect.y + rect.h) {
            mIsPanning = true;
            mPressPos = pos;
            mPanAnchorRange = currentGraphRange();
            setCursor(Qt::ClosedHandCursor);
            updateControls();
            update();
            return;
        }

        updateCursor(pos);
        updateControls();
        update();
        return;
    }

    if (hits.contains(mSelectedIdx)) {
        s32 idx = static_cast<s32>(hits.indexOf(mSelectedIdx));
        mSelectedIdx = hits[(idx + 1) % hits.size()];
    } else {
        mSelectedIdx = hits.last();
    }

    ensureHandleVisible(mSelectedIdx);

    mPressPos = event->pos();
    mPendingClick = true;
    mIsDragging = false;

    mFrozenRange = currentGraphRange();
    mHasFrozenRange = true;

    updateControls();
    update();
}

void AnimGraph::mouseReleaseEvent(QMouseEvent* event) {
    if (mIsPanning) {
        snapZoom();
        clampZoom();
    }

    mHasFrozenRange = false;
    mPendingClick = false;
    mIsDragging = false;
    mIsPanning = false;
    mDragOccured = false;

    updateCursor(event->pos());

    updateControls();
    update();
}

void AnimGraph::wheelEvent(QWheelEvent* event) {
    const auto rect = computeGraphRect();

    const QPoint pos = event->position().toPoint();
    if (pos.x() < rect.x || pos.x() >= rect.x + rect.w ||
        pos.y() < rect.y || pos.y() >= rect.y + rect.h) {
        return;
    }

    const QPoint focus = pos - QPoint(rect.x, rect.y);
    const GraphRange range = currentGraphRange();
    const f32 t = static_cast<f32>(focus.y()) / static_cast<f32>(rect.h);
    const f32 focusValue = range.max - t * (range.max - range.min);

    const f32 factor = event->angleDelta().y() > 0 ? 1.2f : 1.0f / 1.2f;
    zoom(factor, focusValue);
    event->accept();
}

void AnimGraph::zoom(f32 factor, f32 focusValue) {
    GraphRange range = currentGraphRange();

    const f32 dMin = focusValue - range.min;
    const f32 dMax = range.max - focusValue;

    f32 newMin = focusValue - dMin / factor;
    f32 newMax = focusValue + dMax / factor;

    constexpr f32 minRange = 0.001f;
    constexpr f32 maxRange = 1e6f;
    if (newMax - newMin < minRange || newMax - newMin > maxRange) {
        return;
    }

    mZoomMin = newMin;
    mZoomMax = newMax;

    clampZoom();

    mHasZoom = true;

    update();
}

void AnimGraph::snapZoom() {
    const f32 tickStep = chooseTickStep(mZoomMax - mZoomMin);

    mZoomMin = std::round(mZoomMin / tickStep) * tickStep;
    mZoomMax = std::round(mZoomMax / tickStep) * tickStep;

    if (mZoomMin >= mZoomMax) {
        mZoomMin -= tickStep * 0.5f;
        mZoomMax += tickStep * 0.5f;
    }
}

void AnimGraph::clampZoom() {
    if (!mHasValueBounds) {
        return;
    }

    const f32 range = mZoomMax - mZoomMin;
    if (range <= 0.0f) {
        return;
    }

    if (mZoomMin < mValueBoundsMin) {
        mZoomMin = mValueBoundsMin;
        mZoomMax = mValueBoundsMin + range;
    }

    if (mZoomMax > mValueBoundsMax) {
        mZoomMax = mValueBoundsMax;
        mZoomMin = mValueBoundsMax - range;
    }

    mZoomMin = std::max(mZoomMin, mValueBoundsMin);
    mZoomMax = std::min(mZoomMax, mValueBoundsMax);
}

void AnimGraph::mouseMoveEvent(QMouseEvent* event) {
    if (!mIsPanning && !mIsDragging && !mPendingClick && !(event->buttons() & Qt::LeftButton)) {
        updateCursor(event->pos());
    }

    if (mIsPanning) {
        const auto rect = computeGraphRect();
        const f32 contentHF = static_cast<f32>(rect.h);
        const f32 dy = static_cast<f32>(event->pos().y() - mPressPos.y());
        const f32 rangeSize = mPanAnchorRange.max - mPanAnchorRange.min;

        mZoomMin = mPanAnchorRange.min + dy / contentHF * rangeSize;
        mZoomMax = mPanAnchorRange.max + dy / contentHF * rangeSize;

        clampZoom();

        mHasZoom = true;

        update();
        return;
    }

    if (mSelectedIdx < 0 || !(event->buttons() & Qt::LeftButton)) {
        return;
    }

    const s32 dragThreshold = QApplication::startDragDistance();

    if (!mIsDragging) {
        if ((event->pos() - mPressPos).manhattanLength() < dragThreshold) {
            return;
        }

        mIsDragging = true;
        mDragOccured = true;
        mPendingClick = false;
    }

    const auto rect = computeGraphRect();
    const GraphRange range = currentGraphRange();
    const QPoint local = event->pos() - QPoint(rect.x, rect.y);

    const f32 contentWF = static_cast<f32>(rect.w);
    const f32 contentHF = static_cast<f32>(rect.h);

    const f32 x = static_cast<f32>(local.x());
    const f32 y = static_cast<f32>(local.y());

    f32 t = std::clamp(x / contentWF, 0.0f, 1.0f);
    f32 value = range.max - std::clamp(y / contentHF, 0.0f, 1.0f) * (range.max - range.min);

    if (event->modifiers() & Qt::ShiftModifier) {
        const f32 snappedValue = std::round(value / range.tickStep) * range.tickStep;
        value = std::clamp(snappedValue, range.min, range.max);
    }


    moveHandle(mSelectedIdx, t * 100.0f, value);

    {
        QSignalBlocker b1(mPositionSpin);
        QSignalBlocker b2(mValueSpin);
        mPositionSpin->setValue(mPoints[mSelectedIdx].position);
        mValueSpin->setValue(mPoints[mSelectedIdx].value);
    }

    update();
}

QPoint AnimGraph::mapToScreen(const GraphPoint& point, s32 contentW, s32 contentH, const GraphRange& range) {
    s32 x = static_cast<s32>(point.position * static_cast<f32>(contentW) * 0.01f);

    const f32 denom = range.max - range.min;
    const f32 raw = (denom != 0.0f) ? (range.max - point.value) / denom : 0.0f;
    const f32 t = std::isfinite(raw) ? raw : 0.0f;
    constexpr f32 maxCoord = 1e6f;
    s32 y = static_cast<s32>(std::clamp(t, -maxCoord, maxCoord) * static_cast<f32>(contentH));

    return { x, y };
}

QPointF AnimGraph::handleWidgetPos(s32 handleIndex) const {
    if (handleIndex < 0 || handleIndex >= static_cast<s32>(mPoints.size())) {
        return {};
    }

    const auto rect = computeGraphRect();
    const auto range = currentGraphRange();
    const QPoint screen = mapToScreen(mPoints[handleIndex], rect.w, rect.h, range);

    return {
        static_cast<qreal>(rect.x + screen.x()),
        static_cast<qreal>(rect.y + screen.y())
    };
}

QPointF AnimGraph::handleVisualPos(s32 handleIndex) const {
    const auto widgetPos = handleWidgetPos(handleIndex);
    const auto rect = computeGraphRect();

    const s32 cy = static_cast<s32>(widgetPos.y()) - rect.y;
    if (cy >= 0 && cy <= rect.h) { return widgetPos; }

    const s32 cx = static_cast<s32>(widgetPos.x()) - rect.x;
    const s32 clampedX = std::clamp(cx, 0, rect.w - 1) + rect.x;
    const s32 edgeY = (cy < 0) ? rect.y : rect.y + rect.h;
    return { static_cast<qreal>(clampedX), static_cast<qreal>(edgeY) };
}

QRect AnimGraph::getHitRect(const QPoint& center, HandleType type) const {
    Q_UNUSED(type);
    return {center.x() - sHandleHitRadius, center.y() - sHandleHitRadius,
            sHandleHitRadius * 2, sHandleHitRadius * 2};
}

void AnimGraph::drawHandle(QPainter& painter, const QPoint& pos, HandleType type, const QColor& color, const QColor& fillColor) const {
    painter.setPen(QPen(color, 2));
    painter.setBrush(fillColor);

    switch (type) {
        case HandleType::Locked: {
            static constexpr s32 r = sHandleRadius - 2;
            painter.drawEllipse(pos, r, r);
            break;
        }
        case HandleType::HoldStart: {
            static constexpr s32 r = sHandleRadius;
            QPolygonF poly;
            poly << QPointF(pos.x() - r, pos.y() - r)
                 << QPointF(pos.x(),      pos.y() - r)
                 << QPointF(pos.x(),      pos.y() + r)
                 << QPointF(pos.x() - r, pos.y());
            painter.drawPolygon(poly);
            break;
        }
        case HandleType::HoldEnd: {
            static constexpr s32 r = sHandleRadius;
            QPolygonF poly;
            poly << QPointF(pos.x() + r, pos.y() - r)
                 << QPointF(pos.x(),      pos.y() - r)
                 << QPointF(pos.x(),      pos.y() + r)
                 << QPointF(pos.x() + r, pos.y());
            painter.drawPolygon(poly);
            break;
        }
    }
}

AnimGraph::DrawContext AnimGraph::buildDrawContext() {
    const auto pal = palette();
    const QColor gridBg = pal.color(QPalette::Base);
    const QColor gridMajor = pal.color(QPalette::Mid);
    QColor gridMinor = gridMajor;
    gridMinor.setAlpha(80);
    const QColor handleNormal = pal.color(QPalette::WindowText);
    const QColor handleSelected = pal.color(QPalette::Highlight);
    const QColor handleFill = pal.color(QPalette::Base);
    QColor axisColor = pal.color(QPalette::WindowText);
    axisColor.setAlpha(160);

    const GraphRange range = currentGraphRange();

    const QFont widgetFont = font();
    QFont labelFont = widgetFont;
    labelFont.setPointSize(labelFont.pointSize() - 1);
    const QFontMetrics lfm(labelFont);

    QFont titleFont = widgetFont;
    titleFont.setPointSize(titleFont.pointSize() + 1);
    const QFontMetrics tfm(titleFont);

    if (widgetFont != mCachedWidgetFont) {
        mCachedWidgetFont = widgetFont;
        const f32 firstTick = std::ceil(range.min / range.tickStep) * range.tickStep;
        const s32 decimals = std::max(0, static_cast<s32>(-std::floor(std::log10(range.tickStep))));
        s32 maxW = 0;
        const s32 yTickCount = static_cast<s32>((range.max - firstTick) / range.tickStep) + 1;
        for (s32 ti = 0; ti < yTickCount; ++ti) {
        const f32 i = firstTick + static_cast<f32>(ti) * range.tickStep;
            const QString text = QString::number(i, 'f', decimals);
            maxW = std::max(maxW, lfm.horizontalAdvance(text));
        }
        mCachedMaxYTickW = maxW;
    }

    const s32 bottomPad = lfm.height() + 8 + tfm.height() + 10;

    s32 leftPad = mCachedMaxYTickW + 8;
    if (!mVerticalAxisLabel.isEmpty()) {
        leftPad += tfm.height() + 4;
    }
    leftPad = std::max<s32>(leftPad, mPaddingLeft);
    if (mFixedLeftPad > 0) {
        leftPad = mFixedLeftPad;
    }

    mPaddingLeft = leftPad;
    mPaddingBottom = bottomPad;

    const auto rect = computeGraphRect();

    return {
        range,
        rect,
        gridBg,
        gridMajor,
        gridMinor,
        handleNormal,
        handleSelected,
        handleFill,
        axisColor,
        titleFont,
        tfm
    };
}

void AnimGraph::drawGraphLine(QPainter& painter, const DrawContext& ctx, const QVector<QPoint>& poly) {
    Q_UNUSED(ctx);
    for (qsizetype i = 0; i + 1 < poly.size(); ++i) {
        const QPoint& start = poly[i];
        const QPoint& end = poly[i + 1];

        QPen pen(mLineColor, 2);

        if (mPoints[i].handleType == HandleType::HoldStart &&
            mPoints[i + 1].handleType == HandleType::HoldEnd) {
            pen.setStyle(Qt::DashDotLine);
        } else {
            pen.setStyle(Qt::SolidLine);
        }

        painter.setPen(pen);
        painter.drawLine(start, end);
    }
}

void AnimGraph::drawHandles(QPainter& painter, const DrawContext& ctx, const QVector<QPoint>& poly) {
    for (auto i = 0; i < poly.size(); ++i) {
        const QPoint& screen = poly[i];
        if (screen.y() >= 0 && screen.y() <= ctx.contentRect.h) {
            const bool isSelected = (i == mSelectedIdx);
            drawHandle(painter, screen, mPoints[i].handleType,
                       isSelected ? ctx.handleSelected : ctx.handleNormal,
                       ctx.handleFill);
        }
    }
}

QVector<QPoint> AnimGraph::buildPolyline(const DrawContext& ctx) {
    QVector<QPoint> poly;
    poly.reserve(static_cast<qsizetype>(mPoints.size()));
    for (auto & mPoint : mPoints) {
        poly.push_back(mapToScreen(mPoint, ctx.contentRect.w, ctx.contentRect.h, ctx.range));
    }
    return poly;
}

void AnimGraph::drawEdgeIndicators(QPainter& painter, const DrawContext& ctx, const QVector<QPoint>& poly) {
    mEdgeIndicators.clear();
    for (s32 i = 0; i < static_cast<s32>(mPoints.size()); ++i) {
        const QPoint screen = poly[i];
        if (screen.y() < 0 || screen.y() > ctx.contentRect.h) {
            const s32 x = std::clamp(screen.x(), 0, ctx.contentRect.w - 1);
            const s32 y = (screen.y() < 0) ? 0 : ctx.contentRect.h;
            mEdgeIndicators.append({QPoint(x, y), i});

            painter.setPen(Qt::NoPen);
            painter.setBrush((i == mSelectedIdx) ? ctx.handleSelected : ctx.handleNormal);
            painter.drawEllipse(QPointF(x, y), 4.0, 4.0);
        }
    }
}

void AnimGraph::drawAxisTitles(QPainter& painter, const DrawContext& ctx) {
    painter.setFont(ctx.titleFont);
    painter.setPen(ctx.axisColor);

    const QString hLabel = QStringLiteral("Lifetime (%)");
    const QFontMetrics hfm(ctx.titleFont);
    painter.drawText(ctx.contentRect.w / 2 - hfm.horizontalAdvance(hLabel) / 2, ctx.contentRect.h + mPaddingBottom - 14, hLabel);

    if (!mVerticalAxisLabel.isEmpty()) {
        painter.save();
        const QFontMetrics vfm(ctx.titleFont);
        const s32 textW = vfm.horizontalAdvance(mVerticalAxisLabel);
        painter.translate(-mPaddingLeft + ctx.titleMetrics.ascent() + 4, static_cast<f32>(ctx.contentRect.h) / 2 + static_cast<f32>(textW) / 2);
        painter.rotate(-90.0);
        painter.drawText(0, 0, mVerticalAxisLabel);
        painter.restore();
    }
}

void AnimGraph::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    DrawContext ctx = buildDrawContext();

    painter.save();
    painter.translate(ctx.contentRect.x, ctx.contentRect.y);

    painter.save();
    painter.setClipRect(0, 0, ctx.contentRect.w, ctx.contentRect.h);
    drawGrid(painter, ctx);

    const auto poly = buildPolyline(ctx);
    drawGraphLine(painter, ctx, poly);
    painter.restore();

    drawAxisLabels(painter, ctx);
    drawHandles(painter, ctx, poly);
    drawEdgeIndicators(painter, ctx, poly);
    drawAxisTitles(painter, ctx);

    painter.restore();
}

// ========================================================================== //
