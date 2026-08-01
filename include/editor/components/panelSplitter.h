#pragma once

#include "editor/components/collapsiblePanel.h"
#include "typedefs.h"

#include <QSet>
#include <QSize>
#include <QSplitter>


namespace PtclEditor {


// ========================================================================== //


class PanelSplitter final : public QSplitter {
public:
    static constexpr s32 sHandleWidth = 8;

public:
    explicit PanelSplitter(Qt::Orientation orientation, QWidget* parent = nullptr);

    void addWidget(QWidget* widget);
    void insertWidget(s32 index, QWidget* widget);

protected:
    QSplitterHandle* createHandle() override;

private:
    void connectPanel(QWidget* widget);
    void onPanelCollapsed(CollapsiblePanel* panel, bool collapsed);

    static void redistributeSizes(QList<s32>& sizes, s32 fixedIndex, s32 targetSize);

private:
    QSet<const CollapsiblePanel*> mConnectedPanels;

    static constexpr s32 sRestorePadding = 8;
};


// ========================================================================== //


class PanelSplitterHandle final : public QSplitterHandle {
public:
    PanelSplitterHandle(Qt::Orientation orientation, PanelSplitter* parent);

    void setIndex(s32 index);
    void refreshCursor();

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    CollapsiblePanel* panelAt(s32 index) const;
    bool hasCollapsedNeighbor() const;

    QRect collapseClickArea(bool above) const;

    s32 handlePosition() const;

    void drawBackground(QPainter& painter);
    void drawGrip(QPainter& painter);

private:
    bool mHovered{false};
    s32 mIndex{-1};

    static constexpr s32 sGripMaxLength = 128;
    static constexpr s32 sGripThickness = 2;
};


// ========================================================================== //


} // namespace PtclEditor
