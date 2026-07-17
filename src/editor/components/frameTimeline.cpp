#include "editor/components/frameSelectorPopup.h"
#include "editor/components/frameTimeline.h"
#include "editor/components/viewportScrollBar.h"

#include "util/paintUtil.h"

#include <QMouseEvent>
#include <QPainter>
#include <utility>


namespace PtclEditor {


// ========================================================================== //


bool FrameTimelineModel::isEffectivelyStatic() const {
    return mode == FrameAnimMode::FitToLife && ptclLife >= sLifeInfinite;
}

s32 FrameTimelineModel::cycleLength() const {
    if (frameCount <= 0 || frameStep <= 0) {
        return 0;
    }

    return frameStep * frameCount;
}

s32 FrameTimelineModel::totalTicks() const {
    if (frameCount <= 0) {
        return 0;
    }

    switch (mode) {
        case PtclEditor::FrameAnimMode::FixedSpeed: return totalTicksFixedSpeed();
        case PtclEditor::FrameAnimMode::FitToLife:  return totalTicksFitToLife();
        default: std::unreachable();
    }

    return 0;
}

s32 FrameTimelineModel::activeDurationTicks() const {
    if (mode == FrameAnimMode::FixedSpeed) {
        return std::min(ptclLife, cycleLength());
    }
    return ptclLife;
}

s32 FrameTimelineModel::totalTicksFixedSpeed() const {
    const s32 cycle = cycleLength();

    if (ptclLife >= sLifeInfinite) {
        return std::max(cycle, cycle * 3);
    }
    return std::max(ptclLife, cycle);
}

s32 FrameTimelineModel::totalTicksFitToLife() const {
    if (isEffectivelyStatic()) {
        return frameCount;
    }

    return std::max(ptclLife, frameCount);
}

s32 FrameTimelineModel::ticksForFrame(s32 frameIdx) const {
    if (frameCount <= 0 || frameIdx < 0 || frameIdx >= frameCount) {
        return 0;
    }

    switch (mode) {
        case PtclEditor::FrameAnimMode::FixedSpeed: return frameStep;
        case PtclEditor::FrameAnimMode::FitToLife:  return cellStartTick(frameIdx + 1) - cellStartTick(frameIdx);
        default: std::unreachable();
    }

    return 0;
}

s32 FrameTimelineModel::cellStartTick(s32 frameIdx) const {
    if (frameIdx <= 0) {
        return 0;
    }

    switch (mode) {
        case PtclEditor::FrameAnimMode::FixedSpeed: return frameStartTickFixedSpeed(frameIdx);
        case PtclEditor::FrameAnimMode::FitToLife:  return frameStartTickFitToLife(frameIdx);
        default: std::unreachable();
    }

    return 0;
}

s32 FrameTimelineModel::frameStartTickFixedSpeed(s32 frameIdx) const {
    if (frameIdx <= 0) {
        return 0;
    }

    if (frameIdx >= frameCount) {
        return cycleLength();
    }

    return frameIdx * frameStep;
}

s32 FrameTimelineModel::frameStartTickFitToLife(s32 frameIdx) const {
    if (frameIdx <= 0) {
        return 0;
    }

    if (frameIdx >= frameCount) {
        return totalTicks();
    }

    if (isEffectivelyStatic()) {
        return frameIdx;
    }

    const s32 range = totalTicks();
    return (frameIdx * range + frameCount - 1) / frameCount;
}


// ========================================================================== //


FrameTimeline::FrameTimeline(QWidget* parent) :
    QWidget{parent} {

    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    mScrollBar = new ViewportScrollBar(this);

    connect(mScrollBar, &ViewportScrollBar::zoomChanged, this, [this](s32 contentWidth, s32 newOffset) {
        const f32 prevPixelPerTick = mPixelsPerTick;
        updateZoom(contentWidth);
        if (mPixelsPerTick != prevPixelPerTick) {
            setScrollOffset(std::max(0, newOffset));
            updateScrollBar();
        }
    });

    connect(mScrollBar, &ViewportScrollBar::scrollChanged, this, [this](s32 newOffset) {
        setScrollOffset(newOffset);
    });
}

void FrameTimeline::setSource(const TextureFrameInfo& frameInfo) {
    mFrameInfo = frameInfo;
    mFrameInfo.divX = std::max(1, mFrameInfo.divX);
    mFrameInfo.divY = std::max(1, mFrameInfo.divY);
    mFrameInfo.repX = std::max(1, mFrameInfo.repX);
    mFrameInfo.repY = std::max(1, mFrameInfo.repY);
    mModel.maxFrameId = std::max(1, mFrameInfo.divX * mFrameInfo.divY);
    update();
}

void FrameTimeline::setFrames(const std::vector<s32>& frames, s32 frameCount) {
    mModel.frameTable = frames;
    mModel.frameCount = frameCount;
    adjustContent();
}

void FrameTimeline::setAnimMode(FrameAnimMode mode) {
    mModel.mode = mode;
    adjustContent();
}

void FrameTimeline::setFrameStep(s32 step) {
    mModel.frameStep = step;
    adjustContent();
}

void FrameTimeline::setPtclLife(s32 life) {
    mModel.ptclLife = std::max(1, life);
    adjustContent();
}

QSize FrameTimeline::sizeHint() const {
    return {
        sThumbSize + sHorizontalPadding * 2,
        sRulerHeight + sTimelineHeight + sScrollbarHeight
    };
}

QSize FrameTimeline::minimumSizeHint() const {
    return sizeHint();
}

void FrameTimeline::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    mScrollBar->setGeometry(0, height() - sScrollbarHeight, width(), sScrollbarHeight);
    fitToWidth();
}

// Drawing

void FrameTimeline::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::Antialiasing, false);

    const QPalette& pal = palette();
    const QColor base = pal.color(QPalette::Base);

    const s32 paintH = std::max(0, height() - sScrollbarHeight);

    painter.fillRect(0, 0, width(), paintH, base);

    const s32 count = mModel.frameCount;
    if (count <= 0) {
        return;
    }

    drawTimelinePadding(painter, paintH);

    const s32 visStart = mScrollOffset;
    const s32 visEnd = mScrollOffset + width();

    const s32 trackY = sRulerHeight;
    const s32 trackH = paintH - sRulerHeight;

    drawGrid(painter, trackY, trackH, visStart, visEnd);
    drawRuler(painter, visStart, visEnd);
    drawTimeline(painter, trackY, trackH);

    if (mModel.isEffectivelyStatic()) {
        drawInfinityOverlay(painter, trackY, trackH);
    }
}

void FrameTimeline::drawTimeline(QPainter& painter, s32 trackY, s32 trackH) {
    if (mModel.isEffectivelyStatic()) {
        drawTimelineStatic(painter, trackY, trackH);
        return;
    }

    switch (mModel.mode) {
    case FrameAnimMode::FixedSpeed:
        drawTimelineFixed(painter, trackY, trackH);
        break;
    case FrameAnimMode::FitToLife:
        drawTimelineFit(painter, trackY, trackH);
        break;
    default:
        std::unreachable();
    }
}

void FrameTimeline::drawTimelineStatic(QPainter& painter, s32 trackY, s32 trackH) {
    const s32 count = mModel.frameCount;

    for (s32 i = 0; i < count; ++i) {
        drawTimelineCell(painter, trackY, trackH, {
            .startTick = mModel.cellStartTick(i),
            .endTick = mModel.cellStartTick(i + 1),
            .frameIndex = i,
            .isStatic = true,
        });
    }
}

void FrameTimeline::drawTimelineFixed(QPainter& painter, s32 trackY, s32 trackH) {
    const s32 cycleLen = mModel.cycleLength();
    if (cycleLen <= 0) {
        return;
    }

    const s32 totalTicks = mModel.totalTicks();
    const s32 numCycles = (totalTicks + cycleLen - 1) / cycleLen;

    for (s32 cycle = 0; cycle < numCycles; ++cycle) {
        drawFrameCycle(painter, trackY, trackH, {
            .startTick = cycle * cycleLen,
            .endTick = std::min((cycle + 1) * cycleLen, totalTicks),
            .index = cycle
        });
    }

    drawLifeBoundary(painter, trackY, trackH);
    drawInfinityMarker(painter, trackY, trackH);
}

void FrameTimeline::drawTimelineFit(QPainter& painter, s32 trackY, s32 trackH) {
    const s32 count = mModel.frameCount;

    const s32 lifeTick = mModel.ptclLife;
    const bool finiteLife = lifeTick < sLifeInfinite;

    for (s32 i = 0; i < count; ++i) {
        const s32 cellTickStart = mModel.cellStartTick(i);
        const s32 cellTickEnd = mModel.cellStartTick(i + 1);
        if (cellTickStart >= cellTickEnd) {
            continue;
        }

        CellDrawInfo info{
            .startTick  = cellTickStart,
            .endTick    = cellTickEnd,
            .frameIndex = i
        };

        if (finiteLife && cellTickEnd > lifeTick) {
            info.hasLifetimeSplit = true;
            info.lifetimeTick = lifeTick;
        }

        drawTimelineCell(painter, trackY, trackH, info);
    }

    drawLifeBoundary(painter, trackY, trackH);
}

void FrameTimeline::drawFrameCycle(QPainter& painter, s32 trackY, s32 trackH, const FrameCycle& cycle) {
    if (!isTickRangeVisible(cycle.startTick, cycle.endTick)) {
        return;
    }

    const s32 lifeTick = mModel.ptclLife;
    const bool finiteLife = lifeTick < sLifeInfinite;

    for (s32 frame = 0; frame < mModel.frameCount; ++frame) {

        CellDrawInfo info{
            .startTick = cycle.startTick + mModel.cellStartTick(frame),
            .endTick = std::min(cycle.startTick + mModel.cellStartTick(frame + 1), cycle.endTick),
            .frameIndex = frame,
            .isGhost = cycle.isGhost(),
            .repetition = cycle.index
        };

        if (info.startTick >= info.endTick) {
            continue;
        }

        if (finiteLife && cycle.index == 0 && info.endTick > lifeTick) {
            info.hasLifetimeSplit = true;
            info.lifetimeTick = lifeTick;
        }

        drawTimelineCell(painter, trackY, trackH, info);
    }
}

void FrameTimeline::drawTimelineCell(QPainter& painter, s32 trackY, s32 trackH, const CellDrawInfo& drawInfo) {
    const s32 contentStart = contentTickX(drawInfo.startTick);
    const s32 contentEnd   = contentTickX(drawInfo.endTick);

    if (contentEnd <= 0 || contentStart >= width()) {
        return;
    }

    const s32 screenX = contentStart;
    const s32 screenWidth = contentEnd - contentStart;

    if (screenWidth <= 4) {
        return;
    }

    const CellStyle style = getCellStyle(drawInfo);
    const CellLayout layout = makeCellLayout(screenX, screenWidth, trackY, trackH);

    painter.save();

    // Cell
    drawCellBackground(painter, layout.cellRect, style, drawInfo);

    // Selection Highlight
    const bool popupSelected =
        !drawInfo.isGhost &&
        !drawInfo.isStatic &&
        drawInfo.frameIndex == mPopupCellIndex;

    if (popupSelected) {
        painter.setPen(QPen(palette().color(QPalette::Highlight), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(layout.cellRect);
    }

    // Thumbnail
    const s32 frame = (!drawInfo.isGhost && drawInfo.frameIndex == mPopupCellIndex && mPopupPreviewFrame >= 0)
        ? mPopupPreviewFrame : mModel.frameTable[drawInfo.frameIndex];

    if (style.thumbnailOpacity != 1.0f) {
        painter.save();
    }

    QPixmap thumb = PaintUtil::renderTextureFrame(mFrameInfo, frame, sThumbSize);
    painter.setOpacity(style.thumbnailOpacity);
    painter.drawPixmap(layout.thumbPos, thumb);

    if (style.thumbnailOpacity != 1.0f) {
        painter.restore();
    }

    // Text
    static QFont cellFont;
    static bool initialized = false;

    if (!initialized) {
        cellFont = font();
        cellFont.setPixelSize(10);
        initialized = true;
    }

    painter.setPen(style.text);
    painter.setFont(cellFont);
    painter.drawText(layout.textRect, Qt::AlignCenter, QString::number(frame));

    painter.restore();
};

void FrameTimeline::drawCellBackground(QPainter& painter, const QRect& rect, const CellStyle& style, const CellDrawInfo& drawInfo) {
    painter.save();

    if (!drawInfo.hasLifetimeSplit) {
        painter.setPen(QPen(style.border));
        painter.setBrush(style.background);
        painter.drawRect(rect);
        painter.restore();
        return;
    }

    const s32 splitX = std::clamp(contentTickX(drawInfo.lifetimeTick), rect.left(), rect.right());

    QRect before{
        rect.left(),
        rect.top(),
        splitX - rect.left(),
        rect.height()
    };

    QRect after{
        splitX,
        rect.top(),
        rect.right() - splitX + 1,
        rect.height()
    };

    if (before.width() > 0) {
        painter.setPen(style.border);
        painter.setBrush(style.background);
        painter.drawRect(before);
    }

    if (after.width() > 0) {
        QColor warningBg = sWarnColor;
        warningBg.setAlpha(25);

        QColor warningBorder = sWarnColor;
        warningBorder.setAlpha(80);

        painter.setPen(warningBorder);
        painter.setBrush(warningBg);
        painter.drawRect(after);
    }

    painter.restore();
}

FrameTimeline::CellStyle FrameTimeline::getCellStyle(const CellDrawInfo& info) const {
    const QPalette& pal = palette();

    CellStyle style {
        .background = pal.color(QPalette::Highlight),
        .border = pal.color(QPalette::Mid),
        .text = pal.color(QPalette::Text)
    };

    style.background.setAlpha(50);

    if (info.isGhost) {
        style.background.setAlpha(info.repetition % 2 ? 15 : 30);
        style.border.setAlpha(info.repetition % 2 ? 50 : 70);
        style.text = pal.color(QPalette::Disabled, QPalette::Text);
    }
    else if (info.isStatic) {
        style.background.setAlpha(20);
        style.border = pal.color(QPalette::Disabled, QPalette::Mid);
        style.text = pal.color(QPalette::Disabled, QPalette::Text);
        style.thumbnailOpacity = 0.35f;
    }

    return style;
}

FrameTimeline::CellLayout FrameTimeline::makeCellLayout(s32 screenX, s32 screenWidth, s32 trackY, s32 trackH) const {
    static const s32 padding = 3;

    const s32 thumbAreaH = static_cast<s32>(static_cast<f32>(trackH) * 0.7f);
    const s32 cellY = trackY + (trackH - thumbAreaH) / 2;

    QRect cellRect{screenX, cellY, screenWidth, thumbAreaH};

    QPoint thumbPos{
        cellRect.x() + (cellRect.width() - sThumbSize) / 2,
        cellRect.y() + padding
    };

    QRect textRect{
        cellRect.x(),
        thumbPos.y() + sThumbSize + 2,
        cellRect.width(),
        cellRect.bottom() - (thumbPos.y() + sThumbSize + 2) + 1
    };

    return {
        .cellRect = cellRect,
        .thumbPos = thumbPos,
        .textRect = textRect
    };
}

void FrameTimeline::drawGrid(QPainter& painter, s32 trackY, s32 trackH, s32 visStart, s32 visEnd) {
    painter.save();

    const QColor& baseColor = palette().color(QPalette::Mid);

    QColor majorColor = baseColor;
    majorColor.setAlpha(80);

    QColor minorColor = baseColor;
    minorColor.setAlpha(60);

    const QPen majorPen{majorColor, 1};
    const QPen minorPen{minorColor, 1};

    const s32 bottom = trackY + trackH;

    forEachVisibleTick(visStart, visEnd, [&](s32 tick, s32 x, bool isMajor) {
        Q_UNUSED(tick);
        painter.setPen(isMajor ? majorPen : minorPen);
        painter.drawLine(x, trackY, x, bottom);
    });

    painter.restore();
}

void FrameTimeline::drawRuler(QPainter& painter, s32 visStart, s32 visEnd) {
    painter.save();

    const QPalette& pal = palette();

    const QColor headerText = pal.color(QPalette::Text);
    const QColor borderColor = pal.color(QPalette::Mid);
    const QColor backgroundColor = pal.color(QPalette::Window);

    painter.fillRect(0, 0, width(), sRulerHeight, backgroundColor);

    drawRulerActiveRange(painter);

    painter.setPen(borderColor);
    painter.drawLine(0, sRulerHeight - 1, width(), sRulerHeight - 1);

    QFont rulerFont = font();
    rulerFont.setPixelSize(9);
    painter.setFont(rulerFont);

    forEachVisibleTick(visStart, visEnd, [&](s32 tick, s32 tickScreenX, bool isMajor) {
        if (isMajor) {
            painter.setPen(borderColor);
            painter.drawLine(tickScreenX, 0, tickScreenX, sRulerHeight - 1);

            painter.setPen(headerText);
            painter.drawText(QRect(tickScreenX - 20, 0, 40, sRulerHeight), Qt::AlignCenter, QString::number(tick));
        } else {
            painter.setPen(borderColor);
            painter.drawLine(tickScreenX, sRulerHeight - 5, tickScreenX, sRulerHeight - 1);
        }
    });

    painter.restore();
}

void FrameTimeline::drawRulerActiveRange(QPainter& painter) {
    const QPalette& pal = palette();

    const s32 totalTicks = mModel.totalTicks();
    if (totalTicks <= 0 || mPixelsPerTick <= 0) {
        return;
    }

    const s32 activeEnd = std::clamp(mModel.activeDurationTicks(), 0, totalTicks);

    const s32 activeBarY = sRulerHeight - 4;
    const s32 activeBarHeight = 3;
    const s32 contentStartPx = viewX(sSidePadding);
    const s32 activeX = contentTickX(activeEnd);
    const s32 contentEndPx = contentTickX(totalTicks);

    const s32 left = std::max(contentStartPx, 0);
    const s32 right = std::min(contentEndPx, width());
    if (right <= left) {
        return;
    }

    const QColor activeColor = pal.color(QPalette::Highlight);
    const s32 activeClipped = std::clamp(activeX, left, right);
    if (activeClipped > left) {
        painter.fillRect(left, activeBarY, activeClipped - left, activeBarHeight, activeColor);
    }

    QColor inactiveColor = pal.color(QPalette::Highlight);
    inactiveColor.setAlpha(60);
    if (right > activeClipped) {
        painter.fillRect(activeClipped, activeBarY, right - activeClipped, activeBarHeight, inactiveColor);
    }
}

void FrameTimeline::drawLifeBoundary(QPainter& painter, s32 trackY, s32 trackH) {
    const s32 lifeTick = mModel.ptclLife;
    const s32 totalTicks = mModel.totalTicks();
    if (lifeTick >= sLifeInfinite || lifeTick >= totalTicks) {
        return;
    }

    const s32 lifeBoundaryX = viewX(sSidePadding + tickToPixel(lifeTick));
    if (lifeBoundaryX < 0 || lifeBoundaryX > width()) {
        return;
    }

    painter.setPen(QPen(sWarnColor, 2, Qt::DashLine));
    painter.drawLine(lifeBoundaryX, trackY, lifeBoundaryX, trackY + trackH);
}

void FrameTimeline::drawTimelinePadding(QPainter& painter, s32 regionHeight) {
    painter.save();

    const QColor dark = palette().color(QPalette::Dark);

    auto drawPadding = [&](s32 start, s32 end) {
        start = std::max(0, start);
        end = std::clamp(end, 0, width());

        if (end > start) {
            painter.fillRect(start, 0, end - start, regionHeight, dark);
        }
    };

    const s32 contentStart = viewX(0);
    const s32 contentEnd = viewX(contentWidth() - sSidePadding);

    drawPadding(contentStart, contentStart + sSidePadding);
    drawPadding(contentEnd - sSidePadding, contentEnd);

    painter.restore();
}

void FrameTimeline::drawInfinityOverlay(QPainter& painter, s32 trackY, s32 trackH) {
    painter.save();

    painter.fillRect(0, trackY, width(), trackH, QColor(0, 0, 0, 80));

    const s32 midY = trackY + trackH / 2;

    QFont bigFont = font();
    bigFont.setPixelSize(20);
    bigFont.setBold(true);

    painter.setPen(Qt::white);
    painter.setFont(bigFont);

    painter.drawText(QRect(0, midY - 32, width(), 28), Qt::AlignCenter, QStringLiteral("Lifetime is infinite"));

    QFont smallFont = font();
    smallFont.setPixelSize(14);

    painter.setFont(smallFont);

    painter.drawText(QRect(0, midY + 2, width(), 22), Qt::AlignCenter, QStringLiteral("Animation will be static"));

    painter.restore();
}

void FrameTimeline::drawInfinityMarker(QPainter& painter, s32 trackY, s32 trackH) {
    if (mModel.ptclLife < sLifeInfinite) {
        return;
    }

    const s32 contentEnd = contentTickX(mModel.totalTicks());
    const s32 markerX = viewX(contentEnd) + 4;

    painter.save();

    if (markerX >= 0 && markerX < width()) {
        painter.setPen(palette().color(QPalette::Text));
        QFont bigFont = font();
        bigFont.setPixelSize(16);
        painter.setFont(bigFont);
        painter.drawText(QRect(markerX, trackY, 30, trackH), Qt::AlignLeft | Qt::AlignVCenter, QString("Inf"));
    }

    painter.restore();
}

// Interaction

void FrameTimeline::wheelEvent(QWheelEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        zoomAt(static_cast<s32>(event->position().x()), event->angleDelta().y() > 0);
    } else {
        const s32 delta = event->angleDelta().y() + event->angleDelta().x();
        setScrollOffset(mScrollOffset - delta);
    }
    event->accept();
}

void FrameTimeline::mousePressEvent(QMouseEvent* event) {
    if (mModel.isEffectivelyStatic()) {
        return;
    }

    if (event->button() != Qt::LeftButton) {
        return;
    }

    const auto& pos = event->pos();
    if (!inTimelineArea(pos)) {
        return;
    }

    const s32 cellIndex = cellAt(pos);
    if (cellIndex >= 0 && cellIndex < mModel.frameCount) {
        showFrameSelector(cellIndex);
    }
}

void FrameTimeline::mouseMoveEvent(QMouseEvent* event) {
    unsetCursor();

    const auto& pos = event->pos();
    if (!(inTimelineArea(pos))) {
        setToolTip({});
        return;
    }

    if (mModel.frameCount <= 0 || mModel.isEffectivelyStatic()) {
        setToolTip({});
        return;
    }

    const s32 contentX = pos.x() + mScrollOffset;

    QString tip = repetitionTooltip(contentX);
    if (tip.isEmpty()) {
        tip = boundaryTooltip(pos, contentX);
    }

    setToolTip(tip);
}

void FrameTimeline::showFrameSelector(s32 cellIndex) {
    if (cellIndex < 0 || cellIndex >= mModel.frameCount) {
        return;
    }

    const QRect cell = cellRect(cellIndex);
    if (!cell.isValid()) {
        return;
    }

    auto* popup = new FrameSelectorPopup(
        mFrameInfo, {
            .maxFrame = mModel.maxFrameId,
            .currentFrame = mModel.frameTable[cellIndex],
            .slotIndex = cellIndex,
            .anchorGlobal = mapToGlobal(cell.center())
        });

    connect(popup, &FrameSelectorPopup::frameSelected, this, [this, cellIndex](s32 frame) {
        mModel.frameTable[cellIndex] = frame;
        update();
        emit frameEdited(cellIndex, frame);
    });

    popup->show();
}

bool FrameTimeline::inTimelineArea(const QPoint& pos) const {
    return pos.y() >= sRulerHeight && pos.y() < sRulerHeight + sTimelineHeight;
}

QRect FrameTimeline::cellRect(s32 index) const {
    if (mModel.frameCount <= 0 || index < 0 || index >= mModel.frameCount) {
        return {};
    }

    const s32 start = tickToPixel(mModel.cellStartTick(index));
    const s32 end   = tickToPixel(mModel.cellStartTick(index + 1));

    return {
        viewX(sSidePadding + start),
        sRulerHeight,
        end - start,
        sTimelineHeight
    };
}

s32 FrameTimeline::cellAt(const QPoint& pos) const {
    if (pos.y() < sRulerHeight || pos.y() >= sRulerHeight + sTimelineHeight) {
        return sInvalidCell;
    }

    if (mPixelsPerTick <= 0.0f) {
        return sInvalidCell;
    }

    const s32 contentX = pos.x() + mScrollOffset;

    const f32 tick = static_cast<f32>(contentX - sSidePadding) / mPixelsPerTick;

    if (tick < 0.0f) {
        return sInvalidCell;
    }

    if (mModel.mode == FrameAnimMode::FixedSpeed) {
        const s32 frame = static_cast<s32>(tick) / mModel.frameStep;
        return frame < mModel.frameCount ? frame : sInvalidCell;
    }

    // FitToLife
    const s32 targetTick = static_cast<s32>(tick);

    s32 lo = 0;
    s32 hi = mModel.frameCount;

    while (lo < hi) {
        const s32 mid = (lo + hi) / 2;

        if (mModel.cellStartTick(mid + 1) <= targetTick) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    return lo < mModel.frameCount ? lo : sInvalidCell;
}

// Tooltip

QString FrameTimeline::repetitionTooltip(s32 contentX) const {
    if (mModel.mode != FrameAnimMode::FixedSpeed) {
        return {};
    }

    const s32 cycleLen = mModel.cycleLength();
    if (cycleLen <= 0) {
        return {};
    }

    const s32 cyclePixels = tickToPixel(cycleLen);
    const s32 firstCycleEnd = sSidePadding + cyclePixels;
    if (contentX < firstCycleEnd) {
        return {};
    }

    const s32 cycleNum = (contentX - firstCycleEnd) / cyclePixels + 2;
    return QString("Cycle %1 — repetition").arg(cycleNum);
}

QString FrameTimeline::boundaryTooltip(const QPoint& pos, s32 contentX) const {
    if (!(mModel.ptclLife < sLifeInfinite)) {
        return {};
    }

    const s32 cellIndex = cellAt(pos);
    if (cellIndex < 0 || cellIndex >= mModel.frameCount) {
        return {};
    }

    const s32 boundaryPx = sSidePadding + tickToPixel(mModel.ptclLife);
    if (contentX >= boundaryPx) {
        return "Frame is beyond particle lifespan - will be not played";
    }

    return {};
}

// View

void FrameTimeline::updateZoom(s32 contentWidth) {
    const s32 ticks = mModel.totalTicks();
    if (ticks <= 0) {
        mPixelsPerTick = minimumPixelsPerTick();
        return;
    }

    const s32 usableWidth = std::max(0, contentWidth - sSidePadding * 2);
    mPixelsPerTick = static_cast<f32>(usableWidth) / static_cast<f32>(ticks);
    mPixelsPerTick = std::clamp(mPixelsPerTick, minimumPixelsPerTick(), sMaxPixelsPerTick);
}

void FrameTimeline::zoomAt(s32 screenX, bool zoomIn) {
    if (contentWidth() <= 0) {
        return;
    }

    const f32 oldPixelsPerTick = mPixelsPerTick;
    if (oldPixelsPerTick <= 0.0f) {
        return;
    }

    const f32 oldOffset = static_cast<f32>(mScrollOffset);

    if (zoomIn) {
        mPixelsPerTick = std::min(sMaxPixelsPerTick, mPixelsPerTick * 1.5f);
    } else {
        mPixelsPerTick = std::max(minimumPixelsPerTick(), mPixelsPerTick / 1.5f);
    }

    const f32 ratio = mPixelsPerTick / oldPixelsPerTick;
    const s32 newOffset = static_cast<s32>((static_cast<f32>(screenX) + oldOffset) * ratio - static_cast<f32>(screenX));
    setScrollOffset(newOffset);
    updateScrollBar();
}

void FrameTimeline::setScrollOffset(s32 offset) {
    const s32 maxOffset = std::max(0, contentWidth() - width());
    mScrollOffset = std::clamp(offset, 0, maxOffset);
    updateScrollBar();
    update();
}

void FrameTimeline::updateScrollBar() {
    mScrollBar->setRange(contentWidth(), width(), mScrollOffset);
    mScrollBar->setEnabled(!mModel.isEffectivelyStatic());
}

void FrameTimeline::fitToWidth() {
    mPixelsPerTick = minimumPixelsPerTick();
    setScrollOffset(0);
}

void FrameTimeline::adjustContent() {
    mPixelsPerTick = std::max(mPixelsPerTick, minimumPixelsPerTick());
    setScrollOffset(mScrollOffset);
}

f32 FrameTimeline::minimumPixelsPerTick() const {
    const s32 total = mModel.totalTicks();
    const s32 viewWidth = width();

    if (total <= 0 || viewWidth <= 0) {
        return 1.0f;
    }

    const s32 availableWidth = viewWidth - sSidePadding * 2;

    if (availableWidth <= 0) {
        return 1.0f;
    }

    return std::max(1.0f, static_cast<f32>(availableWidth) / static_cast<f32>(total));
}

s32 FrameTimeline::contentWidth() const {
    return tickToPixel(mModel.totalTicks()) + sSidePadding * 2;
}

// Conversion

s32 FrameTimeline::tickToPixel(s32 tick) const {
    return static_cast<s32>(static_cast<f32>(tick) * mPixelsPerTick);
}

s32 FrameTimeline::contentTickX(s32 tick) const {
    return viewX(sSidePadding + tickToPixel(tick));
}

s32 FrameTimeline::viewX(s32 contentX) const {
    return contentX - mScrollOffset;
}

bool FrameTimeline::isTickRangeVisible(s32 start, s32 end) const {
    const s32 startX = contentTickX(start);
    const s32 endX   = contentTickX(end);

    return endX >= 0 && startX <= width();
}

template <TickCallback Func>
void FrameTimeline::forEachVisibleTick(s32 visStart, s32 visEnd, Func&& callback) const {
    const s32 totalTicks = mModel.totalTicks();
    if (totalTicks <= 0 || mPixelsPerTick <= 0) {
        return;
    }

    const s32 majorStep = std::max(1, static_cast<s32>(60.0f / mPixelsPerTick));

    s32 firstTick = std::max(0, static_cast<s32>((static_cast<f32>(visStart - sSidePadding)) / mPixelsPerTick));

    const f32 lastTickF = std::ceilf(static_cast<f32>(visEnd - sSidePadding) / mPixelsPerTick) + 1.0f;
    const s32 lastTick = std::min(totalTicks + 1, static_cast<s32>(lastTickF));

    for (s32 t = firstTick; t < lastTick; ++t) {
        const s32 tickScreenX = contentTickX(t);
        if (tickScreenX < -1 || tickScreenX > width() + 1) {
            continue;
        }
        callback(t, tickScreenX, t % majorStep == 0);
    }
}


// ========================================================================== //


} // namespace PtclEditor
