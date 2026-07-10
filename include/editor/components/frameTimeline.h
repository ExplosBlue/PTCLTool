#pragma once

#include "util/paintUtil.h"

#include <QScrollBar>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ViewportScrollBar;


// ========================================================================== //


enum class FrameAnimMode {
    FixedSpeed = 0,
    FitToLife  = 1
};

static constexpr s32 sLifeInfinite = std::numeric_limits<s32>::max();

struct FrameTimelineModel {
    std::vector<s32> frameTable{};
    s32 frameCount{0};
    s32 maxFrameId{0};
    FrameAnimMode mode{FrameAnimMode::FixedSpeed};
    s32 frameStep{1};
    s32 ptclLife{100};

public:
    s32 totalTicks() const;
    s32 activeDurationTicks() const;

    s32 ticksForFrame(s32 frameIdx) const;
    s32 cellStartTick(s32 frameIdx) const;
    s32 cycleLength() const;
    bool isEffectivelyStatic() const;

private:
    s32 totalTicksFixedSpeed() const;
    s32 totalTicksFitToLife() const;

    s32 frameStartTickFixedSpeed(s32 frameIdx) const;
    s32 frameStartTickFitToLife(s32 frameIdx) const;
};


// ========================================================================== //


template <typename Func>
concept TickCallback = requires(Func func, s32 tick, s32 screenX, bool major) {
    { func(tick, screenX, major) } -> std::same_as<void>;
};


// ========================================================================== //


class FrameTimeline final : public QWidget {
    Q_OBJECT
public:
    explicit FrameTimeline(QWidget* parent = nullptr);

    void setSource(const TextureFrameInfo& frameInfo);
    void setFrames(const std::vector<s32>& frames, s32 frameCount);
    void setAnimMode(FrameAnimMode mode);
    void setFrameStep(s32 step);
    void setPtclLife(s32 life);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void frameEdited(s32 index, s32 value);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    struct CellStyle {
        QColor background;
        QColor border;
        QColor text;
        f32 thumbnailOpacity{1.0f};
    };

    struct CellLayout {
        QRect cellRect;
        QPoint thumbPos;
        QRect textRect;
    };

    struct CellDrawInfo {
        s32 startTick;
        s32 endTick;
        s32 frameIndex;

        bool isGhost{false};
        bool isStatic{false};

        s32 repetition{0};

        bool hasLifetimeSplit{false};
        s32 lifetimeTick{0};
    };

    struct FrameCycle {
        s32 startTick;
        s32 endTick;
        s32 index;

        bool isGhost() const {
            return index > 0;
        }
    };

private:
    // Drawing
    void drawTimeline(QPainter& painter, s32 trackY, s32 trackH);
    void drawTimelineStatic(QPainter& painter, s32 trackY, s32 trackH);
    void drawTimelineFixed(QPainter& painter, s32 trackY, s32 trackH);
    void drawTimelineFit(QPainter& painter, s32 trackY, s32 trackH);

    void drawFrameCycle(QPainter& painter, s32 trackY, s32 trackH, const FrameCycle& cycle);

    void drawTimelineCell(QPainter& painter, s32 trackY, s32 trackH, const CellDrawInfo& drawInfo);
    void drawCellBackground(QPainter& painter, const QRect& rect, const CellStyle& style, const CellDrawInfo& info);
    CellStyle getCellStyle(const CellDrawInfo& info) const;
    CellLayout makeCellLayout(s32 screenX, s32 screenWidth, s32 trackY, s32 trackH) const;

    void drawGrid(QPainter& painter, s32 trackY, s32 trackH, s32 visStart, s32 visEnd);
    void drawRuler(QPainter& painter, s32 visStart, s32 visEnd);
    void drawRulerActiveRange(QPainter& painter);
    void drawLifeBoundary(QPainter& painter, s32 trackY, s32 trackH);
    void drawTimelinePadding(QPainter& painter, s32 regionHeight);
    void drawInfinityOverlay(QPainter& painter, s32 trackY, s32 trackH);
    void drawInfinityMarker(QPainter& painter, s32 trackY, s32 trackH);

    // Interaction
    void showFrameSelector(s32 cellIndex);

    bool inTimelineArea(const QPoint& pos) const;
    QRect cellRect(s32 index) const;
    s32 cellAt(const QPoint& pos) const;

    // Tooltip
    QString repetitionTooltip(s32 contentX) const;
    QString boundaryTooltip(const QPoint& pos, s32 contentX) const;

    // View
    void updateZoom(s32 viewportWidth);
    void zoomAt(s32 screenX, bool zoomIn);

    void setScrollOffset(s32 offset);
    void updateScrollBar();

    void fitToWidth();
    void adjustContent();

    f32 minimumPixelsPerTick() const;
    s32 contentWidth() const;

    // Conversion
    s32 tickToPixel(s32 tick) const;
    s32 contentTickX(s32 tick) const;
    s32 viewX(s32 contentX) const;
    bool isTickRangeVisible(s32 start, s32 end) const;

    template <TickCallback Func>
    void forEachVisibleTick(s32 visStart, s32 visEnd, Func&& callback) const;

private:
    FrameTimelineModel mModel{};
    ViewportScrollBar* mScrollBar{nullptr};

    TextureFrameInfo mFrameInfo{};

    f32 mPixelsPerTick{1.0f};
    s32 mScrollOffset{0};

    s32 mPopupCellIndex{-1};
    s32 mPopupPreviewFrame{-1};

    static constexpr s32 sRulerHeight = 18;
    static constexpr s32 sTimelineHeight = 76;
    static constexpr s32 sThumbSize = 28;

    static constexpr s32 sHorizontalPadding = 4;
    static constexpr s32 sScrollbarHeight = 14;
    static constexpr s32 sSidePadding = 40;

    static constexpr f32 sMaxPixelsPerTick = 200.0f;

    static constexpr s32 sInvalidCell = -1;

    static constexpr QColor sWarnColor{220, 120, 0};
};


// ========================================================================== //


} // namespace PtclEditor
