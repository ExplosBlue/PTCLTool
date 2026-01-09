#include "editor/animGraph.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>


// ========================================================================== //


GraphHandleEditor::GraphHandleEditor(QWidget* parent) :
    QFrame{parent} {
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setFrameShape(QFrame::Box);
    setLineWidth(1);

    mPosition = new QDoubleSpinBox(this);
    mValue = new QDoubleSpinBox(this);

    mPosition->setRange(0.0f, 100.0f);
    mPosition->setDecimals(2);
    mPosition->setSuffix("%");

    mValue->setDecimals(3);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(6, 4, 6, 4);
    layout->addWidget(mPosition);
    layout->addWidget(mValue);

    connect(mPosition, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this] {
        emit valuesEdited(static_cast<f32>(mPosition->value()), static_cast<f32>(mValue->value()));
    });

    connect(mValue, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this] {
        emit valuesEdited(static_cast<f32>(mPosition->value()), static_cast<f32>(mValue->value()));
    });
}


void GraphHandleEditor::setValues(f32 position, f32 value) {
    QSignalBlocker b1(mPosition);
    QSignalBlocker b2(mValue);

    mPosition->setValue(position);
    mValue->setValue(value);
}

void GraphHandleEditor::setValueRange(f32 min, f32 max) {
    mValue->setRange(min, max);
}


// ========================================================================== //


AnimGraph::AnimGraph(QWidget* parent) :
    QWidget{parent} {
    setMinimumSize(200, 200);
}

void AnimGraph::setControlPoints(const PointList& points) {
    mPoints = points;
    update();
};

void AnimGraph::setLineColor(const QColor& color) {
    mLineColor = color;
    update();
};

f32 AnimGraph::chooseTickStep(f32 range) {
    constexpr f32 base = 10.0f;
    constexpr s32 maxTicks = 5;

    f32 step = base;
    while (range > step * static_cast<f32>(maxTicks - 1)) {
        step *= 2.0f;
    }
    return step;
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

void AnimGraph::drawAxisLabels(QPainter& painter, s32 width, s32 height, s32 xDivs, const GraphRange& range) {
    painter.save();

    QFont font = painter.font();
    font.setPointSize(font.pointSize() - 1);
    painter.setFont(font);

    const QFontMetrics fm(font);
    const s32 margin = 4;
    const s32 labelHeight = 16;

    // TODO: choose better color
    painter.setPen(QColor(80, 80, 80));

    const f32 widthF = static_cast<f32>(width);
    const f32 heightF = static_cast<f32>(height);

    for (s32 i = 0; i <= xDivs; ++i) {
        const f32 t = f32(i) / f32(xDivs);
        const s32 x = s32(t * widthF);
        const s32 value = s32(t * 100.0f);

        const QString text = QString::number(value);
        const s32 textW = fm.horizontalAdvance(text);

        painter.drawText(x - textW / 2, height + labelHeight - margin, text);
    }

    const s32 tickCount = static_cast<s32>((range.max - range.min) / range.tickStep);
    for (s32 tick = 0; tick <= tickCount; ++tick) {
        const f32 i = range.min + static_cast<f32>(tick) * range.tickStep;

        const f32 t = (range.max - i) / (range.max - range.min);
        const s32 y = s32(t * heightF);

        const QString text = QString::number(i, 'f', 1);
        const s32 textW = fm.horizontalAdvance(text);
        const s32 textH = fm.height();

        painter.drawText(-textW - margin, y + textH / 2, text);
    }

    painter.restore();
}

void AnimGraph::drawGrid(QPainter& painter, s32 width, s32 height, const GraphRange& range) {
    painter.save();

    constexpr s32 xMajorDivs = 10;
    constexpr s32 xMinorDivs = 3;

    const f32 widthF = static_cast<f32>(width);
    const f32 heightF = static_cast<f32>(height);

    // Background
    painter.fillRect(0, 0, width, height, sColorGridBg);

    // X Minor Grid
    painter.setPen(QPen(sColorGridlineMinor, 1));
    for (int i = 0; i <= xMajorDivs * xMinorDivs; ++i) {
        const f32 t = f32(i) / f32(xMajorDivs * xMinorDivs);
        const s32 x = s32(t * widthF);
        painter.drawLine(x, 0, x, height);
    }

    // X Major Grid
    painter.setPen(QPen(sColorGridlineMajor, 1));
    for (int i = 0; i <= xMajorDivs; ++i) {
        const f32 t = f32(i) / f32(xMajorDivs);
        const s32 x = s32(t * widthF);
        painter.drawLine(x, 0, x, height);
    }

    // Y Major Grid
    painter.setPen(QPen(sColorGridlineMajor, 1));
    const s32 tickCount = static_cast<s32>((range.max - range.min) / range.tickStep);
    for (s32 tick = 0; tick <= tickCount; ++tick) {
        const f32 i = range.min + static_cast<f32>(tick) * range.tickStep;

        const f32 t = (range.max - i) / (range.max - range.min);
        const s32 y = s32(t * heightF);
        painter.drawLine(0, y, width, y);
    }

    painter.restore();
}

s32 AnimGraph::hitTestPoint(const QPoint& mousePos) const {
    const s32 contentW = width() - sPaddingLeft - sPaddingRight;
    const s32 contentH = height() - sPaddingTop - sPaddingBottom;

    const GraphRange range = currentGraphRange();
    const QPoint local = mousePos - QPoint(sPaddingLeft, sPaddingTop);

    for (int i = 0; i < mPoints.size(); ++i) {
        const QPoint p = mapToScreen(mPoints[i], contentW, contentH, range);
        if ((p - local).manhattanLength() <= sHandleRadius + 2) {
            return i;
        }
    }
    return -1;
}

void AnimGraph::showHandleEditor(s32 index) {
    if (index < 0 || index >= mPoints.size()) {
        return;
    }

    if (!mHandleEditor) {
        mHandleEditor = new GraphHandleEditor(this);
        connect(mHandleEditor, &GraphHandleEditor::valuesEdited, this, [this](f32 pos, f32 val) {
            if (mSelectedIdx >= 0) {
                moveHandle(mSelectedIdx, pos, val);
                update();
            }
        });
    }

    const s32 contentW = width() - sPaddingLeft - sPaddingRight;
    const s32 contentH = height() - sPaddingTop - sPaddingBottom;

    const GraphRange range = currentGraphRange();
    const QPoint handlePos = mapToScreen(mPoints[index], contentW, contentH, range) + QPoint(sPaddingLeft, sPaddingTop);

    mHandleEditor->setValueRange(-1000.0f, 1000.0f);
    mHandleEditor->setValues(mPoints[index].position, mPoints[index].value);

    const QPoint globalPos = mapToGlobal(handlePos + QPoint(10, -10));
    mHandleEditor->move(globalPos);
    mHandleEditor->show();
}

void AnimGraph::hideHandleEditor() {
    if (mHandleEditor) {
        mHandleEditor->hide();
    }
}

void AnimGraph::moveHandle(s32 handleIndex, f32 newPos, f32 newValue) {
    // TODO: allow constraints to be defined per instance?
    switch (handleIndex) {
    case 0:
        // Lock start handle pos
        mPoints[0].position = 0.0f;
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
        // Lock end handle pos
        mPoints[3].position = 100.0f;
        mPoints[3].value = newValue;
        break;
    }

    enforceOrdering();
}

void AnimGraph::enforceOrdering() {
    mPoints[0].position = 0.0f;
    mPoints[3].position = 100.0f;

    mPoints[1].position = std::clamp(mPoints[1].position, 0.0f, mPoints[2].position);
    mPoints[2].position = std::clamp(mPoints[2].position, mPoints[1].position, 100.0f);
}

AnimGraph::GraphRange AnimGraph::currentGraphRange() const {
    if (mHasFrozenRange) {
        return mFrozenRange;
    }
    return computeGraphRange();
}

void AnimGraph::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {
        return;
    }

    mSelectedIdx = hitTestPoint(event->pos());
    if (mSelectedIdx == -1) {
        hideHandleEditor();
        return;
    }

    mPressPos = event->pos();
    mPendingClick = true;
    mIsDragging = false;

    mFrozenRange = currentGraphRange();
    mHasFrozenRange = true;

    update();
}

void AnimGraph::mouseReleaseEvent(QMouseEvent* event) {
    mHasFrozenRange = false;

    if (mSelectedIdx >= 0) {
        if (mPendingClick || mDragOccured) {
            showHandleEditor(mSelectedIdx);
        }
    }

    mPendingClick = false;
    mIsDragging = false;
    mDragOccured = false;

    update();
}

void AnimGraph::mouseMoveEvent(QMouseEvent* event) {
    if (mSelectedIdx < 0) {
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
        hideHandleEditor();
    }

    const s32 contentW = width() - sPaddingLeft - sPaddingRight;
    const s32 contentH = height() - sPaddingTop - sPaddingBottom;

    const GraphRange range = currentGraphRange();
    const QPoint local = event->pos() - QPoint(sPaddingLeft, sPaddingTop);

    const f32 contentWF = static_cast<f32>(contentW);
    const f32 contentHF = static_cast<f32>(contentH);

    const f32 x = static_cast<f32>(local.x());
    const f32 y = static_cast<f32>(local.y());

    const f32 t = std::clamp(x / contentWF, 0.0f, 1.0f);
    const f32 value = range.max - std::clamp(y / contentHF, 0.0f, 1.0f) * (range.max - range.min);

    moveHandle(mSelectedIdx, t * 100.0f, value);

    if (mHandleEditor) {
        mHandleEditor->setValues(mPoints[mSelectedIdx].position, mPoints[mSelectedIdx].value);
    }

    update();
}

QPoint AnimGraph::mapToScreen(const GraphPoint& point, s32 contentW, s32 contentH, const GraphRange& range) {
    s32 x = static_cast<s32>(point.position * static_cast<f32>(contentW) * 0.01f);
    s32 y = s32(((range.max - point.value) / (range.max - range.min)) * static_cast<f32>(contentH));

    return { x, y };
}

void AnimGraph::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const s32 contentX = sPaddingLeft;
    const s32 contentY = sPaddingTop;
    const s32 contentW = width() - sPaddingLeft - sPaddingRight;
    const s32 contentH = height() - sPaddingTop - sPaddingBottom;

    painter.save();
    painter.translate(contentX, contentY);

    const GraphRange range = currentGraphRange();

    painter.save();
    painter.setClipRect(0, 0, contentW, contentH);
    drawGrid(painter, contentW, contentH, range);
    painter.restore();

    drawAxisLabels(painter, contentW, contentH, 10, range);

    QVector<QPoint> poly;
    poly.reserve(static_cast<qsizetype>(mPoints.size()));
    for (auto & mPoint : mPoints) {
        poly.push_back(mapToScreen(mPoint, contentW, contentH, range));
    }

    painter.setPen(QPen(mLineColor, 2));
    painter.drawPolyline(QPolygon::fromVector(poly));

    for (s32 i = 0; i < poly.size(); ++i) {
        const auto& pt = poly[i];

        if (i == mSelectedIdx) {
            painter.setPen(QPen(sColorHandleActive, 2));
        } else {
            painter.setPen(QPen(sColorHandle, 2));
        }
        painter.drawEllipse(pt, 4, 4);
    }

    painter.restore();
}

// ========================================================================== //
