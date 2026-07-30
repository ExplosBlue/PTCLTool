#pragma once

#include "editor/components/thumbnailWidget.h"

#include <QImage>
#include <QPoint>
#include <QRect>
#include <QTransform>


// ========================================================================== //


enum class ResizeEdge : u8 {
    None   = 0,
    Left   = 1 << 0,
    Right  = 1 << 1,
    Top    = 1 << 2,
    Bottom = 1 << 3,

    TopLeft     = Top | Left,
    TopRight    = Top | Right,
    BottomLeft  = Bottom | Left,
    BottomRight = Bottom | Right,
};

constexpr bool hasFlag(ResizeEdge value, ResizeEdge flag) {
    return (static_cast<u8>(value) & static_cast<u8>(flag)) != 0;
}


// ========================================================================== //


class CropWidget : public ThumbnailWidget {
    Q_OBJECT
public:
    explicit CropWidget(QWidget* parent = nullptr);

    void setImage(const QImage& image);
    void setCropEnabled(bool enabled);
    void setCropRect(s32 x, s32 y, s32 w, s32 h);
    void setCropRect(const QRect& rect);
    QRect cropRect() const;

signals:
    void cropChanged(QRect rect);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    QPointF widgetToImage(QPoint widgetPos) const;

private:
    void clearCrop();

    bool canCrop() const;
    bool hasValidCrop() const;

    bool handleDrag(QMouseEvent* event);
    bool handleResize(QMouseEvent* event);
    void updateHoverCursor(QMouseEvent* event);

    ResizeEdge edgeAt(QPoint widgetPos) const;
    void drawHandle(QPainter& painter, QPoint center);

    QRect imageToWidget(const QRect& imageRect) const;
    QTransform imageToWidgetTransform() const;

    QRect resizeRect(ResizeEdge edge, QPoint imagePos, const QRect& original, bool snapToValid) const;

private:
    QImage mSourceImage{};
    QRect mCropRect{};
    bool mCropEnabled{false};

    bool mIsDragging{false};
    s32 mDragOrigX{0};
    s32 mDragOrigY{0};
    QPoint mDragStartWidget{};

    ResizeEdge mResizeEdge{ResizeEdge::None};
    QRect mResizeOrigRect{};

    static constexpr s32 sResizeHandleSize{8};
};


// ========================================================================== //
