#pragma once

#include "typedefs.h"

#include <QPoint>
#include <QScrollBar>
#include <QSize>
#include <QWidget>
#include <QWheelEvent>


// ========================================================================== //


class ThumbnailWidget : public QWidget {
    Q_OBJECT
public:
    explicit ThumbnailWidget(QWidget* parent = nullptr);

    void setPixmap(const QPixmap& pixmap);
    void setThumbnailSize(QSize size);
    void setZoomEnabled(bool enabled);

    void clear();
    void resetView();

    f32 zoom() const { return mZoom; }

    void zoomIn();
    void zoomOut();

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    QSize sizeHint() const override;

protected:
    void calcTextureRect();
    void updateScrollBars();

    void drawBackground(QPainter& painter) const;
    void drawCheckerboard(QPainter& painter) const;
    void drawTexture(QPainter& painter) const;

protected:
    QPixmap mPixmap{};
    QSize mThumbnailSize{64, 64};
    QRect mTextureRect{};

    f32 mZoom{1.0f};
    bool mZoomEnabled{false};
    QPoint mScrollOffset{0, 0};

    QScrollBar* mHScrollBar{nullptr};
    QScrollBar* mVScrollBar{nullptr};
};


// ========================================================================== //
