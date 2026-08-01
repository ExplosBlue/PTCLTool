#include "editor/components/panelSplitter.h"

#include "editor/components/collapsiblePanel.h"

#include <QEnterEvent>
#include <QPainter>

#include <functional>


namespace PtclEditor {


// ========================================================================== //


PanelSplitter::PanelSplitter(Qt::Orientation orientation, QWidget* parent) :
    QSplitter{orientation, parent} {
    setHandleWidth(sHandleWidth);
    setChildrenCollapsible(false);
}

QSplitterHandle* PanelSplitter::createHandle() {
    auto* handle = new PanelSplitterHandle(orientation(), this);
    handle->setIndex(count() - 1);

    return handle;
}

void PanelSplitter::addWidget(QWidget* widget) {
    QSplitter::addWidget(widget);
    connectPanel(widget);
}

void PanelSplitter::insertWidget(s32 index, QWidget* widget) {
    QSplitter::insertWidget(index, widget);
    connectPanel(widget);
}

void PanelSplitter::connectPanel(QWidget* widget) {
    auto* panel = qobject_cast<CollapsiblePanel*>(widget);
    if (!panel || mConnectedPanels.contains(panel)) {
        return;
    }

    mConnectedPanels.insert(panel);
    connect(panel, &CollapsiblePanel::collapsedChanged, this, [this, panel](bool collapsed) {
        onPanelCollapsed(panel, collapsed);
    });
}

void PanelSplitter::redistributeSizes(QList<s32>& sizes, s32 fixedIndex, s32 targetSize) {
    const s32 delta = sizes[fixedIndex] - targetSize;

    if (delta == 0) {
        return;
    }

    s32 othersTotal = 0;
    for (s32 i = 0; i < sizes.size(); ++i) {
        if (i != fixedIndex) {
            othersTotal += sizes[i];
        }
    }
    if (othersTotal <= 0) {
        return;
    }

    const s32 amount = qAbs(delta);
    for (s32 i = 0; i < sizes.size(); ++i) {
        if (i == fixedIndex) {
            continue;
        }
        const s32 share = qRound(static_cast<qreal>(sizes[i]) / static_cast<qreal>(othersTotal) * static_cast<qreal>(amount));
        if (delta > 0) {
            sizes[i] += share;
        } else {
            sizes[i] = qMax(0, sizes[i] - share);
        }
    }
};

void PanelSplitter::onPanelCollapsed(CollapsiblePanel* panel, bool collapsed) {
    const s32 index = indexOf(panel);
    if (index < 0) {
        return;
    }

    auto sizes = this->sizes();
    const auto total = std::ranges::fold_left(sizes, 0, std::plus{});
    if (total <= 0) {
        return;
    }

    const s32 collapsedSize = panel->collapsedHeight();
    const s32 expandedMin = panel->expandedMinHeight();

    const s32 fallback = static_cast<s32>(total / sizes.size());
    const s32 target = collapsed ? collapsedSize : qBound(collapsedSize, panel->mRestoreSize > 0 ? panel->mRestoreSize : fallback, total);

    if (collapsed) {
        panel->mRestoreSize = qMax(sizes[index], expandedMin + sRestorePadding);
    }

    redistributeSizes(sizes, index, target);
    sizes[index] = target;
    setSizes(sizes);

    panel->setMaximumHeight(collapsed ? collapsedSize : QWIDGETSIZE_MAX);

    if (QSplitterHandle* h = handle(index - 1)) {
        h->update();
        static_cast<PanelSplitterHandle*>(h)->refreshCursor();
    }
    if (QSplitterHandle* h = handle(index)) {
        h->update();
        static_cast<PanelSplitterHandle*>(h)->refreshCursor();
    }
}


// ========================================================================== //


void PanelSplitterHandle::setIndex(s32 index) {
    mIndex = index;
}

PanelSplitterHandle::PanelSplitterHandle(Qt::Orientation orientation, PanelSplitter* parent) :
    QSplitterHandle{orientation, parent} {
    refreshCursor();
}

void PanelSplitterHandle::refreshCursor() {
    setCursor(hasCollapsedNeighbor()
        ? Qt::ArrowCursor
        : (orientation() == Qt::Horizontal ? Qt::SplitHCursor : Qt::SplitVCursor));
}

QSize PanelSplitterHandle::sizeHint() const {
    return orientation() == Qt::Horizontal
        ? QSize(PanelSplitter::sHandleWidth, 1)
        : QSize(1, PanelSplitter::sHandleWidth);
}

s32 PanelSplitterHandle::handlePosition() const {
    return mIndex;
}

QRect PanelSplitterHandle::collapseClickArea(bool above) const {
    QRect area = rect();

    if (orientation() == Qt::Horizontal) {
        area.setWidth(width() / 2);
        if (!above) {
            area.moveLeft(width() / 2);
        }
    } else {
        area.setHeight(height() / 2);
        if (!above) {
            area.moveTop(height() / 2);
        }
    }
    return area;
}

CollapsiblePanel* PanelSplitterHandle::panelAt(s32 index) const {
    if (index < 0 || index >= splitter()->count()) {
        return nullptr;
    }

    return qobject_cast<CollapsiblePanel*>(splitter()->widget(index));
}

bool PanelSplitterHandle::hasCollapsedNeighbor() const {
    const CollapsiblePanel* above = panelAt(mIndex);
    if (above && above->isCollapsed()) {
        return true;
    }

    const CollapsiblePanel* below = panelAt(mIndex + 1);
    return below && below->isCollapsed();
}

void PanelSplitterHandle::drawBackground(QPainter& painter) {
    const QPalette pal = palette();

    const bool highlighted = mHovered && !hasCollapsedNeighbor();

    const QColor bg = highlighted
        ? pal.color(QPalette::Highlight)
        : pal.color(QPalette::Mid);

    painter.fillRect(rect(), bg);
}

void PanelSplitterHandle::drawGrip(QPainter& painter) {
    painter.save();

    painter.setPen(Qt::NoPen);
    const QPalette pal = palette();
    const bool horizontal = orientation() == Qt::Horizontal;

    const auto color = mHovered ? pal.color(QPalette::HighlightedText) : pal.color(QPalette::Button);

    painter.setPen(color);
    painter.setPen(QPen{color, sGripThickness});
    painter.setBrush(color);

    if (horizontal) {
        const s32 cx = width() / 2;
        const s32 cy = height() / 2;

        const s32 lineSize = std::min(height() / 3, sGripMaxLength);
        painter.drawLine(cx, cy - (lineSize / 2), cx, cy + (lineSize / 2));

    } else {
        const s32 cx = width() / 2;
        const s32 cy = height() / 2;

        const s32 lineSize = std::min(width() / 3, sGripMaxLength);
        painter.drawLine(cx - (lineSize / 2), cy, cx + (lineSize / 2), cy);
    }

    painter.restore();
}

void PanelSplitterHandle::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    drawBackground(painter);
    if (!hasCollapsedNeighbor()) {
        drawGrip(painter);
    }
}

void PanelSplitterHandle::enterEvent(QEnterEvent* event) {
    QSplitterHandle::enterEvent(event);
    mHovered = true;
    update();
}

void PanelSplitterHandle::leaveEvent(QEvent* event) {
    QSplitterHandle::leaveEvent(event);
    mHovered = false;
    update();
}


// ========================================================================== //


} // namespace PtclEditor
